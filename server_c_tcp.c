#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#include <ctype.h>  // Required for toupper()
#include <stdbool.h>

#define MAX 80
#define SA struct sockaddr

void error(const char *msg) {
    perror(msg);  
    exit(1);      
}

void func(int connfd) {
    char buff[MAX];
    int sum = 0;
    int count = 0;
    char response[MAX];
    bool status = true;
    int num = 0;

    while (status) {
        bzero(buff, MAX);

        int bytesRead = read(connfd, buff, sizeof(buff) - 1);
        if (bytesRead <= 0) {
            printf("Client disconnected or read error\n");
            status = false;
            break;
        }

        buff[bytesRead] = '\0'; // Null-terminate received string
        printf("From client: %s\n", buff);

        for (int i = 0; i < bytesRead; i++) {
            if (buff[i] == '\0' || buff[i] == '\n') {
                continue;  // Skip null terminator and newline characters
            }
            if (!isdigit(buff[i])) {  // If it's not a digit
                write(connfd, "Sorry, cannot compute!", 23);
                status = false;
                break;
            }
        }

        if (status) {
            sum = 0;
            if (count == 0) {
                for (int i = 0; i < bytesRead; i++) {
                    if (isdigit(buff[i])) {
                        sum += buff[i] - '0';
                    }
                }
            } else {
                while (num > 0) {
                    sum += num % 10;  // Add last digit to sum
                    num /= 10;         // Remove last digit
                }
            }
            num = sum;

            snprintf(response, sizeof(response), "%d", sum);
            write(connfd, response, strlen(response));  // Send final sum to client
            write(connfd, "\n", 1);  // Ensure newline separation
            printf("Sent to client: %s\n", response);

            if (sum < 10) {
                status = false;
            }
            count++;
        }
    }
}

int main(int argc, char *argv[]) {
    int sockfd, connfd;
    socklen_t len; 
    struct sockaddr_in servaddr, cli;
    int port = atoi(argv[1]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     if (sockfd < 0) {
         error("ERROR opening socket");}
        
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(sockfd, (struct sockaddr *) &servaddr,
              sizeof(servaddr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
    len = sizeof(cli);
    

    

    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    } else {
        printf("server accepted the client...\n");
    }

    func(connfd);

    close(sockfd);
}
