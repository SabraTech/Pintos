#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void* pop (void **esp);

void halt (void) NO_RETURN;
void exit (int status) NO_RETURN;
void exit_ (void **esp, uint32_t *eax);
// pid_t exec (const char *file);
int wait (pid_t);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd);
int read (int fd, void *buffer, unsigned length);
int write (int fd, const void *buffer, unsigned length);
void write_ (void **esp, uint32_t *eax);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void*
pop (void **esp)
{
  void *ret = (void*) *esp;
  *esp += sizeof (void*);
  return ret;
}

static void
syscall_handler (struct intr_frame *f) 
{
  void *esp = f->esp;
  int syscall_num = *(int*) pop (&esp);
  printf ("system call! %d\n", syscall_num);
  switch (syscall_num) {
    case SYS_WRITE:
      write_ (&esp, &f->eax); break;
    case SYS_EXIT:
      exit_ (&esp, &f->eax); break;
  }
}

void
write_ (void **esp, uint32_t *eax)
{
  int fd = *(int*) pop (esp);
  void *buffer = pop (esp);
  unsigned length = *(unsigned*) pop (esp);
  
  *eax = write (fd, buffer, length);
}

void
exit_ (void **esp, uint32_t *eax)
{
  exit (0);
}

void 
exit (int status)
{
  struct thread *cur = thread_current();
  // if (thread_alive(cur->parent))
  //   {
  //     cur->cp->status = status;
  //   }
  //printf ("%s: exit(%d)\n", cur->name, status);
  thread_exit();
}

int 
write (int fd, const void *buffer, unsigned length)
{
  if (fd == 1) 
    {
      putbuf (buffer, length);
      return length;
    }
  return -1;
}
