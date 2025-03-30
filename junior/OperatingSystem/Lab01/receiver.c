#include "receiver.h"
#include "constants.h"

#include <mqueue.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/sem.h>

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
    mq_fd = mq_open(MQ_NAME, O_RDONLY | O_CREAT | O_NONBLOCK, 0644, &mqAttr);
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
}

static inline int min(int a, int b)
{
    return a < b ? a : b;
}

void receive(message_t *message_ptr, mailbox_t *mailbox_ptr)
{
    // decrease receiver semaphore
    sem_op.sem_op = -1;
    if (semop(semid_recv, &sem_op, 1) == -1)
        handle_error("semop receiver failed");

    // receive message
    clock_gettime(CLOCK_MONOTONIC, &start);
    if (mailbox_ptr->flag == FLAG_MSG_PASS) {
        ssize_t len_recv = mq_receive(mailbox_ptr->storage.msqid,
                                      message_ptr->data, MAX_DATA_SIZE, NULL);
        if (len_recv == -1)
            handle_error("mq_receive failed");
    } else if (mailbox_ptr->flag == FLAG_SHARED_MEM) {
        char *d = mailbox_ptr->storage.shm_addr;
        message_ptr->length = strlen(d);
        int len = min(message_ptr->length + 1, MAX_DATA_SIZE);
        memcpy(message_ptr->data, d, len);
    } else
        handle_error("Invalid flag\n");
    clock_gettime(CLOCK_MONOTONIC, &end);
    time_taken +=
        (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;

    // increase sender semaphore
    sem_op.sem_op = 1;
    if (semop(semid_send, &sem_op, 1) == -1)
        handle_error("semop sender failed");
}

int main(int argc, char **argv)
{
    if (argc < 2)
        handle_error("No enough args\n");

    // get access to the existing semaphore set
    semid_send = semget(SEM_KEY_SEND, 1, 0666);
    semid_recv = semget(SEM_KEY_RECV, 1, 0666);
    if (semid_send == -1 || semid_recv == -1)
        handle_error("semget failed");

    // set up sem_op
    sem_op.sem_num = 0;
    sem_op.sem_flg = 0;

    int type = atoi(argv[1]);
    char buffer[MAX_DATA_SIZE + 1];
    message_t msg = (message_t){0, buffer};
    mailbox_t mailbox = {.flag = type};

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

    while (true) {
        receive(&msg, &mailbox);
        if (strcmp(msg.data, "EOF") == 0)
            break;
        printf(CYAN "Recieving message: " RESET "%s\n", msg.data);
    }

    printf(RED "Sender Exit!\n" RESET);
    if (type == FLAG_MSG_PASS)
        finalize_mq();
    else if (type == FLAG_SHARED_MEM)
        finalize_shm();

    printf("Total time taken in recieving msg: %lf s\n", time_taken);

    return 0;
}