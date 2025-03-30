#include "sender.h"
#include "constants.h"

#include <mqueue.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <time.h>

#define EXIT_MSG   \
    (message_t)    \
    {              \
        3, "EOF\0" \
    }

static inline void handle_error(char *error_msg)
{
    perror(error_msg);
    exit(EXIT_FAILURE);
}

struct timespec start, end;
double time_taken = 0.0f;

struct sembuf sem_op;
int semid_send, semid_recv;

int shm_fd = -1;
char *data = NULL;

mqd_t mq_fd = -1;
struct mq_attr mqAttr;

bool init_mq()
{
    mqAttr.mq_maxmsg = 1;
    mqAttr.mq_msgsize = MAX_DATA_SIZE;
    mqAttr.mq_flags = O_NONBLOCK;
    mqAttr.mq_curmsgs = 0;
    mq_fd = mq_open(MQ_NAME, O_WRONLY | O_CREAT | O_NONBLOCK, 0644, &mqAttr);
    // if (mq_close(mq_fd) == -1) handle_error("mq_close failed");
    // if (mq_unlink(MQ_NAME) == -1) handle_error("mq_unlink failed");
    // mq_fd = mq_open(MQ_NAME, O_WRONLY | O_CREAT | O_NONBLOCK, 0644, &mqAttr);
    if (mq_fd < 0) {
        perror("mq_open failed");
        return false;
    }
    return true;
}

void finalize_mq()
{
    if (mq_close(mq_fd) == -1)
        handle_error("mq_close failed");
    if (mq_unlink(MQ_NAME) == -1)
        handle_error("mq_unlink failed");
}

bool init_shm()
{
    shm_fd = shm_open(SHM_FILE, O_CREAT | O_RDWR, 0777);
    if (shm_fd < 0) {
        perror("shm_open failed");
        return false;
    }
    ftruncate(shm_fd, MAX_DATA_SIZE);
    data = (char *) mmap(NULL, 1024, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd,
                         0);
    if (!data) {
        perror("mmap failed");
        return false;
    }
    return true;
}

void finalize_shm()
{
    if (close(shm_fd) == -1)
        handle_error("close failed");
    if (munmap(data, MAX_DATA_SIZE) == -1)
        handle_error("munmap failed");
    if (shm_unlink(SHM_FILE) == -1)
        handle_error("shm_unlink failed");
}

static inline int min(int a, int b)
{
    return a < b ? a : b;
}

void send(message_t message, mailbox_t *mailbox_ptr)
{
    message.length = min(message.length + 1, MAX_DATA_SIZE);

    // decrease sender semaphore
    sem_op.sem_op = -1;
    if (semop(semid_send, &sem_op, 1) == -1)
        handle_error("semop sender failed");

    // send message
    clock_gettime(CLOCK_MONOTONIC, &start);
    if (mailbox_ptr->flag == FLAG_MSG_PASS) {
        if (mq_send(mailbox_ptr->storage.msqid, message.data, message.length,
                    0) == -1)
            handle_error("mq_send failed");
    } else if (mailbox_ptr->flag == FLAG_SHARED_MEM) {
        // sprintf(mailbox_ptr->storage.shm_addr, "%s", message.data);
        memcpy(mailbox_ptr->storage.shm_addr, message.data, message.length);
    } else
        handle_error("Invalid flag\n");
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_taken +=
        (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;

    // increase receiver semaphore
    sem_op.sem_op = 1;
    if (semop(semid_recv, &sem_op, 1) == -1)
        handle_error("semop receiver failed");
}

int main(int argc, char **argv)
{
    if (argc < 3)
        handle_error("No enough args\n");

    int type = atoi(argv[1]);
    char *filename = argv[2];

    mailbox_t mailbox;
    mailbox.flag = type;

    // init sem_id to 0
    semid_send = semget((key_t) SEM_KEY_SEND, 1, 0666 | IPC_CREAT);
    semid_recv = semget(SEM_KEY_RECV, 1, 0666 | IPC_CREAT);
    if (semid_send == -1 || semid_recv == -1)
        handle_error("semget failed");
    if (semctl(semid_send, 0, SETVAL, 1) == -1 ||
        semctl(semid_recv, 0, SETVAL, 0) == -1)
        handle_error("semctl SETVAL failed");

    // set up sem_op
    sem_op.sem_num = 0;
    sem_op.sem_flg = 0;

    if (type == FLAG_MSG_PASS) {
        printf(CYAN "Message Passing\n" RESET);
        if (!init_mq())
            return -1;
        mailbox.storage.msqid = mq_fd;
    } else if (type == FLAG_SHARED_MEM) {
        printf(CYAN "Shared Memory\n" RESET);
        if (!init_shm())
            return -1;  // init shm_fd and data for shared memory
        mailbox.storage.shm_addr = data;
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
        handle_error("File not found\n");

    size_t len;
    char *line = NULL;

    while (getline(&line, &len, fp) != -1) {
        // replacing \r\n to \0
        while (line[strlen(line) - 1] == '\n' ||
               line[strlen(line) - 1] == '\r') {
            line[strlen(line) - 1] = '\0';
        }
        message_t msg = (message_t){strlen(line), line};
        send(msg, &mailbox);
        printf(CYAN "Sending message: " RESET "%s\n", msg.data);
    }

    send(EXIT_MSG, &mailbox);
    printf(RED "End of input file! exit!\n" RESET);

    // wait for reciever fninish last data
    sem_op.sem_op = -1;
    if (semop(semid_send, &sem_op, 1) == -1)
        handle_error("semop receiver failed");

    if (line)
        free(line);
    fclose(fp);

    // remove semaphores
    semctl(semid_send, 0, IPC_RMID);
    semctl(semid_recv, 0, IPC_RMID);

    if (type == FLAG_MSG_PASS)
        finalize_mq();
    else if (type == FLAG_SHARED_MEM)
        finalize_shm();

    printf("Total time taken in sending msg: %lf s\n", time_taken);

    return 0;
}