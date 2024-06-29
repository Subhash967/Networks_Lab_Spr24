#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_BUFFER_SIZE 1024
#define POP3_PORT 11000

typedef struct  {
    // int index;
    char rectime[MAX_BUFFER_SIZE];
    char from[MAX_BUFFER_SIZE];
    char to[MAX_BUFFER_SIZE];
    char subject[MAX_BUFFER_SIZE];
    char data[MAX_BUFFER_SIZE];
    char wholemsg[MAX_BUFFER_SIZE];
    size_t msgsize;
    int assignfordeletion;
    
} message;

void Deletemessage(int k,char path[]){

    // printf("deleltion");
    // fflush(stdout);

    FILE *inputFile, *tempFile;
    char buffer[1000];
    memset(buffer,0,sizeof(buffer));
    const char *file_path = path;
    inputFile = fopen(file_path, "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return ;
    }

    // Create a temporary file for writing
    tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        perror("Error opening temporary file");
        fclose(inputFile);
        return ;
    }
                    
    int count = 1;
    while (fgets(buffer, sizeof(buffer), inputFile) != NULL) {
                        
        // printf("count%d,k%d\n",count,k);   
        if(count!=k){
            fprintf(tempFile, "%s", buffer);
        }

        if (strcmp(buffer, ".\n") == 0) {
            count++;
        }
        memset(buffer,0,sizeof(buffer));
    }

    // Close the input file
    fclose(inputFile);

    // Close the temporary file
    fclose(tempFile);

    // Open the input file in write mode
    inputFile = fopen(file_path, "w");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return ;
    }

    // Open the temporary file for reading
    tempFile = fopen("temp.txt", "r");
    if (tempFile == NULL) {
        perror("Error opening temporary file");
        fclose(inputFile);
        return ;
    }

    // Copy the contents of the temporary file back to the input file
    while (fgets(buffer, sizeof(buffer), tempFile) != NULL) {
        fprintf(inputFile, "%s", buffer);
    }

    // Close the files
    fclose(inputFile);
    fclose(tempFile);

    // Remove the temporary file
    // if (remove("temp.txt") != 0) {
    //     perror("Error deleting temporary file");
    //     return ;
    


}

