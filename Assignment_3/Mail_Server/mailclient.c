#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include<time.h>
#include <fcntl.h>

#define MAX_BUFFER_SIZE 1024
#define POP3_PORT 11000

char username[50];
char password[50];

typedef struct  {
    // int index;
    char rectime[MAX_BUFFER_SIZE];
    char from[MAX_BUFFER_SIZE];
    char to[MAX_BUFFER_SIZE];
    char subject[MAX_BUFFER_SIZE];
    char data[MAX_BUFFER_SIZE];
    char wholemsg[MAX_BUFFER_SIZE];
    
} message;

int isValidEmail(const char *email) {
    int length = strlen(email);

    // Check if '@' is present
    const char *atSymbol = strchr(email, '@');
    if (atSymbol == NULL) {
        return 0;  // '@' not present
    }

    // Check if '@' is not at the beginning or end
    if (atSymbol == email || atSymbol == email + length - 1) {
        return 0;  // '@' at the beginning or end
    }

    return 1;  // Valid email
}


int main(int argc, char *argv[]) {

    int choice;
    int client_socket;
    int round=0;
    char *q="QUIT\r\n";
    int smtp_socket,smtp_port,pop3_port;
    smtp_port = atoi(argv[2]);
    pop3_port = atoi(argv[3]);

    while(1){

        printf("Enter username: ");
        scanf("%s",username);
        printf("Enter password: ");
        scanf("%s",password);
        printf("\n");
        while(1){
            printf("1.Manage mails\n");
            printf("2.Send mail\n");
            printf("3.Quit\n");
            printf("Enter your choice:");
            scanf("%d", &choice);
            if (choice==3){
                if(round!=0){
                send(client_socket,q,sizeof(q),0);
                }
                exit(0);
            }
            if (choice == 2 ){
                struct sockaddr_in serverAddr;

                // Create socket
                smtp_socket = socket(AF_INET, SOCK_STREAM, 0);
                if (smtp_socket < 0) {
                    perror("Error in socket creation");
                    exit(EXIT_FAILURE);
                }

                // Set up server address struct
                memset(&serverAddr, 0, sizeof(serverAddr));
                serverAddr.sin_family = AF_INET;
                serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
                serverAddr.sin_port = htons(smtp_port); 

                // Connect to the server
                if (connect(smtp_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
                    perror("Error in connecting to server");
                    exit(EXIT_FAILURE);
                }
                char msg[MAX_BUFFER_SIZE];
                char buffer[MAX_BUFFER_SIZE];
                int msglen;
                //recieve a response from server and printing it

                char from_mail[100];
                printf("MAIL FROM:");
                scanf(" %s", from_mail);
                getchar();
                printf("from mail = %s\n",from_mail);

                if (!isValidEmail(from_mail)) continue;


                char to_mail[100];
                printf("RCPT TO:");
                scanf(" %s", to_mail);
                getchar();
                printf("rcpt mail = %s\n",to_mail);

                if (!isValidEmail(to_mail)) continue;


                
                memset(buffer,0,sizeof(buffer));
                recv(smtp_socket,buffer,sizeof(buffer),0);
                printf("%s\n",buffer);// 220 printing
                // sending "HELO <iitkgp.edu>" to server

                    strcpy(msg,"HELO <iitkgp.edu>\n");
                    msglen=strlen(msg);
                    send(smtp_socket,msg,msglen,0);

                //recieve a response from server and printing it
                memset(buffer,0,sizeof(buffer));
                recv(smtp_socket,buffer,sizeof(buffer),0);
                printf("%s\n",buffer); // 250 printing 

                    memset(msg,0,sizeof(msg));
                    sprintf(msg, "MAIL FROM : <%s>", from_mail);
                    // msg = "MAIL FROM: <ag@iitkgp.edu>";
                    msglen=sizeof(msg);
                    printf("sending msg from_mail = %s/n",msg);
                    send(smtp_socket,msg,msglen,0);

                    memset(buffer,0,sizeof(buffer));
                    recv(smtp_socket,buffer,sizeof(buffer),0);
                    printf("%s\n",buffer);// 250 sender ok 

                    
                    sprintf(msg,"RCPT TO <%s>",to_mail);
                    printf("sending msg rcpt = %s/n",msg);
                    msglen=sizeof(msg);
                    send(smtp_socket,msg,msglen,0);

                    memset(buffer,0,sizeof(buffer));
                    recv(smtp_socket,buffer,sizeof(buffer),0);
                    printf("%s\n",buffer);// 250 recepient ok

                    strcpy(msg,"DATA");
                    msglen=sizeof(msg);
                    send(smtp_socket,msg,msglen,0);

                    memset(buffer,0,sizeof(buffer));
                    recv(smtp_socket,buffer,sizeof(buffer),0);
                    printf("%s\n",buffer);// 354 enter mail


                    
                    while(strncmp(buffer,".",1)!=0){
                        memset(buffer,0,sizeof(buffer));
                        fgets(buffer,sizeof(buffer),stdin);
                        // if(strcmp(buffer,".\n")==0){
                        //     break;
                        // }
                        //printf("%s",buffer);
                        send(smtp_socket,buffer,sizeof(buffer),0);
                    }
                    // fflush(stdin);

                    send(smtp_socket,buffer,sizeof(buffer),0);
                    printf("while loop done\n");

                    memset(buffer,0,sizeof(buffer));
                    recv(smtp_socket,buffer,sizeof(buffer),0);
                    printf("%s\n",buffer);// 250 ok for delivery

                    strcpy(msg,"QUIT");
                    msglen=strlen(msg);
                    send(smtp_socket,msg,msglen,0);

                    memset(buffer,0,sizeof(buffer));
                    recv(smtp_socket,buffer,sizeof(buffer),0);
                    printf("%s\n",buffer);// 221 closing connection

                    strcpy(msg,"<client hangs up>");
                    msglen=strlen(msg);
                    send(smtp_socket,msg,msglen,0);

                    // Close the client socket
                    close(smtp_socket);
                }
            
            if(choice==1){

                
                struct sockaddr_in server_address;

                // Create socket
                client_socket = socket(AF_INET, SOCK_STREAM, 0);
                if (client_socket == -1) {
                    perror("Error creating socket");
                    exit(EXIT_FAILURE);
                }

                // Set up server address
                memset(&server_address, 0, sizeof(server_address));
                server_address.sin_family = AF_INET;
                server_address.sin_addr.s_addr = inet_addr("127.0.0.1");  // Use the actual IP address of your POP3 server
                server_address.sin_port = htons(pop3_port);

                // Connect to the server
                if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
                    perror("Error connecting to server");
                    exit(EXIT_FAILURE);
                }

                char buffer[MAX_BUFFER_SIZE];
                char buf[500];
                char *ok="+OK";
                char *err="-ERR";
                char *space=" ";

                memset(buffer,0,sizeof(buffer));
                recv(client_socket,buffer,sizeof(buffer),0);
                printf("%s\n",buffer);


                memset(buffer,0,sizeof(buffer));
                strcpy(buffer,username);
                strcat(buffer,space);
                strcat(buffer,password);
                send(client_socket,buffer,sizeof(buffer),0);

                memset(buffer,0,sizeof(buffer));
                recv(client_socket,buffer,sizeof(buffer),0);
                printf("%s\n",buffer);

                if(strcmp(buffer,err)==0){
                    close(client_socket);
                }

                    
                char *mesg="+OK\r\n";
                char *quit="QUIT\r\n";
                char *stat="STAT\r\n";
                char *list="LIST\r\n";
                char *retr="RETR\r\n";
                char *rset="RSET\r\n";
                char *end="\r\n.\r\n";
                char *ende="\n.\n";

                printf("%s\n",stat);
                send(client_socket,stat,sizeof(stat),0);

                memset(buffer,0,sizeof(buffer));
                recv(client_socket,buffer,sizeof(buffer),0);
                printf("%s\n",buffer);
                int n= atoi(&buffer[4]);

                // printf("%d",n);
                // fflush(stdout);

                // printf("%s\n",list);
                // send(client_socket,list,sizeof(list),0);


                // memset(buffer,0,sizeof(buffer));
                // buffer[0]='\0';
                // while(strcmp(buffer + strlen(buffer) - 5,end)!=0){
                //     memset(buf,0,sizeof(buf));
                //     recv(client_socket,buf,sizeof(buf),0);
                //     printf("%s\n",buf);
                //     strcat(buffer,buf);
                // }
                // printf("list=\n%s",buffer);


                message Msg[n+1];
                for(int i=1;i<=n;i++){
                    Msg[i].data[0]='\0';
                }

                for(int i=1;i<=n;i++){
                    memset(buffer,0,sizeof(buffer));
                    sprintf(buffer,"RETR %d",i);

                    printf("%s\n",buffer);
                    fflush(stdout);
                    send(client_socket,buffer,sizeof(buffer),0);

                    memset(buffer,0,sizeof(buffer));
                    // buffer[0]='\0';
                    int length;
                    char lastThree[4];
                    memset(buf,0,sizeof(buf));
                    memset(lastThree,0,sizeof(lastThree));
                    while(strcmp(lastThree,ende)!=0){
                        
                        recv(client_socket,buf,sizeof(buf),0);
                        printf("%s\n",buf);
                        fflush(stdout);
                        strcat(buffer,buf);
                        length = strlen(buffer);
                        strncpy(lastThree, buffer + length - 3, 3);
                        lastThree[3] = '\0';
                        memset(buf,0,sizeof(buf));
                    }
                    
                    strcpy(Msg[i].wholemsg,buffer);

                }

                // for(int i=1;i<=n;i++){
                //     printf("%s",Msg[i].wholemsg);
                //     fflush(stdout);
                // }



                for(int i=1;i<=n;i++){
                    
                    char * temp=strdup(Msg[i].wholemsg);
                    char *line = strtok(temp, "\n");

                    // Iterate through each line
                    while (line != NULL) {
                        // Process the current line (e.g., print it)
                        // printf("%s\n", line);

                        if (strncmp(line, "Recieved time:", 14) == 0) {
                            
                            sscanf(line, "Recieved time: %49[^\n]", Msg[i].rectime);
                            // printf("%s\n",Msg[i].rectime);
                        }

                        // Check for "From:" and extract email address
                        else if (strncmp(line, "From:", 5) == 0) {
                            
                            sscanf(line, "From: %49[^\n]", Msg[i].from) ;
                            // printf("%s\n",Msg[i].from);
                            
                        }

                        else if (strncmp(line, "To:", 3) == 0) {
                            
                            sscanf(line, "To: %49[^\n]", Msg[i].to) ;
                            // printf("%s\n",Msg[i].to);
                        }

                        else if (strncmp(line, "Subject:", 8) == 0) {
                            
                            sscanf(line, "Subject: %49[^\n]", Msg[i].subject) ;
                            // printf("%s\n",Msg[i].subject);
                        }

                    
                        else{
                            strcat(Msg[i].data,line);
                        }

                        // Move to the next line
                        line = strtok(NULL, "\n");
                    }

                    

                }

                while(1){
                    for(int i=1;i<=n;i++){
                        printf("%d. %s %s %s\n",i,Msg[i].from,Msg[i].rectime,Msg[i].subject);
                    }
                    
                    int m;
                    printf("Enter mail no. to see:");
                    scanf("%d",&m);

                    while(m>n){
                        printf("Mail no. out of range, give again");
                        scanf("%d",&m);
                    }

                    if(m==-1){
                        round++;
                        break;
                    }
                    else{
                        printf("%s",Msg[m].wholemsg);

                        getchar();
                        char g=getchar();
                        if(g=='d'){
                            memset(buffer,0,sizeof(buffer));
                            sprintf(buffer,"DELE %d\r\n",m);
                            send(client_socket,buffer,sizeof(buffer),0);
                            memset(buf,0,sizeof(buf));
                            recv(client_socket,buf,sizeof(buf),0);
                            printf("%s\n",buf);
                        }

                    }
                }
            }

            

        }
    }
}
