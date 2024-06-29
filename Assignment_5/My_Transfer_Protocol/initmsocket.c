#include"msocket.h"

int no_sockets=0;



struct sembuf sbw,sbs;

typedef int semaphore;

semaphore sem1;
semaphore sem2;

int shmid_sock_info;
int shmid_mtp;

SOCK_INFO *sock_info;
mtp_socket_t *shared_mtp_sockets;

int bit_pattern_to_int(const char *bit_pattern) {
    // Convert binary string to integer using strtol
    return (int)strtol(bit_pattern, NULL, 2);


}

void garbage_collector() {
    while(1){
        sleep(T/2);
        for(int i=0; i<25; i++){
            if(shared_mtp_sockets[i].sock_id != 0){
                pid_t result;
                int status;
                // result = waitpid(shared_mtp_sockets[i].process_id, &status, WNOHANG);
                if(result == 0){
                    continue;
                } else if (result == -1){
                    perror("waitpid");
                } else {
                    if(WIFEXITED(status)){
                        printf("Child process %d exited with status %d\n", result, WEXITSTATUS(status));
                        if(close(shared_mtp_sockets[i].sock_id) == -1){
                            perror("close");
                        }
                        shared_mtp_sockets[i].sock_id= 0;
                    } else if(WIFSIGNALED(status)){
                        printf("Child process %d killed by signal %d\n", result, WTERMSIG(status));
                        if(close(shared_mtp_sockets[i].sock_id) == -1){
                            perror("close");
                        }
                        shared_mtp_sockets[i].sock_id= 0;
                    } else if(WIFSTOPPED(status)){
                        printf("Child process %d stopped by signal %d\n", result, WSTOPSIG(status));
                    }
                }
            }
        }
    }
}

