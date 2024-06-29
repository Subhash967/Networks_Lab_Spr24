#include"msocket.h"



int m_socket(int domain, int type, int protocol){

    // initialize_semaphores();
    // initialize_shared_memory();

    typedef int semaphore;

    semaphore sem1;
    semaphore sem2;

    int shmid_sock_info;
    int shmid_mtp;

    SOCK_INFO *sock_info;
    mtp_socket_t *shared_mtp_sockets;

    key_t k1 = ftok(SEM_K_PATH, SEM_K_ID1);
    if (k1 == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    key_t k2 = ftok(SEM_K_PATH, SEM_K_ID2);
    if (k2 == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    sem1 = semget(k1, 1, IPC_CREAT| 0666);
    if (sem1 == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    sem2 = semget(k2, 1, IPC_CREAT| 0666);
    if (sem2 == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    
    key_t k3=ftok(SHM_K_PATH,SHM_K_ID1);
    if(k3==-1){
        perror("ftok shared memory key1");
        exit(EXIT_FAILURE);
    }

    key_t k4=ftok(SHM_K_PATH,SHM_K_ID2);
    if(k4==-1){
        perror("ftok shared memory key2");
        exit(EXIT_FAILURE);
    }

    shmid_sock_info = shmget(k3,SHM_SOCK_INFO,IPC_CREAT |0666);
    if(shmid_sock_info==-1){
        perror("shmget key1");
        exit(EXIT_FAILURE);
    }

    shmid_mtp = shmget(k4,25*sizeof(mtp_socket_t),IPC_CREAT |0666);
    if(shmid_mtp==-1){
        perror("shmget key2");
        exit(EXIT_FAILURE);
    }

    sock_info = (SOCK_INFO *)shmat(shmid_sock_info,NULL,0);
    if(sock_info==(void *)-1){
        perror("shmat key1");
        exit(EXIT_FAILURE);
    }

    shared_mtp_sockets = (mtp_socket_t *) shmat(shmid_mtp,NULL,0);
    // if(shared_mtp_sockets==(void *)-1){
    //     perror("shmat key2");
    //     exit(EXIT_FAILURE);
    // }







    if (type != SOCK_MTP){
        perror("socket type not supported");
        errno=EAFNOSUPPORT;
        return -1;
    }
    int index = -1;
    for (int i = 1; i < MAX_SOCKETS; i++) {
        if (shared_mtp_sockets[i].is_free == 1) { // Where to initialise?
            index = i;
            break;
        }
    }
    if (index == -1) {
        errno = ENOBUFS;
        return -1;
    }

    struct sembuf sbw,sbs;

    sbw.sem_num = 0;
    sbw.sem_op = -1;
    sbw.sem_flg = 0;

    sbs.sem_num = 0;
    sbs.sem_op = 1;
    sbs.sem_flg = 0;

    // signal sem1
    semop(sem1,&sbs,1);

    // wait for sem2 signal
    semop(sem2,&sbw,1);

   
    


    // Check for error
    if (sock_info->sock_errno != 0) {
        errno = sock_info->sock_errno;
        return -1;
    }

    printf("socket created\n");

    shared_mtp_sockets[index].sock_id=sock_info->sock_id;

    // printf("%d\n",shared_mtp_sockets[index].sock_id);
    // printf("%d\n",index);

    shared_mtp_sockets[index].is_free=0;//not free

    sock_info->sock_id = 0;
    strcpy(sock_info->IP,"");
    sock_info->port=0;
    sock_info->sock_errno=0;

    return index;
}

int m_bind(int sockindex, char *source_ip, int source_port, char *dest_ip, int dest_port) {

    // initialize_semaphores();
    // initialize_shared_memory();

    typedef int semaphore;

    semaphore sem1;
    semaphore sem2;

    int shmid_sock_info;
    int shmid_mtp;

    SOCK_INFO *sock_info;
    mtp_socket_t *shared_mtp_sockets;

    key_t k1 = ftok(SEM_K_PATH, SEM_K_ID1);
    if (k1 == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    key_t k2 = ftok(SEM_K_PATH, SEM_K_ID2);
    if (k2 == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    sem1 = semget(k1, 1, IPC_CREAT| 0666);
    if (sem1 == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    sem2 = semget(k2, 1, IPC_CREAT| 0666);
    if (sem2 == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    
     key_t k3=ftok(SHM_K_PATH,SHM_K_ID1);
    if(k3==-1){
        perror("ftok shared memory key1");
        exit(EXIT_FAILURE);
    }

    key_t k4=ftok(SHM_K_PATH,SHM_K_ID2);
    if(k4==-1){
        perror("ftok shared memory key2");
        exit(EXIT_FAILURE);
    }

    shmid_sock_info = shmget(k3,SHM_SOCK_INFO,IPC_CREAT|0666);
    if(shmid_sock_info==-1){
        perror("shmget key1");
        exit(EXIT_FAILURE);
    }

    shmid_mtp = shmget(k4,25*sizeof(mtp_socket_t),IPC_CREAT|0666);
    if(shmid_mtp==-1){
        perror("shmget key2");
        exit(EXIT_FAILURE);
    }

    sock_info = (SOCK_INFO *)shmat(shmid_sock_info,NULL,0);
    if(sock_info==(void *)-1){
        perror("shmat key1");
        exit(EXIT_FAILURE);
    }

    shared_mtp_sockets = (mtp_socket_t *) shmat(shmid_mtp,NULL,0);
    // if(shared_mtp_sockets==(void *)-1){
    //     perror("shmat key2");
    //     exit(EXIT_FAILURE);
    // }

    int sock_id=shared_mtp_sockets[sockindex].sock_id;

    sock_info->sock_id=sock_id;
    strcpy(sock_info->IP,source_ip);
    
    sock_info->port=source_port;
    sock_info->sock_errno=0;

    struct sembuf sbw,sbs;

    sbw.sem_num = 0;
    sbw.sem_op = -1;
    sbw.sem_flg = 0;

    sbs.sem_num = 0;
    sbs.sem_op = 1;
    sbs.sem_flg = 0;

    // signal sem1
    semop(sem1,&sbs,1);

    // wait for sem2 signal
    semop(sem2,&sbw,1);


    // Check for error
    if (sock_info->sock_id ==-1) {
        errno = sock_info->sock_errno;
        return -1;
    }

    printf("socket binded\n");

    strcpy(shared_mtp_sockets[sockindex].dest_ip,dest_ip);
    shared_mtp_sockets[sockindex].dest_port=dest_port;

    sock_info->sock_id = 0;
    strcpy(sock_info->IP,"");
    sock_info->port=0;
    sock_info->sock_errno=0;
    return 0;
}

ssize_t m_sendto(int sockfd, char *buf, size_t len, int flags,char *dest_ip, int dest_port) {



    mtp_socket_t *shared_mtp_sockets;

    key_t k4=ftok(SHM_K_PATH,SHM_K_ID2);
    if(k4==-1){
        perror("ftok shared memory key2");
        exit(EXIT_FAILURE);
    }

    int shmid_mtp = shmget(k4,25*sizeof(mtp_socket_t),0666);
    if(shmid_mtp==-1){
        perror("shmget key2");
        exit(EXIT_FAILURE);
    }

    shared_mtp_sockets = (mtp_socket_t *) shmat(shmid_mtp,NULL,0);
    if(shared_mtp_sockets==(void *)-1){
        perror("shmat key2");
        exit(EXIT_FAILURE);
    }
    
    int sockindex = sockfd;
    if (sockindex < 0 || sockindex >= MAX_SOCKETS || ( (shared_mtp_sockets[sockindex].sendbuf_rear == shared_mtp_sockets[sockindex].sendbuf_front) && strcmp(shared_mtp_sockets[sockfd].send_buffer[shared_mtp_sockets[sockfd].sendbuf_front],"") !=0)) {
        errno= EBADF;
        return -1;
    }

    if (strncmp(shared_mtp_sockets[sockindex].dest_ip,dest_ip, sizeof(shared_mtp_sockets[sockindex].dest_ip)) != 0 || shared_mtp_sockets[sockindex].dest_port != dest_port) {
        errno=ENOTCONN;
        return -1;
    }

    printf("message %d\n",shared_mtp_sockets[sockfd].seq_no);
    printf("%s\n",buf);

    char buffer[1024];

    char bit_pattern[5]; // 4 bits + '\0' for string termination
    int s=shared_mtp_sockets[sockfd].seq_no;
    int i;
    for (i = 3; i >= 0; i--) {
        bit_pattern[i] = (s & 1) + '0'; // Extract the least significant bit
        s >>= 1; // Right shift n by 1 to get the next bit
    }
    bit_pattern[4] = '\0'; // Null-terminate the string


    sprintf(buffer,"%s%s",bit_pattern,buf);
   
    sprintf(shared_mtp_sockets[sockindex].send_buffer[shared_mtp_sockets[sockindex].sendbuf_rear],"%s",buffer);

    // printf("sendbuf_rear before %d\n",shared_mtp_sockets[sockindex].sendbuf_rear);

    shared_mtp_sockets[sockindex].sendbuf_rear = (shared_mtp_sockets[sockindex].sendbuf_rear + 1)%10;

    // printf("sendbuf_rear after %d\n",shared_mtp_sockets[sockindex].sendbuf_rear);

    

    shared_mtp_sockets[sockfd].seq_no=(shared_mtp_sockets[sockfd].seq_no+1)%16;

    // printf("seqno %d\n",shared_mtp_sockets[sockindex].seq_no);
    
    return 0;
    
}

ssize_t m_recvfrom(int sockfd, char *buf, size_t len, int flags, char *src_ip, int src_port) {

    // printf("Entered m_recvfrom\n");
    
    mtp_socket_t *shared_mtp_sockets ;

    key_t k4=ftok(SHM_K_PATH,SHM_K_ID2);
    if(k4==-1){
        perror("ftok shared memory key2");
        exit(EXIT_FAILURE);
    }

    int shmid_mtp = shmget(k4,25*sizeof(mtp_socket_t),0666);
    if(shmid_mtp==-1){
        perror("shmget key2");
        exit(EXIT_FAILURE);
    }

    

    shared_mtp_sockets = (mtp_socket_t *) shmat(shmid_mtp,NULL,0);
    if(shared_mtp_sockets==(void *)-1){
        perror("shmat key2");
        exit(EXIT_FAILURE);
    }

    // printf("Entered m_recvfrom\n");
    // printf("recv buffer front %d\n",shared_mtp_sockets[sockfd].recvbuf_front);
    // printf("recv rear front %d\n",shared_mtp_sockets[sockfd].recvbuf_rear);
    // printf("message to be taken from recv buffer front : %s\n",shared_mtp_sockets[sockfd].recv_buffer[shared_mtp_sockets[sockfd].recvbuf_front]);

    if((shared_mtp_sockets[sockfd].recvbuf_front == shared_mtp_sockets[sockfd].recvbuf_rear) && strcmp(shared_mtp_sockets[sockfd].recv_buffer[shared_mtp_sockets[sockfd].recvbuf_front],"") == 0){ // there is no message in message buffer
        // errno=ENOMSG;
        // printf("Entered no message\n");
        return -1;
    }
    else{

        printf("recieving message from recv buffer\n");
        sleep(1);

        printf("%s\n",shared_mtp_sockets[sockfd].recv_buffer[shared_mtp_sockets[sockfd].recvbuf_front]);

        strcpy(buf,shared_mtp_sockets[sockfd].recv_buffer[shared_mtp_sockets[sockfd].recvbuf_front]);
        strcpy(shared_mtp_sockets[sockfd].recv_buffer[shared_mtp_sockets[sockfd].recvbuf_front],"");//delete that message
        shared_mtp_sockets[sockfd].recvbuf_front = (shared_mtp_sockets[sockfd].recvbuf_front + 1)%5;
        shared_mtp_sockets[sockfd].rwnd.window_size ++;
        



    }



    
    return 0;

    
}

int m_close(int sockfd) {

    mtp_socket_t *shared_mtp_sockets;

    key_t k4=ftok(SHM_K_PATH,SHM_K_ID2);
    if(k4==-1){
        perror("ftok shared memory key2");
        exit(EXIT_FAILURE);
    }

    int shmid_mtp = shmget(k4,25*sizeof(mtp_socket_t),IPC_CREAT |0666);
    if(shmid_mtp==-1){
        perror("shmget key2");
        exit(EXIT_FAILURE);
    }

    shared_mtp_sockets = (mtp_socket_t *) shmat(shmid_mtp,NULL,0);
    // if(shared_mtp_sockets==(void *)-1){
    //     perror("shmat key2");
    //     exit(EXIT_FAILURE);
    // }

    // Look at SOCK_INFO
    int index = sockfd;
    // if (index < 0 || index >= MAX_SOCKETS || shared_mtp_sockets[index].sock_id == 0) {
    //     sock_info[index].sock_errno = EBADF;
    //     return -1;
    // }

    close(shared_mtp_sockets[sockfd].sock_id);
    // memset(&sock_info[index], 0, sizeof(struct SOCK_INFO));

    //cleaning up the socket
        shared_mtp_sockets[index].is_free = 1; // 1 for true
        shared_mtp_sockets[index].process_id = 0; 
        shared_mtp_sockets[index].sock_id = 0;
        shared_mtp_sockets[index].sendbuf_front = 0;
        shared_mtp_sockets[index].sendbuf_rear = 0;
        shared_mtp_sockets[index].recvbuf_front = 0;
        shared_mtp_sockets[index].recvbuf_rear = 0;
        memset(shared_mtp_sockets[index].dest_ip, '\0', sizeof(shared_mtp_sockets[index].dest_ip));
        shared_mtp_sockets[index].dest_port = 0;
        // Initialize send_buffer and recv_buffer arrays to NULL
        for (int j = 0; j < 10; j++) {
            memset(shared_mtp_sockets[index].send_buffer[j], '\0', MAX_MESSAGE_SIZE);
            
        }
        for (int j = 0; j < 5; j++) {
            memset(shared_mtp_sockets[index].recv_buffer[j], '\0', MAX_MESSAGE_SIZE);
            // shared_mtp_sockets[index].seqno_of_buffer[j]=-1;//initially there is no message in the buffer
            // shared_mtp_sockets[index].rwnd.expected_seqnos[j]=j;
        }
        shared_mtp_sockets[index].swnd.window_size=0;
        shared_mtp_sockets[index].swnd.front=0;
        shared_mtp_sockets[index].swnd.last_ACK_time=clock()/CLOCKS_PER_SEC;
        // shared_mtp_sockets[index].swnd.last_send_time=clock()/CLOCKS_PER_SEC;
        shared_mtp_sockets[index].swnd.rear=0;
        shared_mtp_sockets[index].swnd.rcv_ws_des_sock=5;
        shared_mtp_sockets[index].rwnd.window_size=5;
        shared_mtp_sockets[index].rwnd.front=0;
        shared_mtp_sockets[index].rwnd.rear=4;
        shared_mtp_sockets[index].rwnd.last_seqno_recv=-1;
        // shared_mtp_sockets[index].rwnd.nxt_seqno_to_be_taken_out_from_buffer=0;
        shared_mtp_sockets[index].rwnd.nospace=0;
        shared_mtp_sockets[index].rwnd.nxt_expected_seqno=0;
        strcpy(shared_mtp_sockets[index].rwnd.last_ack_seqno,"");
        for (int j = 0; j < 16; j++) {
           
           shared_mtp_sockets[index].swnd.sent_notack[j]=0;
        //    if(j>4){
        //     shared_mtp_sockets[index].rwnd.seq_expected[j]=0;
        //    }
        //    else {shared_mtp_sockets[index].rwnd.seq_expected[j]=1;}
            
        }

    return 0;
}

int dropMessage(float p1) {
    float rand_num = (float)rand() / RAND_MAX;
    if (rand_num < p1) {
        return 1; // Message dropped
    }
    return 0; // Message not dropped
}


