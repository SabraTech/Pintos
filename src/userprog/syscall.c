#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <list.h>
#include "threads/vaddr.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"

static void syscall_handler (struct intr_frame *);

static void *user_to_kernel_vaddr (void *uaddr);
static void halt (void) NO_RETURN;
static void exit (int status) NO_RETURN;
static pid_t exec (const char *file);
static int wait (pid_t);
static bool create (const char *file, unsigned initial_size);
static bool remove (const char *file);
static int open (const char *file);
static int filesize (int fd);
static int read (int fd, void *buffer, unsigned length);
static int write (int fd, const void *buffer, unsigned length);
static void seek (int fd, unsigned position);
static unsigned tell (int fd);
static void close (int fd);

static bool remove_from_file_table (int fd);
static struct file * get_file_by_fd (int fd);
static int add_to_file_table (struct file *f);

static struct lock filesys_lock;
static struct list file_table;

struct file_table_entry
  {
    int fd;
    struct file *f;
    struct list_elem elem;
  };

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init (&filesys_lock);
  list_init (&file_table);
}

#define SYSNUM(s) *(int*) user_to_kernel_vaddr (s)
#define ARG0(s) *(int*) user_to_kernel_vaddr (s + 4)
#define ARG1(s) *(int*) user_to_kernel_vaddr (s + 8)
#define ARG2(s) *(int*) user_to_kernel_vaddr (s + 12)

static void
syscall_handler (struct intr_frame *f)
{
  switch (SYSNUM(f->esp)) {
    case SYS_HALT:
      halt ();
      break;
    case SYS_EXIT:
      exit (ARG0(f->esp));
      break;
    case SYS_EXEC:
      f->eax = exec (ARG0(f->esp));
      break;
    case SYS_WAIT:
      f->eax = wait (ARG0(f->esp));
      break;
    case SYS_CREATE:
      f->eax = create (ARG0(f->esp), ARG1(f->esp));
      break;
    case SYS_REMOVE:
      f->eax = remove (ARG0(f->esp));
      break;
    case SYS_OPEN:
      f->eax = open (ARG0(f->esp));
      break;
    case SYS_FILESIZE:
      f->eax = filesize (ARG0(f->esp));
      break;
    case SYS_READ:
      f->eax = read (ARG0(f->esp), ARG1(f->esp), ARG2(f->esp));
      break;
    case SYS_WRITE:
      f->eax = write (ARG0(f->esp), ARG1(f->esp), ARG2(f->esp));
      break;
    case SYS_SEEK:
      seek (ARG0(f->esp), ARG1(f->esp));
      break;
    case SYS_TELL:
      f->eax = tell (ARG0(f->esp));
      break;
    case SYS_CLOSE:
      close (ARG0(f->esp));
      break;
    default:
      exit (-1);
  }
}

void
halt (void)
{
  shutdown_power_off();
}

void
exit (int status)
{
  struct thread *cur = thread_current();
  if (cur->child_elem != NULL)
    cur->child_elem->exit_status = status;
  printf ("%s: exit(%d)\n", cur->name, status);
  thread_exit();
}

pid_t
exec (const char *file)
{
  file = user_to_kernel_vaddr (file);
  char *fn_copy = palloc_get_page (0);
  if (fn_copy == NULL)
    return TID_ERROR;
  strlcpy (fn_copy, file, PGSIZE);
  
  tid_t child_tid = process_execute (fn_copy);
  if (child_tid == TID_ERROR)
    return TID_ERROR;
  
  struct child_thread_elem *child = thread_get_child (child_tid);
  if (child->loading_status == -1)
    return -1;
  
  return child_tid;
}

int 
wait (pid_t pid)
{
  return process_wait (pid);
}

bool 
create (const char *file, unsigned initial_size)
{
  file = user_to_kernel_vaddr (file);
  lock_acquire (&filesys_lock);
  bool success = filesys_create (file, initial_size);
  lock_release (&filesys_lock);
  return success;
}

bool
remove (const char *file)
{
  file = user_to_kernel_vaddr (file);
  lock_acquire (&filesys_lock);
  bool success = filesys_remove (file);
  lock_release (&filesys_lock);
  return success;
}

int open (const char *file_name)
{
  file_name = user_to_kernel_vaddr (file_name);
  lock_acquire (&filesys_lock);
  struct file *f = filesys_open (file_name);
  if (f == NULL)
    {
      lock_release (&filesys_lock);
      return -1;
    }
  int fd = add_to_file_table (f);
  lock_release (&filesys_lock);
  return fd;
}

