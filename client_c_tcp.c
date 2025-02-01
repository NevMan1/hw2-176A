#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()

#define MAX 80
#define SA struct sockaddr

// Function designed for communication between client and server
void func(int sockfd)
{
    char buff[MAX];

    // Clear buffer before sending the message
    bzero(buff, sizeof(buff));

    printf("Enter string: ");
    fgets(buff, sizeof(buff), stdin);  // Take input from user

    // Send message to server
    write(sockfd, buff, strlen(buff)); // Send only the actual message length

    // Clear buffer before receiving the response
    bzero(buff, sizeof(buff));

    // Read response from server
    read(sockfd, buff, sizeof(buff));

    // Print the server response
    printf("From Server: %s", buff);
}

int main(int argc, char *argv[])
{
    int sockfd; // Declare only sockfd since that's the only socket needed
    struct sockaddr_in servaddr;

    // Check for valid input arguments
    if (argc < 3) {
        printf("Usage: %s <server-ip> <port>\n", argv[0]);
        exit(0);
    }

    // Create socket and check for errors
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(0);
    } 

    bzero(&servaddr, sizeof(servaddr));

    // Parse server IP and port number from command line arguments
    char *server_ip = argv[1];
    int port = atoi(argv[2]);

    // Assign IP and PORT to the server address
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);

    // Convert the server IP from string to binary form
    if (inet_pton(AF_INET, server_ip, &servaddr.sin_addr) <= 0) {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("Connection with the server failed...\n");
        exit(0);
    } 

    // Function to send and receive messages
    func(sockfd);

    // Close the socket
    close(sockfd);

    return 0;
}