void *S_thread() {
   
    printf("S thread started\n");
    
    while (1) {
        
        for (int i = 1; i < MAX_SOCKETS; i++){
            if(shared_mtp_sockets[i].sock_id!=0){
            time_t current_time;
            time(&current_time);

            // printf("Current time = %ld\n",current_time);
            // printf("last ack time= %ld\n",shared_mtp_sockets[i].swnd.last_ACK_time);
            // sleep(1);
            // printf("index =%d\n",i);
            // sleep(1);

            // Check if timeout period T is over
            if (((current_time - shared_mtp_sockets[i].swnd.last_ACK_time) >= T) && (shared_mtp_sockets[i].swnd.last_ACK_time !=0 || shared_mtp_sockets[i].flag>=5)) {
                // Retransmit messages within the window that have not been acknowledged
                // printf("send buffer front %d\n",shared_mtp_sockets[i].swnd.front);
                // printf("send buffer rear %d\n",shared_mtp_sockets[i].swnd.rear);

                if(shared_mtp_sockets[i].swnd.front>shared_mtp_sockets[i].swnd.rear){

                
                    for (int j = shared_mtp_sockets[i].swnd.front; j < shared_mtp_sockets[i].swnd.rear; j++) {
                        // printf("entered retransmission %d(swnd front)\n",j);
                        // if (shared_mtp_sockets[i].swnd.sent_notack[j] == 1) {
                            // Retransmit using original sendto() call
                            
                            // Use dest_ip and dest_port for sending
                            // printf("Entered retransmission\n");

                            struct sockaddr_in dest_addr1;
                            memset(&dest_addr1, 0, sizeof(dest_addr1));
                            dest_addr1.sin_family = AF_INET;
                            inet_aton(shared_mtp_sockets[i].dest_ip, &dest_addr1.sin_addr);
                            dest_addr1.sin_port = htons(shared_mtp_sockets[i].dest_port);

                            // printf("message : %s",shared_mtp_sockets[i].send_buffer[j]);

                            sendto(shared_mtp_sockets[i].sock_id, shared_mtp_sockets[i].send_buffer[j], strlen(shared_mtp_sockets[i].send_buffer[j]), 0, (struct sockaddr *)&dest_addr1, sizeof(dest_addr1));
                        // }
                        time(&shared_mtp_sockets[i].swnd.last_ACK_time);
                    }
                }
                else{
                    for (int j = shared_mtp_sockets[i].swnd.front; j < 10; j++){
                        // printf("entered retransmission %d(swnd front)\n",j);
                        // // if (shared_mtp_sockets[i].swnd.sent_notack[j] == 1) {
                        //     // Retransmit using original sendto() call
                            
                        //     // Use dest_ip and dest_port for sending
                        //     printf("Entered retransmission\n");

                            struct sockaddr_in dest_addr1;
                            memset(&dest_addr1, 0, sizeof(dest_addr1));
                            dest_addr1.sin_family = AF_INET;
                            inet_aton(shared_mtp_sockets[i].dest_ip, &dest_addr1.sin_addr);
                            dest_addr1.sin_port = htons(shared_mtp_sockets[i].dest_port);

                            // printf("message : %s",shared_mtp_sockets[i].send_buffer[j]);

                            sendto(shared_mtp_sockets[i].sock_id, shared_mtp_sockets[i].send_buffer[j], strlen(shared_mtp_sockets[i].send_buffer[j]), 0, (struct sockaddr *)&dest_addr1, sizeof(dest_addr1));
                        // }
                        time(&shared_mtp_sockets[i].swnd.last_ACK_time);



                    }
                    for (int j = 0; j < shared_mtp_sockets[i].swnd.rear; j++){
                        // printf("entered retransmission %d(swnd front)\n",j);
                        // // if (shared_mtp_sockets[i].swnd.sent_notack[j] == 1) {
                        //     // Retransmit using original sendto() call
                            
                        //     // Use dest_ip and dest_port for sending
                        //     printf("Entered retransmission\n");

                            struct sockaddr_in dest_addr1;
                            memset(&dest_addr1, 0, sizeof(dest_addr1));
                            dest_addr1.sin_family = AF_INET;
                            inet_aton(shared_mtp_sockets[i].dest_ip, &dest_addr1.sin_addr);
                            dest_addr1.sin_port = htons(shared_mtp_sockets[i].dest_port);

                            // printf("message : %s",shared_mtp_sockets[i].send_buffer[j]);

                            sendto(shared_mtp_sockets[i].sock_id, shared_mtp_sockets[i].send_buffer[j], strlen(shared_mtp_sockets[i].send_buffer[j]), 0, (struct sockaddr *)&dest_addr1, sizeof(dest_addr1));
                        // }
                        time(&shared_mtp_sockets[i].swnd.last_ACK_time);




                    }



                }
                
            } else {
                // Check if window_size < 5 and send any pending messages in send_buffer
                // if(shared_mtp_sockets[i].swnd.last_ACK_time ==0){// initially send five

                //     int f=shared_mtp_sockets[i].sendbuf_front;
                //     int z=5;
                //     while(z>0){

                //         struct sockaddr_in dest_addr;
                //         memset(&dest_addr, 0, sizeof(dest_addr));
                //         dest_addr.sin_family = AF_INET;
                //         dest_addr.sin_addr.s_addr = inet_addr(shared_mtp_sockets[i].dest_ip); 
                //         dest_addr.sin_port = htons(shared_mtp_sockets[i].dest_port);

                //         sendto(shared_mtp_sockets[i].sock_id, shared_mtp_sockets[i].send_buffer[z], strlen(shared_mtp_sockets[i].send_buffer[z]), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
                        
                //         shared_mtp_sockets[i].swnd.window_size++;
                //         shared_mtp_sockets[i].swnd.rear++;
                //         shared_mtp_sockets[i].swnd.sent_notack[z]=1;
                        
                //         z--;




                //     }


                // }






                if (shared_mtp_sockets[i].swnd.rcv_ws_des_sock > 0 && shared_mtp_sockets[i].swnd.window_size <5) {

                    // printf("Entered\n");
                    
                    int rcvws=shared_mtp_sockets[i].swnd.rcv_ws_des_sock;
                    int sws=5-shared_mtp_sockets[i].swnd.window_size;//how many can we send such that it cannot exceed send buffer

                    int x;

                    if(rcvws<sws){ x= rcvws;}
                    else{ x= sws;}

                    // printf("x=%d\n",x);

                    //taking the minimum among them for how many we can send

                    // Check how many there are messages in send_buffer that are not in send_window
                
                    

                    int k = shared_mtp_sockets[i].swnd.rear;

                    // printf("k=%d\n",k);
                    // sleep(1);

                    // printf("shared_mtp_sockets[%d].sendbuf_rear=%d\n",i,shared_mtp_sockets[i].sendbuf_rear);


                    while( (k != shared_mtp_sockets[i].sendbuf_rear) && x>0){

                            // printf("Entered\n");

                            struct sockaddr_in dest_addr;
                            memset(&dest_addr, 0, sizeof(dest_addr));
                            dest_addr.sin_family = AF_INET;
                            inet_aton(shared_mtp_sockets[i].dest_ip, &dest_addr.sin_addr);
                            dest_addr.sin_port = htons(shared_mtp_sockets[i].dest_port);

                            sendto(shared_mtp_sockets[i].sock_id, shared_mtp_sockets[i].send_buffer[k], strlen(shared_mtp_sockets[i].send_buffer[k]), 0, (const struct sockaddr *)&dest_addr, sizeof(dest_addr));
                            shared_mtp_sockets[i].flag++;
                            char seqno_sent[5];
                            strncpy(seqno_sent,shared_mtp_sockets[i].send_buffer[k],4);
                            seqno_sent[4]='\0';

                            int sent_seqno = bit_pattern_to_int(seqno_sent);

                            shared_mtp_sockets[i].swnd.window_size++;
                            shared_mtp_sockets[i].swnd.rear = (shared_mtp_sockets[i].swnd.rear + 1)%10;
                            shared_mtp_sockets[i].swnd.sent_notack[sent_seqno]=1;
                            x--;//no of messages i can send

                            k=(k+1)%10;

                            // printf(" after k=%d\n",k);
                            


                    }
                    //     }
                        
                    // }
                } 
            }
        }
        }
        // Randomly sleep for some time < T/2
        // printf("no of messages in buffer %d\n",shared_mtp_sockets[1].sendbuf_rear);
        // printf("no of messages in the send window %d\n",shared_mtp_sockets[1].swnd.rear);
        printf("S_thread sleeping....\n");
        // int random_sleep_time = rand() % ((int)(T * 500000));
        // usleep(random_sleep_time);
        sleep(5);
    }
    return NULL;
}