int filesize (int fd)
{
  lock_acquire (&filesys_lock);
  struct file *f = get_file_by_fd (fd);
  lock_release (&filesys_lock);

  if (f == NULL)
    return 0; // try -1
  lock_acquire (&filesys_lock);
  int size = file_length (f);
  lock_release (&filesys_lock);
  return size;
}

int
read (int fd, void *buffer, unsigned length)
{
  buffer = user_to_kernel_vaddr (buffer);
  /* read from stdin (console) */
  if (fd == 0)
    {
      int i;
      char *cbuffer = buffer;
      for (i = 0; i < length; i++)
      	{
      	  cbuffer[i] = input_getc();
      	}
      return length;
    }
  
  /* read from file */
  lock_acquire (&filesys_lock);
  struct file *f = get_file_by_fd (fd);
  if (f == NULL)
    {
      lock_release (&filesys_lock);
      return -1; // try 0
    }
  int size = file_read (f, buffer, length);
  lock_release (&filesys_lock);
  return size;
}

int
write (int fd, const void *buffer, unsigned length)
{
  buffer = user_to_kernel_vaddr (buffer);
  /* write to stdout (console) */
  if (fd == 1)
    {
      putbuf (buffer, length);
      return length;
    }

  /* write to file */
  lock_acquire (&filesys_lock);
  struct file *f = get_file_by_fd (fd);
  if (f == NULL)
    {
      lock_release (&filesys_lock);
      return -1; // try 0
    }
  int size = file_write (f, buffer, length);
  lock_release (&filesys_lock);
  return size;
}

void seek (int fd, unsigned position)
{
  if (position < 0)
    return;

  lock_acquire (&filesys_lock);
  struct file *f = get_file_by_fd (fd);
  if (f == NULL || position < 0)
    {
      lock_release (&filesys_lock);
      return;
    }
  file_seek (f, position);
  lock_release (&filesys_lock);
}

unsigned tell (int fd)
{
  lock_acquire (&filesys_lock);
  struct file *f = get_file_by_fd (fd);
  if (f == NULL)
    {
      lock_release (&filesys_lock);
      return -1;
    }
  int pos = file_tell (f);
  lock_release (&filesys_lock);
  return pos;
}

void close (int fd)
{
  lock_acquire (&filesys_lock);
  struct file *f = get_file_by_fd (fd);
  if (f == NULL)
    {
      lock_release (&filesys_lock);
      return;
    }
  remove_from_file_table (fd);
  file_close (f);
  lock_release (&filesys_lock);
}

/* return kernel virtual address pointing to the physical address pointed to by
   uaddr, to be used in kernel code.
   If uaddr has no mapping in pdir, exits
*/
static void *
user_to_kernel_vaddr (void *uaddr)
{
  struct thread *t = thread_current ();
  void *kaddr = NULL;
  if(is_user_vaddr(uaddr))
    kaddr = pagedir_get_page (t->pagedir, uaddr);
  if (kaddr == NULL)
    exit (-1);
  return kaddr;
}

/* functions to access file_table */



/* return file object crossponding to given file descriptor, 
   if not found returns NULL 
   Note: fd = 0, 1 are reserved for stdin, stdout */
static struct file *
get_file_by_fd (int fd)
{
  struct list_elem *e = list_begin (&file_table);
  struct file_table_entry *entry;
  for(;e != list_end (&file_table); e = list_next (e))
    {
      entry = list_entry (e, struct file_table_entry, elem);
      if(entry->fd == fd)
        return entry->f;
    }
  
  return NULL;
}

/* remove the file object and fd from file_table
   returns true if successfully removed and false otherwise */
static bool
remove_from_file_table (int fd)
{
  struct list_elem *e = list_begin (&file_table);
  struct file_table_entry *entry;
  for(;e != list_end (&file_table); e = list_next (e))
    {
      entry = list_entry (e, struct file_table_entry, elem);
      if(entry->fd == fd)
        {
          list_remove (&entry->elem);
          free (entry);
          return true;
        }
    }
  return false;
}

/* adds new entry (file object) to file_table and returns its file descriptor */
static int 
add_to_file_table (struct file *f)
{
  static int next_fd = 5;
  struct file_table_entry *entry = malloc (sizeof (struct file_table_entry));
  entry->fd = next_fd++;
  entry->f = f;
  list_push_back (&file_table, &entry->elem);
  return entry->fd;
}
