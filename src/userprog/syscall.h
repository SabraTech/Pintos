#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

typedef int pid_t;

void syscall_init (void);


struct file * get_file_by_fd (int fd);
int add_to_file_table (struct file *f);


#endif /* userprog/syscall.h */