void *R_thread() {

    printf("R thread started\n");
    int max = 0;
    char buffer[1024];

    // while(1){

    //     sleep(3);
    //     int j;
    //         for( j=no_sockets+1;j<MAX_SOCKETS;j++){

    //             if(shared_mtp_sockets[j].sock_id != 0){

    //                continue;
    //             }
    //             else{
    //                 break;
    //             }

    //         }
    //         no_sockets=j-1;
    //     for(int i=1;i<j;i++){
    //                 socklen_t len;
    //                 struct sockaddr_in src_addr;
    //                 memset(&src_addr, 0, sizeof(src_addr));
    //                 src_addr.sin_family = AF_INET;
    //                 src_addr.sin_addr.s_addr = inet_addr(shared_mtp_sockets[i].dest_ip); 
    //                 src_addr.sin_port = htons(shared_mtp_sockets[i].dest_port);
    //                 len=sizeof(src_addr);
    //                 memset(buffer,0,sizeof(buffer));
    //                 printf("recieving in socket %d\n",i);
    //                 int flags = fcntl(shared_mtp_sockets[i].sock_id, F_GETFL, 0);
    //                 fcntl(shared_mtp_sockets[i].sock_id, F_SETFL, flags | O_NONBLOCK);
    //                 int x =recvfrom(shared_mtp_sockets[i].sock_id,buffer,sizeof(buffer),0, (struct sockaddr *)&src_addr, &len);

    //                 if(dropMessage(0.2)==1){
    //                     printf("Dropped message %s\n",buffer);
                        
    //                 }
    //                 else{
    //                 // printf("no of messages recieved %d\n",x);
    //                 if(x!=-1){

    //                 if(strncmp(buffer,"ACK",3)==0){// it means it is a ack message,this part is fully sender
    //                     // ack format ACKseq_norecive window size of destination socket
    //                     // based on the recieve window size the sender will send accordingly

    //                     printf("Entered ack message of %d\n",i);

    //                     char seqno_firstsentmsg[5];
    //                     strncpy(seqno_firstsentmsg,shared_mtp_sockets[i].send_buffer[shared_mtp_sockets[i].swnd.front],4);
    //                     seqno_firstsentmsg[4]='\0';

    //                     int fsent_seqno = bit_pattern_to_int(seqno_firstsentmsg);

    //                     char seqno_ack[5];
    //                     seqno_ack[0]=buffer[3];
    //                     seqno_ack[1]=buffer[4];
    //                     seqno_ack[2]=buffer[5];
    //                     seqno_ack[3]=buffer[6];
    //                     seqno_ack[4]='\0';

    //                     int ack_seqno=bit_pattern_to_int(seqno_ack);
    //                     printf("%d\n",ack_seqno);
    //                     fflush(stdout);

    //                     if(shared_mtp_sockets[i].swnd.sent_notack[ack_seqno]==0 ){//duplicate ack message and if that is in send window then that sent_notack will be 1
                        
    //                         //do nothing
                        
    //                     }

    //                     if(ack_seqno == fsent_seqno){

    //                         printf("as ack recieved clearing that message in send buffer\n");
                        
    //                         time(&shared_mtp_sockets[i].swnd.last_ACK_time);

    //                         strcpy(shared_mtp_sockets[i].send_buffer[shared_mtp_sockets[i].swnd.front],"");
    //                         shared_mtp_sockets[i].swnd.front = (shared_mtp_sockets[i].swnd.front + 1)%10;
    //                         shared_mtp_sockets[i].sendbuf_front = (shared_mtp_sockets[i].sendbuf_front)%10;
    //                         shared_mtp_sockets[i].swnd.window_size--;
    //                         shared_mtp_sockets[i].swnd.sent_notack[ack_seqno]=0;


    //                     }//else drop the ack no need of it

    //                     shared_mtp_sockets[i].swnd.rcv_ws_des_sock=buffer[7];//updating recv window size of destination socket
                            

    //                 }
    //                 else{// will remove the header and put it into the front of recieve window buffer(empty space where messages can be recieved) 
    //                 //send ack message,update expected seq no ,recieve window size--,rwnd.front++,recvbuf_back++,when m_recvfrom calls recvbuf_front++ as it removes 1st message

    //                     if(shared_mtp_sockets[i].rwnd.window_size==0){
    //                         //set flag to no space                            
    //                         shared_mtp_sockets[i].rwnd.nospace=1;
    //                     }

    //                     else{
    //                         char seqno_recvmsg[5];

    //                         strncpy(seqno_recvmsg,buffer,4);
    //                         seqno_recvmsg[4]='\0';

    //                         for (int j = 0; j < 1020; j++) {
                                
    //                             buffer[j] = buffer[j + 4]; // Shift the buffer to the left by 4 positions
    //                         }
    //                         printf("%s\n",seqno_recvmsg);
                           
    //                         // Fill the remaining elements with null 
    //                         for (int j = 1020; j < 1024; j++) {
    //                             buffer[j] = '\0'; 
    //                         }

    //                         int intseqrecv=bit_pattern_to_int(seqno_recvmsg);

    //                         if( intseqrecv == shared_mtp_sockets[i].rwnd.nxt_expected_seqno){ // expected seqno is recieved

    //                             strcpy(shared_mtp_sockets[i].recv_buffer[shared_mtp_sockets[i].rwnd.front],buffer);
    //                             shared_mtp_sockets[i].rwnd.window_size--;
    //                             shared_mtp_sockets[i].rwnd.front = (shared_mtp_sockets[i].rwnd.front + 1)%5;
    //                             shared_mtp_sockets[i].recvbuf_rear = (shared_mtp_sockets[i].recvbuf_rear + 1)%5;

    //                             shared_mtp_sockets[i].rwnd.nxt_expected_seqno = ( shared_mtp_sockets[i].rwnd.nxt_expected_seqno + 1)%16;

    //                             // int seqno= bit_pattern_to_int(seqno_recvmsg);
    //                             strcpy(shared_mtp_sockets[i].rwnd.last_ack_seqno,seqno_recvmsg);

    //                             char buf[9];// constructing ACK message
    //                             sprintf(buf,"ACK%s%d",seqno_recvmsg,shared_mtp_sockets[1].rwnd.window_size);

    //                             printf("ack sent with %s\n",buf);

    //                             struct sockaddr_in dest_addr;
    //                             memset(&dest_addr, 0, sizeof(dest_addr));
    //                             dest_addr.sin_family = AF_INET;
    //                             dest_addr.sin_addr.s_addr = inet_addr(shared_mtp_sockets[i].dest_ip); 
    //                             dest_addr.sin_port = htons(shared_mtp_sockets[i].dest_port);

    //                             sendto(shared_mtp_sockets[i].sock_id,buf, strlen(buf), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));//ACK sent
    //                         }
    //                         //else drop
    //                     }

    //                 }

    //                 }
    //                 }



    //     }

    // }


    fd_set readfds,dummy;
    FD_ZERO(&readfds);
    
    struct timeval timeout;
    int retval;


    while(1){    
        
        

        char buffer[1024];

        // Initialize fd_set
        FD_ZERO(&dummy);
        dummy=readfds;
        

        // Set timeout value
        timeout.tv_sec = 5;  // 5 seconds
        timeout.tv_usec = 0;

        // Use select to check for incoming messages on sockets
        // printf("max + 1 =%d\n",max+1);
        retval = select(max+1, &dummy, NULL, NULL, &timeout);
        // printf(" recieving of message in user2 socket FD_ISSET %d\n",FD_ISSET(shared_mtp_sockets[1].sock_id, &readfds));
        printf("retval of select %d\n",retval);
        sleep(1);
        if (retval == -1) {
            perror("select()");
            exit(EXIT_FAILURE);
        } else if (retval!=0) {
            // At least one socket has data ready to be read
            for (int i = 1; i < 25; i++) {
                if(shared_mtp_sockets[i].sock_id!=0){
                    // printf("Entered socket %d\n",i);
                    sleep(1);
                    if (FD_ISSET(shared_mtp_sockets[i].sock_id, &dummy)==1) {

                        // printf("Entered \n");
                        // Receive and handle the message on this socket
                        socklen_t len;
                        struct sockaddr_in src_addr;
                        memset(&src_addr, 0, sizeof(src_addr));
                        src_addr.sin_family = AF_INET;
                        src_addr.sin_addr.s_addr = inet_addr(shared_mtp_sockets[i].dest_ip); 
                        src_addr.sin_port = htons(shared_mtp_sockets[i].dest_port);
                        len=sizeof(src_addr);
                        memset(buffer,0,sizeof(buffer));
                        printf("recieving in socket %d\n",i);
                        int flags = fcntl(shared_mtp_sockets[i].sock_id, F_GETFL, 0);
                        fcntl(shared_mtp_sockets[i].sock_id, F_SETFL, flags | O_NONBLOCK);
                        int x =recvfrom(shared_mtp_sockets[i].sock_id,buffer,sizeof(buffer),0, (struct sockaddr *)&src_addr, &len);

                        if(dropMessage(0.2)==1){
                            printf("Dropped message %s\n",buffer);
                            
                        }
                        else{
                        // printf("no of messages recieved %d\n",x);
                        if(x!=-1){

                        if(strncmp(buffer,"ACK",3)==0){// it means it is a ack message,this part is fully sender
                            // ack format ACKseq_norecive window size of destination socket
                            // based on the recieve window size the sender will send accordingly

                            // printf("Entered ack message of %d\n",i);

                            char seqno_firstsentmsg[5];
                            strncpy(seqno_firstsentmsg,shared_mtp_sockets[i].send_buffer[shared_mtp_sockets[i].swnd.front],4);
                            seqno_firstsentmsg[4]='\0';

                            int fsent_seqno = bit_pattern_to_int(seqno_firstsentmsg);

                            char seqno_ack[5];
                            seqno_ack[0]=buffer[3];
                            seqno_ack[1]=buffer[4];
                            seqno_ack[2]=buffer[5];
                            seqno_ack[3]=buffer[6];
                            seqno_ack[4]='\0';

                            int ack_seqno=bit_pattern_to_int(seqno_ack);
                            printf("%d\n",ack_seqno);
                            fflush(stdout);

                            if(shared_mtp_sockets[i].swnd.sent_notack[ack_seqno]==0 ){//duplicate ack message and if that is in send window then that sent_notack will be 1
                            
                                //do nothing
                            
                            }

                            if(ack_seqno == fsent_seqno){

                                printf("as ack recieved clearing %d message in send buffer\n",ack_seqno);
                            
                                time(&shared_mtp_sockets[i].swnd.last_ACK_time);

                                strcpy(shared_mtp_sockets[i].send_buffer[shared_mtp_sockets[i].swnd.front],"");
                                shared_mtp_sockets[i].swnd.front = (shared_mtp_sockets[i].swnd.front + 1)%10;
                                shared_mtp_sockets[i].sendbuf_front = (shared_mtp_sockets[i].sendbuf_front)%10;
                                shared_mtp_sockets[i].swnd.window_size--;
                                shared_mtp_sockets[i].swnd.sent_notack[ack_seqno]=0;


                            }//else drop the ack no need of it

                            shared_mtp_sockets[i].swnd.rcv_ws_des_sock=buffer[7];//updating recv window size of destination socket
                                

                        }
                        else{// will remove the header and put it into the front of recieve window buffer(empty space where messages can be recieved) 
                        //send ack message,update expected seq no ,recieve window size--,rwnd.front++,recvbuf_back++,when m_recvfrom calls recvbuf_front++ as it removes 1st message


                            // printf("Entered data recieving\n");

                            if(shared_mtp_sockets[i].rwnd.window_size==0){
                                //set flag to no space                            
                                shared_mtp_sockets[i].rwnd.nospace=1;
                            }

                            else{
                                char seqno_recvmsg[5];

                                strncpy(seqno_recvmsg,buffer,4);
                                seqno_recvmsg[4]='\0';

                                for (int j = 0; j < 1020; j++) {
                                    
                                    buffer[j] = buffer[j + 4]; // Shift the buffer to the left by 4 positions
                                }
                            
                                // Fill the remaining elements with null 
                                for (int j = 1020; j < 1024; j++) {
                                    buffer[j] = '\0'; 
                                }

                                int intseqrecv=bit_pattern_to_int(seqno_recvmsg);
                                // printf("recieved seqno %d\n",intseqrecv);
                                // printf("expected seqno %d\n",shared_mtp_sockets[i].rwnd.nxt_expected_seqno);

                                if( intseqrecv == shared_mtp_sockets[i].rwnd.nxt_expected_seqno){ // expected seqno is recieved

                                    strcpy(shared_mtp_sockets[i].recv_buffer[shared_mtp_sockets[i].rwnd.front],buffer);
                                    shared_mtp_sockets[i].rwnd.window_size--;
                                    shared_mtp_sockets[i].rwnd.front = (shared_mtp_sockets[i].rwnd.front + 1)%5;
                                    shared_mtp_sockets[i].recvbuf_rear = (shared_mtp_sockets[i].recvbuf_rear + 1)%5;

                                    shared_mtp_sockets[i].rwnd.nxt_expected_seqno = ( shared_mtp_sockets[i].rwnd.nxt_expected_seqno + 1)%16;

                                    // int seqno= bit_pattern_to_int(seqno_recvmsg);
                                    strcpy(shared_mtp_sockets[i].rwnd.last_ack_seqno,seqno_recvmsg);

                                    char buf[9];// constructing ACK message
                                    sprintf(buf,"ACK%s%d",seqno_recvmsg,shared_mtp_sockets[1].rwnd.window_size);

                                    printf("recvseqno is expectedseqno,so ack sent with %s\n",buf);

                                    struct sockaddr_in dest_addr;
                                    memset(&dest_addr, 0, sizeof(dest_addr));
                                    dest_addr.sin_family = AF_INET;
                                    dest_addr.sin_addr.s_addr = inet_addr(shared_mtp_sockets[i].dest_ip); 
                                    dest_addr.sin_port = htons(shared_mtp_sockets[i].dest_port);

                                    sendto(shared_mtp_sockets[i].sock_id,buf, strlen(buf), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));//ACK sent
                                }
                                //else drop
                            }

                        }

                        }
                        }


                        
                    }
                }
            }
        } else if (retval==0) {
            // Timeout occurred
            // Check for newly created sockets and set them in the select call
            int j;
            for( j=no_sockets+1;j<MAX_SOCKETS;j++){

                if(shared_mtp_sockets[j].sock_id != 0){

                    // printf("Entered %d\n",j);

                    FD_SET(shared_mtp_sockets[j].sock_id, &readfds);
                    if(shared_mtp_sockets[j].sock_id > max){
                        max = shared_mtp_sockets[j].sock_id;
                    }
                    // printf("max=%d\n",max);

                }
                else{
                    break;
                }

            }
            no_sockets=j-1;
            // printf("%d",no_sockets);
            // fflush(stdout);



            // if flag set to nospace and rwnd window size is not 0 send duplicate ack message  
            for(int j=0;j<MAX_SOCKETS;j++){

                if(shared_mtp_sockets[j].rwnd.nospace==1 && shared_mtp_sockets[j].rwnd.window_size !=0){

                    shared_mtp_sockets[j].rwnd.nospace=0;

                    //send duplicate ack
                    char buf[9];// constructing ACK message
                    sprintf(buf,"ACK%s%d",shared_mtp_sockets[j].rwnd.last_ack_seqno,shared_mtp_sockets[j].rwnd.window_size);
                    
                    printf("%s",buf);

                    struct sockaddr_in dest_addr;
                    memset(&dest_addr, 0, sizeof(dest_addr));
                    dest_addr.sin_family = AF_INET;
                    dest_addr.sin_addr.s_addr = inet_addr(shared_mtp_sockets[j].dest_ip); 
                    dest_addr.sin_port = htons(shared_mtp_sockets[j].dest_port);

                    sendto(shared_mtp_sockets[j].sock_id,buf, strlen(buf), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));// dupliacte ACK sent



                }


            }
        }
    }
}




