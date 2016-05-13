#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void signal_handler(int sig) {
    printf("Process %ld received signal %d\n", (long) getpid(), sig);
}

void debugger(pid_t child) {
    printf("In debugger process %ld\n", (long) getpid());

    if (signal( SIGCHLD, signal_handler) == SIG_ERR) {
        perror("signal");
        exit(-1);
    }

		printf("waiting\n");

    int status;
    wait(&status);

		printf("waited\n");

    caddr_t addr = (caddr_t)0x100000f31;
		printf("value at %p: %d\n", addr, ptrace(PT_READ_I, child, addr, 0));
		printf("value at %p: %d\n", addr, ptrace(PT_READ_D, child, addr, 0));
		printf("value at %p: %d\n", addr, ptrace(PT_READ_U, child, addr, 0));

    ptrace(PT_CONTINUE, child, (void*)1, 0);

    //int data = (orig_data & ~0xff) | 0xcc;
    //ptrace(PT_WRITE_I, child, (void*)addr, data);

    //ptrace(PT_CONTINUE, child, NULL, 0);
    wait(&status);

    //ptrace(PT_WRITE_I, child, (void*)addr, orig_data);
}

void debuggie(const char* path) {
    char* argv[] = { NULL };
    char* envp[] = { NULL };

    printf("In debuggie process %ld\n", (long) getpid());

    if (ptrace(PT_TRACE_ME, 0, NULL, 0)) {
        perror("ptrace");
        return;
    }

    execve(path, argv, envp);
}

int main(int argc, char* argv[]) {
		if(argc < 2)
			return -1;

    pid_t pid = fork();

    if (pid == 0)
        debuggie(argv[1]);
    else if (pid > 0)
        debugger(pid);
    else
        printf("error\n");

    return 0;
}

