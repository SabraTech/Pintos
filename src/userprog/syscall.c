#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

int pop (int **stack);

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
void read_ (void **esp, uint32_t *eax);
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

int
pop (int **stack)
{
  int ret = *(int *) *stack;
  *stack += sizeof (int *);
  return ret;
}

static void
syscall_handler (struct intr_frame *f) 
{
  int *stack = f->esp;
  int syscall_num = *(int*)stack;
  printf ("in system call! %d - esp: %x - stack: %x\n", syscall_num, f->esp, stack);
  switch (syscall_num) {
    case SYS_WRITE:
      {
        // write_ (&stack, &f->eax); break;
        stack  += 4;
  			int fd = *(int*)stack;
  			stack  += 4;
  			void * buffer = *(char **)stack;
  			stack  += 4;
  			unsigned size = *(unsigned*)stack;
  			if(fd ==1 )
  			  putbuf((char *)buffer, size);
        f->eax = size;
        
   			break;
      }
    case SYS_EXIT:
      {
        stack  += 4;
        int status = *(int*)stack;
        exit (status);
        break;
      }
  }
  printf ("out system call! %d - esp: %x - stack: %x\n", syscall_num, f->esp, stack);
}

void
write_ (void **esp, uint32_t *eax)
{
  int fd = pop (esp);
  void *buffer = pop (esp);
  unsigned length = pop (esp);
  
  *eax = write (fd, "b", 2);
}

void
read_ (void **esp, uint32_t *eax)
{
  int fd = pop (esp);
  void *buffer = *(char **) pop (esp);
  unsigned length = pop (esp);
  
  *eax = read (fd, buffer, length);
}


void
exit_ (void **esp, uint32_t *eax)
{
  
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

int
read (int fd, void *buffer, unsigned length)
{
  return length;
}