int main() {

    // sem1 = -1;
    // sem2 = -1;

    // shmid_sock_info = -1;
    // shmid_mtp = -1;
    
    
    printf("server started\n");
    fflush(stdout);
    
    
    // initialize_semaphores();
    // initialize_shared_memory();

    

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

    sem1 = semget(k1, 1, IPC_CREAT | 0666);
    if (sem1 == -1)
    {
        perror("semget");
        exit(EXIT_FAILURE);
    }

    sem2 = semget(k2, 1, IPC_CREAT | 0666);
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
    if(shared_mtp_sockets==(void *)-1){
        perror("shmat key2");
        exit(EXIT_FAILURE);
    }

    // memset(&sock_info,0,sizeof(sock_info));

    memset(shared_mtp_sockets,0,25*sizeof(mtp_socket_t));

    sock_info->sock_id = 0;
    strcpy(sock_info->IP,"");
    sock_info->port=0;
    sock_info->sock_errno=0;
    semctl(sem1, 0, SETVAL, 0);
    semctl(sem2, 0, SETVAL, 0);


    for(int i=0;i<MAX_SOCKETS;i++){
        shared_mtp_sockets[i].is_free = 1; // 1 for true
        shared_mtp_sockets[i].process_id = 0; 
        shared_mtp_sockets[i].sock_id = 0;
        shared_mtp_sockets[i].sendbuf_front = 0;
        shared_mtp_sockets[i].sendbuf_rear = 0;
        shared_mtp_sockets[i].recvbuf_front = 0;
        shared_mtp_sockets[i].recvbuf_rear = 0;
        shared_mtp_sockets[i].seq_no = 0;
        shared_mtp_sockets[i].flag=0;
        memset(shared_mtp_sockets[i].dest_ip, 0, 16*sizeof(char));
        shared_mtp_sockets[i].dest_port = 0;
        // Initialize send_buffer and recv_buffer arrays to NULL
        for (int j = 0; j < 10; j++) {
            memset(shared_mtp_sockets[i].send_buffer[j], 0, MAX_MESSAGE_SIZE);
            
        }
        for (int j = 0; j < 5; j++) {
            memset(shared_mtp_sockets[i].recv_buffer[j], 0, MAX_MESSAGE_SIZE);
            // shared_mtp_sockets[i].rwnd.expected_seqnos[j]=j;
        }
        shared_mtp_sockets[i].swnd.window_size=0;
        shared_mtp_sockets[i].swnd.front=0;
        shared_mtp_sockets[i].swnd.last_ACK_time=0;
        // shared_mtp_sockets[i].swnd.last_send_time=clock()/CLOCKS_PER_SEC;
        shared_mtp_sockets[i].swnd.rear=0;
        shared_mtp_sockets[i].swnd.rcv_ws_des_sock=5;
        shared_mtp_sockets[i].rwnd.window_size=5;
        shared_mtp_sockets[i].rwnd.front=0;
        shared_mtp_sockets[i].rwnd.rear=4;
        shared_mtp_sockets[i].rwnd.last_seqno_recv=-1;
        // shared_mtp_sockets[i].rwnd.nxt_seqno_to_be_taken_out_from_buffer=0;
        shared_mtp_sockets[i].rwnd.nospace=0;
        shared_mtp_sockets[i].rwnd.nxt_expected_seqno=0;
        strcpy(shared_mtp_sockets[i].rwnd.last_ack_seqno,"");
        for (int j = 0; j < 16; j++) {
           
           shared_mtp_sockets[i].swnd.sent_notack[j]=0;
        //    if(j>4){
        //     shared_mtp_sockets[i].rwnd.seq_expected[j]=0;
        //    }
        //    else {shared_mtp_sockets[i].rwnd.seq_expected[j]=1;}
            
        }
    }

    sbw.sem_num = 0;
    sbw.sem_op = -1;
    sbw.sem_flg = 0;

    sbs.sem_num = 0;
    sbs.sem_op = 1;
    sbs.sem_flg = 0;


    // Create R and S threads
    pthread_t thread_R_id, thread_S_id;
    pthread_create(&thread_R_id, NULL, R_thread, NULL);
    pthread_create(&thread_S_id, NULL, S_thread, NULL);

    // Main thread operations
    // TODO: Implement main thread operations as described in the specifications
    while(1){
        //wait for sem1 signal
        semop(sem1,&sbw,1);
        printf("\nServer received message:\n");

        printf("sockid:%d\nsockip:%s\nsockport:%d\n",sock_info->sock_id,sock_info->IP,sock_info->port);
        
        if(sock_info->sock_id == 0 && strcmp(sock_info->IP,"")==0 && sock_info->port == 0 && sock_info->sock_errno == 0){

            printf("socket created");
            
            int sockfd = socket(AF_INET,SOCK_DGRAM,0);
            sock_info->sock_id=sockfd;

            printf("%d\n",sockfd);

        }
        if(sock_info->sock_id != 0 && strcmp(sock_info->IP,"")!=0 && sock_info->port != 0){

            printf("socket binded\n");
            fflush(stdout);

            struct sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            inet_aton(sock_info->IP, &addr.sin_addr); 
            addr.sin_port = htons(sock_info->port); 

            // Bind the socket to the address given by user
            if (bind(sock_info->sock_id, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
                perror("Bind failed");
                sock_info->sock_errno=EADDRINUSE;
                sock_info->sock_id=-1;
            }
        }
        semop(sem2,&sbs,1);
    }

    // Wait for R and S threads to finish
    pthread_join(thread_R_id, NULL);
    pthread_join(thread_S_id, NULL);

    // // Detach and destroy shared memory
    // if (shmdt(sock_info) == -1) {
    //     perror("shmdt");
    //     exit(EXIT_FAILURE);
    // }
    // if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    //     perror("shmctl");
    //     exit(EXIT_FAILURE);
    // }

    // Destroy semaphores
    // if (semctl(sem1, 0, IPC_RMID) == -1 || semctl(sem2, 0, IPC_RMID) == -1) {
    //     perror("semctl");
    //     exit(EXIT_FAILURE);
    // }

    return 0;
}



