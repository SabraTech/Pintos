#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
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

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
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
  printf ("%s: exit(%d)\n", cur->name, status);
  thread_exit();
}

pid_t
exec (const char *file)
{

}

int wait (pid_t pid)
{

}
bool create (const char *file, unsigned initial_size)
{

}

bool remove (const char *file)
{

}

int open (const char *file)
{

}

int filesize (int fd)
{

}


int
read (int fd, void *buffer, unsigned length)
{
  return length;
}

int
write (int fd, const void *buffer, unsigned length)
{
  buffer = user_to_kernel_vaddr (buffer);
  if (fd == 1)
    {
      putbuf (buffer, length);
      return length;
    }
  return -1;
}

void seek (int fd, unsigned position)
{

}

unsigned tell (int fd)
{

}

void close (int fd)
{

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
