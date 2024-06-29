#include "msocket.h"

int main(int argc,char *argv[]){

    int M2 = m_socket(AF_INET,SOCK_MTP,0);

    int local_port = atoi(argv[1]);

    char* local_IP="127.0.0.1";
    char* remote_IP="127.0.0.1";

    int remote_port = atoi(argv[2]);
    
    printf("%d",local_port);
    int x;

    // Bind the socket to the address given by user
    if (x=m_bind(M2, local_IP,local_port,remote_IP,remote_port) < 0) {
        perror("Bind failed");
    }

    printf("%d\n",x);

    FILE *file;
    char buffer[1021]; // 1020 characters plus one for null terminator
    memset(buffer,0,sizeof(buffer));

    file = fopen("transfered_file.txt", "w");

    if (file == NULL) {
        printf("Error opening the file.\n");
        exit(1);
    }

    
    // // Close the file to overwrite previous data
    // fclose(file);

    // // Open the file in append mode
    // file = fopen("my_file.txt", "a");

    if (file == NULL) {
        printf("Error opening the file.\n");
        exit(1);
    }

    while(1){

        // printf("Entered\n");

        memset(buffer,0,sizeof(buffer));
        int r=m_recvfrom(M2,buffer,sizeof(buffer),0,remote_IP,remote_port);

        

        if(strcmp(buffer,"END")==0){
            break;
        }

        // printf("r=%d\n",r);

        if(r==-1){
            sleep(2);
        }else{

        // buffer[r] = '\0';

        // printf("%s\n",buffer);

        fprintf(file,"%s",buffer);
        }

    }
    

    // Close the file
    fclose(file);

}
