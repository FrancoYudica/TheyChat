#include "net/file_transfer.h"
#include "net/net_communication.h"

ErrorCode send_file(const char* filepath, uint32_t sockfd)
{

    // // Opens file
    // FILE* file = fopen(filepath, "rb");
    // if (file == NULL) {
    //     perror("Error opening file");
    //     exit(1);
    // }

    // // Calculates file size in bytes
    // fseek(file, 0, SEEK_END);
    // size_t file_size = ftell(file);
    // fseek(file, 0, SEEK_SET);

    // char buffer[BUFFER_SIZE];

    // // Sends to client the header: filename/byte_size
    // sprintf(buffer, "%s/%ld", filename, file_size);
    // if (send(client_sockfd, buffer, strlen(buffer), 0) == -1) {
    //     perror("Error sending file");
    //     exit(EXIT_FAILURE);
    // }

    // // Waits for header acknowledgment
    // char ack;
    // if (recv(client_sockfd, &ack, sizeof(ack), MSG_WAITALL) <= 0) {
    //     perror("recv acknowledgment");
    //     fclose(file);
    //     exit(EXIT_FAILURE);
    // }

    // printf("Sending file \"%s\" of size %ld\n", filename, file_size);

    // // Amount of bytes read from the file
    // size_t bytes_read;

    // do {
    //     // Reads the file
    //     bytes_read = fread(buffer, 1, sizeof(buffer), file);

    //     // Ensures that all bytes are sent by looping
    //     size_t total_bytes_sent = 0;
    //     while (total_bytes_sent < bytes_read) {
    //         size_t bytes_sent = send(client_sockfd, buffer + total_bytes_sent, bytes_read - total_bytes_sent, 0);
    //         if (bytes_sent == -1) {
    //             perror("Error sending file");
    //             fclose(file);
    //             exit(1);
    //         }
    //         total_bytes_sent += bytes_sent;
    //     }

    // } while (bytes_read > 0);
    // printf("File sent!\n");
    // fclose(file);

    return ERR_NET_OK;
}
bool file_exists(const char* filepath)
{
    return false;
}

ErrorCode receive_file(uint32_t sockfd)
{
    return ERR_NET_OK;
}
