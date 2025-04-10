#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    int flag;  // 1 for message passing, 2 for shared memory
    union {
        int msqid;  // for system V api. You can replace it with struecture for
                    // POSIX api
        char *shm_addr;
    } storage;
} mailbox_t;


typedef struct {
    int length;
    char *data;
} message_t;

void send(message_t message, mailbox_t *mailbox_ptr);