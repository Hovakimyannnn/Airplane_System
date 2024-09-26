#include "../includes/include.h"

void* write_thr(void* arg) {
    char buffer[BUFFER_SIZE];
    Arg ob = *(Arg*)arg;

    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        send(ob.sockfd, buffer, strlen(buffer), 0);
    }

    return NULL;
}


void* read_thr(void* arg) {
    char buffer[BUFFER_SIZE];
    Arg ob = *(Arg*)arg;

    while (1) {
        int bytes_received = recv(ob.sockfd, buffer, BUFFER_SIZE, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';  
            puts(buffer);
            if (atoi(buffer) == 5) {
                printf("Exit condition met. Closing connection.\n");
                close(ob.sockfd);
                exit(0);
            }
        } else if (bytes_received == 0) {
            printf("Server disconnected.\n");
            close(sockfd);
            exit(1);
            break;  
        } else {
            perror("recv failed");
            break;
        }
    }

    return NULL;
}

int main() {
    pthread_t write_thread, read_thread;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    socklen_t size = sizeof(serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


    if (connect(sockfd, (struct sockaddr*)&serv_addr, size) < 0) {
        perror("connect failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    Arg ob;
    ob.server = serv_addr;
    ob.sockfd = sockfd;


    pthread_create(&write_thread, NULL, write_thr, &ob);
    pthread_detach(write_thread);  

    pthread_create(&read_thread, NULL, read_thr, &ob);
    pthread_detach(read_thread);   

    while(1) {
        sleep(1);
    }
    return 0;
}


