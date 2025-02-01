#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void report_error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int client_socket, server_port, bytes_sent;
    struct sockaddr_in server_addr;
    struct hostent *remote_server;

    // open the socket and establish connection
    char input_buffer[256];
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    
    server_port = atoi(argv[2]);
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
        report_error("ERROR opening socket");
    
    remote_server = gethostbyname(argv[1]);
    if (remote_server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)remote_server->h_addr, 
         (char *)&server_addr.sin_addr.s_addr,
         remote_server->h_length);
    server_addr.sin_port = htons(server_port);

    // Try to connect to the server
    if (connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        report_error("ERROR connecting");

    // Get the input from the user
    printf("Enter string: ");
    bzero(input_buffer, 256);
    fgets(input_buffer, 255, stdin);

    // Send user message to the server
    bytes_sent = write(client_socket, input_buffer, strlen(input_buffer));
    if (bytes_sent < 0)
        report_error("ERROR writing to socket");

    bzero(input_buffer, 256);

    // Read server response 
    bytes_sent = read(client_socket, input_buffer, 255);
    if (bytes_sent < 0)
        report_error("ERROR reading from socket");

    // See if the  server ran into any errors
    if (strcmp(input_buffer, "Sorry, cannot compute!") == 0) {
        printf("From server: Sorry, cannot compute!\n");
        close(client_socket);
        return 0;
    }

    // The server responds with numbers separated by '!' This will work as a seperator
    int substring_start = -1;
    for (int i = 0; input_buffer[i] != '\0'; i++) {
        if (input_buffer[i] == '!') {
            if (substring_start != -1) {
                printf("From server: ");
                for (int j = substring_start; j < i; j++) {
                    printf("%c", input_buffer[j]);
                }
                printf("\n");
            }
            substring_start = -1;
        } else {
            if (substring_start == -1) {
                substring_start = i;
            }
        }
    }

    // Close the  socket
    close(client_socket);
    return 0;
}
