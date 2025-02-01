// I got inspiration from https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
// Implemented a lot of the  principals that they used

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <ctype.h>

void handle_error(const char *message)
{
    perror(message);
    exit(1);
}

int main(int argc, char *argv[])
{
    // Define socket variables
    int server_fd, client_fd, port_number;
    socklen_t client_len;
    char data_buffer[256];
    struct sockaddr_in server_addr, client_addr;
    int read_bytes;

    if (argc < 2) {
        fprintf(stderr, "ERROR, port number missing\n");
        exit(1);
    }

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
        handle_error("ERROR creating socket");

    bzero((char *) &server_addr, sizeof(server_addr));
    port_number = atoi(argv[1]);

    // This Setups the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // This is what will listern 
    server_addr.sin_port = htons(port_number);

    // Initializes and binds the socket
    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        handle_error("ERROR binding socket");

    // Listen for incoming connections from the clinet
    listen(server_fd, 5);
    client_len = sizeof(client_addr);

    while (1) {
        // THe logic begins once it accepts client connection
        client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_len);
        if (client_fd < 0)
            handle_error("ERROR accepting connection");

        bzero(data_buffer, 256); // Clear the buffer 

        // Read what message comes in from the user.
        read_bytes = read(client_fd, data_buffer, 255);
        if (read_bytes < 0) {
            handle_error("ERROR reading from socket");
        }
        
        printf("Received message: %s\n", data_buffer);

        int digit_sum = 0;
        bool valid_input = true;

        // Calculate sum of digits and check if input contains only digits
        for (int i = 0; data_buffer[i] != '\0' && data_buffer[i] != '\n'; i++) {
            const char current_char = data_buffer[i];
            if (isdigit(current_char)) {
                digit_sum += current_char - '0'; // Add digit to sum
            } else {
                valid_input = false;
                break;
            }
        }

        char result_buffer[256] = ""; // Initialize result buffer

        // If valid input, process sum and respond
        if (valid_input) {
            snprintf(data_buffer, sizeof(data_buffer), "%d", digit_sum); // Convert sum to string
            strcat(result_buffer, data_buffer); // Add to result
            strcat(result_buffer, "!"); // Append 'a'

            // Continue summing digits until single digit result is obtained
            while (digit_sum >= 10) {
                digit_sum = 0;
                for (int i = 0; data_buffer[i] != '\0'; i++) {
                    const char current_char = data_buffer[i];
                    if (isdigit(current_char)) {
                        digit_sum += current_char - '0'; // Add digit to sum
                    }
                }

                // Clear buffer and convert sum to string
                bzero(data_buffer, 256);
                snprintf(data_buffer, sizeof(data_buffer), "%d", digit_sum);
                strcat(result_buffer, data_buffer); // Append sum to result
                strcat(result_buffer, "!"); // Append the seperatros.
            }

            // Send the result back to the client
            read_bytes = write(client_fd, result_buffer, strlen(result_buffer));
            if (read_bytes < 0) {
                handle_error("ERROR writing to socket");
            }
            bzero(result_buffer, 256); // Clear the result buffer
        } else {
            // If not valid input, send error message
            read_bytes = write(client_fd, "Sorry, cannot compute\n", 23);
            if (read_bytes < 0) {
                handle_error("ERROR writing to socket");
            }
        }

        // Close client connection
        close(client_fd);
    }

    return 0;
}
