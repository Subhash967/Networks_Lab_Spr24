#include "msocket.h"

int main(int argc,char *argv[]){

    int M1 = m_socket(AF_INET,SOCK_MTP,0);

    int local_port = atoi(argv[1]);

    char* local_IP="127.0.0.1";
    char* remote_IP="127.0.0.1";

    int remote_port = atoi(argv[2]);
    

    // Bind the socket to the address given by user
    if (m_bind(M1, local_IP,local_port,remote_IP,remote_port) < 0) {
        perror("Bind failed");
    }

    FILE *file_ptr;
    char buffer[1021]; // 1020 characters plus one for null terminator

    // Open the file for reading
    file_ptr = fopen("original_file.txt", "r");
    if (file_ptr == NULL) {
        perror("Error opening file");
        return -1;
    }

    // Read 1020 characters step by step in a for loop
    int x=0;
    size_t read_chars;
    while ((read_chars = fread(buffer, 1, 50, file_ptr)) > 0) {
        // Null-terminate the buffer
        buffer[read_chars] = '\0';

        do{

        sleep(4);

        x= m_sendto(M1,buffer,strlen(buffer),0,remote_IP,remote_port); 

        }while(x==-1);


        // printf("%d\n",x);

        // if(x==-1){
        //     perror("error");//wait for the buffer to be empty
        // }




    }
    memset(buffer,0,sizeof(buffer));
    strcpy(buffer,"END");
    do{

        sleep(4);

        x= m_sendto(M1,buffer,strlen(buffer),0,remote_IP,remote_port); 

        }while(x==-1);


    // Close the file
    fclose(file_ptr);

}