int main(int argc,char*argv[]) {
    int server_socket, client_socket;
    int pop3_port= atoi(argv[1]);
    struct sockaddr_in server_address, client_address;
    socklen_t server_sockLen = sizeof(server_address);
    socklen_t client_address_len = sizeof(client_address);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(pop3_port);

    // Bind socket
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("POP3 server listening on port %d...\n", pop3_port);
    char username[100];

    while (1) {
        // Accept connection
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        // printf("Connection accepted from %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        char buffer[MAX_BUFFER_SIZE];
        char msg[MAX_BUFFER_SIZE];

        int bytes_received;

        // Send POP3 greeting
        snprintf(buffer, MAX_BUFFER_SIZE, "+OK POP3 server ready\r\n");
        send(client_socket, buffer, strlen(buffer), 0);

        memset(buffer,0,sizeof(buffer));
        recv(client_socket,buffer,sizeof(buffer),0);
        // printf("%s\n",buffer);


        char user[]="user.txt";
        FILE *file1 = fopen(user,"r");
        if (file1 == NULL) {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }

        char line[MAX_BUFFER_SIZE];
        int flag=0;
        while (fgets(line, sizeof(line), file1) != NULL) {
            if(strcmp(buffer,line)==0){
                flag=1;
            }
        }

        fclose(file1);

        // printf("xxxx");
        // fflush(stdout);

        if(flag==1){
            char *mesg="+OK\r\n";
            char *quit="QUIT\r\n";
            char *stat="STAT\r\n";
            char *list="LIST\r\n";
            char *retr="RETR\r\n";
            char *rset="RSET\r\n";
            char *dot=".\r\n";

            send(client_socket,mesg,sizeof(mesg),0);
            sscanf(buffer, "%s", username);

            char path[100];
            strcpy(path,username);
            strcat(path, "/mymailbox");
            // printf("%s",path);
            // fflush(stdout);
            const char *file_path = path;
            FILE *file = fopen(file_path, "r");
            if (file == NULL) {
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }
            char line[MAX_BUFFER_SIZE];
            int count = 0;
            while (fgets(line, sizeof(line), file) != NULL) {
                line[strcspn(line, "\n")] = '\0';
                if (strcmp(line, ".") == 0) {
                    count++;
                }
            }
            // printf("%d\n",count);
            // fflush(stdout);
            message Msg[count+1];
            for(int i=1;i<=count;i++){
                Msg[i].data[0]='\0';
                Msg[i].wholemsg[0]='\0';
                Msg[i].assignfordeletion=0;
                Msg[i].msgsize=0;
            }
            int c=1;
            FILE *file1 = fopen(file_path, "r");
            if (file1 == NULL) {
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }
            char line1[MAX_BUFFER_SIZE];
            size_t totalsize = 0;
            while (fgets(line1, sizeof(line1), file1) != NULL) {
                    // Remove newline character from the line

                    //  printf("%s",line1);

                    // printf("%s",line1);
                    // fflush(stdout);

                    strcat(Msg[c].wholemsg,line1);

                    line1[strcspn(line1, "\n")] = '\0';
                    
                    totalsize += (strlen(line1))*sizeof(char);
                    Msg[c].msgsize += (strlen(line1))*sizeof(char);

                    

                    // if (strncmp(line1, "Recieved time:", 14) == 0) {
                        
                    //     sscanf(line1, "Recieved time: %49[^\n]", Msg[c].rectime);
                    //     printf("%s",Msg[c].rectime);
                    // }

                    // // Check for "From:" and extract email address
                    // else if (strncmp(line1, "From:", 5) == 0) {
                        
                    //     if (sscanf(line1, "From: %49[^\n]", Msg[c].from) != 1) {
                    //         printf("Error");
                    //     }
                    // }

                    // else if (strncmp(line1, "To:", 3) == 0) {
                        
                    //     if (sscanf(line1, "From: %49[^\n]", Msg[c].to) != 1) {
                    //         printf("Error");
                    //     }
                    // }

                    // else if (strncmp(line1, "Subject:", 8) == 0) {
                        
                    //     if (sscanf(line1, "From: %49[^\n]", Msg[c].subject) != 1) {
                    //         printf("Error");
                    //     }
                    // }

                     if (strncmp(line1, ".", 1) == 0) {
                        
                        c++;
                    }

                    // else{
                    //     strcat(Msg[c].data,line1);
                    // }

            }

            // for(int i=1;i<=count;i++){
            //     printf("%s",Msg[i].wholemsg);
            //     fflush(stdout);
            // }

            // printf("%lu\n",totalsize);
            // fflush(stdout);

            memset(buffer,0,sizeof(buffer));
            recv(client_socket,buffer,sizeof(buffer),0);
            if(strcmp(buffer,stat)==0){
                sprintf(buffer,"+OK %d %lu",count,totalsize);
            }
            send(client_socket,buffer,sizeof(buffer),0);


            // memset(buffer,0,sizeof(buffer));
            // recv(client_socket,buffer,sizeof(buffer),0);
            // if(strcmp(buffer,list)==0){
            //     sprintf(buffer,"+OK %d messages\r\n",count);
            //     send(client_socket,buffer,sizeof(buffer),0);
            //     printf("sent %s",buffer);
            //     for(int i=1;i<=count;i++){
            //         sprintf(buffer,"%d %lu\r\n",i,Msg[i].msgsize);
            //         send(client_socket,buffer,sizeof(buffer),0);
            //         printf("sent %s",buffer);
            //     }
            //     send(client_socket,dot,sizeof(dot),0);
            // }

            char buf[10];
            
            for(int i=1;i<=count;i++){
                
                memset(buffer,0,sizeof(buffer));
                recv(client_socket,buffer,sizeof(buffer),0);
                // printf("%s\n",buffer);
                // fflush(stdout);
                sprintf(buf,"RETR %d",i);
                if(strcmp(buffer,buf)==0){
                    send(client_socket,Msg[i].wholemsg,sizeof(Msg[i].wholemsg),0);
                    // printf("sent %d\n%s",i,Msg[i].wholemsg);
                }
            }
            while(1){
                memset(buffer,0,sizeof(buffer));
                recv(client_socket,buffer,sizeof(buffer),0);
                // sprintf(buf,"DELE %d",i);  
                // printf("%s",buffer);
                // fflush(stdout);
                if(strncmp(buffer,"DELE",4)==0){
                    // char *tok=strtok(buffer," ");
                    // printf("%s",tok);
                    // fflush(stdout);
                    // tok=strtok(NULL,"\r");
                    // printf("%c",buffer[5]);
                    // fflush(stdout);
                    int k=atoi(&buffer[5]);
                    if(Msg[k].assignfordeletion==1){
                        char *mesg="-ERR Message already assigned for deletion";
                        send(client_socket,mesg,sizeof(mesg),0);
                    }
                    else{
                        Msg[k].assignfordeletion=1;
                        char *mesg="+OK Message assigned for deletion";
                            send(client_socket,mesg,sizeof(mesg),0);
                    }
                    // printf("update %d",Msg[k].assignfordeletion);
                    // fflush(stdout);
                }   
                if(strncmp(buffer,"QUIT",4)==0){
                    printf("goodbye\n");
                    fflush(stdout);
                    for(int i=1;i<=count;i++){
                        if(Msg[i].assignfordeletion==1){

                            Deletemessage(i,path);
                        }
                    }
                    break;
                }   
                // printf("x");    
            }
        }
        else{
            char *mesg="-ERR";
            send(client_socket,mesg,sizeof(mesg),0);
        }




        // Close the client socket
        close(client_socket);
    }


    // Close the server socket
    close(server_socket);

    return 0;
}

