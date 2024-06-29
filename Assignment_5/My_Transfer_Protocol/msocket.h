#ifndef MSOCKET_H
#define MSOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>



#define ENOTBOUND 1001

#define MAX_SOCKETS 25
#define KEY_SOCK_INFO 333
#define SOCK_MTP 3
#define MAX_MESSAGE_SIZE 1024
#define MAX_WINDOW_SIZE 5

#define T 15 // Timeout value
#define p 0.05

#define SEM_K_PATH "."
#define SEM_K_ID1 3333
#define SEM_K_ID2 4444

#define SHM_K_PATH "."
#define SHM_K_ID1 5555
#define SHM_K_ID2 6666

typedef struct {
    int sock_id;
    char IP[16];
    int port;
    int sock_errno;
} SOCK_INFO;

typedef struct {
        int sent_notack[16];
        int window_size;
        int front;
        int rear;
        time_t last_ACK_time;
        // double last_send_time;
        int rcv_ws_des_sock;
} send_window;

typedef struct {
    // int seq_expected[16];
    int window_size;
    int front;
    int rear;
    int last_seqno_recv;
    // int expected_seqnos[5];
    int nxt_expected_seqno;
    // int nxt_seqno_to_be_taken_out_from_buffer;
    int nospace;
    char last_ack_seqno[5];
    
} recieve_window;

typedef struct{
    int is_free;
    int flag;
    int seq_no;
    int sendbuf_front;
    int sendbuf_rear;
    int recvbuf_front;
    int recvbuf_rear;
    int process_id;
    int sock_id;
    char dest_ip[16];
    int dest_port;
    char send_buffer[10][MAX_MESSAGE_SIZE];
    char recv_buffer[5][MAX_MESSAGE_SIZE];
    
    send_window swnd;
    recieve_window rwnd;
}mtp_socket_t;

#define SHM_SOCK_INFO sizeof(SOCK_INFO)
// #define SHM_MTP_SOCKETS (MAX_SOCKETS*sizeof(mtp_socket_t))



// void init_msocket();
// void sem_wait(int sem_id);
// void sem_signal(int sem_id);
// void initialize_semaphores();
// void initialize_shared_memory();
int m_socket(int domain, int type, int protocol);
int m_bind(int sockfd, char *source_ip, int source_port,char *dest_ip, int dest_port);
ssize_t m_sendto(int sockfd,char *buf, size_t len, int flags,char *dest_ip, int dest_port);
ssize_t m_recvfrom(int sockfd,char *buf, size_t len, int flags, char *src_ip, int src_port);
int m_close(int sockfd);
int dropMessage(float p1);

#endif 
