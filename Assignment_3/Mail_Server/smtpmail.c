#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include<time.h>
#include <fcntl.h>

#define MAX_BUFFER_SIZE 1024

int main(int argc,char*argv[]) {
    int smtp_socket,new_socket;
    int smtp_port= atoi(argv[1]);
    struct sockaddr_in serverAddr,clientaddress;
    socklen_t smtp_sockLen = sizeof(serverAddr);
    socklen_t client_socklen = sizeof(clientaddress);



    smtp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (smtp_socket < 0) {
        perror("Error in socket creation");
        exit(EXIT_FAILURE);
    }
    printf("socket created\n");


    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(smtp_port); 

    // Bind the socket
    if (bind(smtp_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error in binding");
        exit(EXIT_FAILURE);
    }
    printf("socket binded\n");

    // Listen for incoming connections
    if (listen(smtp_socket, 5) < 0) {
        perror("Error in listening");
        exit(EXIT_FAILURE);
    }
    printf("socket listened\n");


            while (1) {
                 // Accept a connection
                new_socket = accept(smtp_socket,(struct sockaddr *)&clientaddress, &client_socklen);
                if (new_socket < 0) {
                    perror("Error in accepting connection");
                    exit(EXIT_FAILURE);
                }
                printf("connection successfully\n");
    
                pid_t pid= fork();
                if( pid <0){
                    printf("Fork failed\n");
                }else if(pid == 0){ 
                    // ******* child *********
                close(smtp_socket);
                

                //send a response to client "220 server ready" message
                char *msg = "220 <iitkgp.edu Service ready\n";
                send(new_socket, msg, strlen(msg), 0);

                //recieve  a response from client "Helo <iitkgp.edu> " message
                char buffer[MAX_BUFFER_SIZE];
                memset(buffer,0,sizeof(buffer));
                recv(new_socket,buffer,sizeof(buffer),0);
                printf("%s\n",buffer); // printing helo
                

                msg = "250 OK Hello iitkgp.edu";
                send(new_socket, msg, strlen(msg), 0);

                memset(buffer,0,sizeof(buffer));
                recv(new_socket,buffer,sizeof(buffer),0);
                printf("from mail shall come before\n");

                printf("%s\n",buffer);//  mail from<ag>
                printf("from mail shall come after\n");

                msg = " 250 <ag@iitkgp.edu>... Sender ok";
                send(new_socket, msg, strlen(msg), 0);
                
                memset(buffer,0,sizeof(buffer));
                recv(new_socket,buffer,sizeof(buffer),0);
                printf("rcpt mail shall come before\n");
                printf("%s\n",buffer);// RCPT to<gb>
                printf("rcpt mail shall come after\n");

                
    // Find the position of '<' and '@'
    const char *startPos = strchr(buffer, '<');
    const char *endPos = strchr(buffer, '@');
    size_t emailLength = endPos - startPos - 1;
    char emailAddress[emailLength + 1];

    if (startPos != NULL && endPos != NULL && startPos < endPos) {
        // Extract the email address
        strncpy(emailAddress, startPos + 1, emailLength);
        emailAddress[emailLength] = '\0';

        printf("Extracted Email Address: %s\n", emailAddress);
    } else {
        printf("Invalid RCPT TO command format\n");
    }


                msg = " 250 root... Recipient ok";
                send(new_socket, msg, strlen(msg), 0);

                memset(buffer,0,sizeof(buffer));
                recv(new_socket,buffer,sizeof(buffer),0);
                printf("%s\n",buffer);// DATA

                msg = "354 Enter mail, end with \".\" on a line by itself";
                send(new_socket, msg, strlen(msg), 0);
                char big_buffer[4096];
                memset(big_buffer,0,sizeof(big_buffer));

                time_t currentTime;
                time(&currentTime);

                // Convert the current time to the local time
                struct tm *localTime = localtime(&currentTime);

                // Format the time as a string
                char timeString[100];  // Adjust the size as needed
                strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localTime);

                // Print the formatted time
                printf("Current Date and Time: %s\n", timeString);
                char recieve_time[100];
                sprintf(recieve_time,"Recieved time: %s\n",timeString);
                strcat(big_buffer,recieve_time);

                while(strncmp(buffer,".",1)!=0){
                    recv(new_socket,buffer,sizeof(buffer),0);
                    printf("%s\n",buffer);
                    strcat(big_buffer,buffer);

                }
                recv(new_socket,buffer,sizeof(buffer),0);
                //strcat(big_buffer,buffer);
                printf("%s\n",big_buffer);


                FILE* mailbox;
                char path[100];
                sprintf(path,"%s/mymailbox",emailAddress);
                mailbox = fopen(path,"a");
                fputs(big_buffer,mailbox);
                fclose(mailbox);
                
                msg ="250 OK Message accepted for delivery";
                send(new_socket, msg, strlen(msg), 0); // 250 0k accepted for delivery

                memset(buffer,0,sizeof(buffer));
                recv(new_socket,buffer,sizeof(buffer),0);
                printf("%s\n",buffer);// QUIT

                msg = "221 iitkgp.edu closing connection";
                send(new_socket, msg, strlen(msg), 0); // 221 closing connection

                memset(buffer,0,sizeof(buffer));
                recv(new_socket,buffer,sizeof(buffer),0);
                printf("%s\n",buffer);// client hangs up




                exit(0);
            }else{
            
                // ******** parent **********
                // Close the connection with the client
                close(new_socket);
                }
                // parent also has this newly created socket, since

            }

    // Close the server socket
    close(smtp_socket);

    return 0;
}

