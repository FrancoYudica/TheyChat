#include "net/file_transfer.h"
#include "net/net_communication.h"
#include <unistd.h>

ErrorCode send_file(const char* filepath, uint32_t sockfd)
{
    return ERR_NET_OK;
    // // Opens file
    // FILE* file = fopen(filepath, "rb");
    // if (file == NULL) {
    //     perror("Error opening file");
    //     return ERR_OPEN_FILE;
    // }

    // // Calculates file size in bytes
    // fseek(file, 0, SEEK_END);
    // size_t file_size = ftell(file);
    // fseek(file, 0, SEEK_SET);

    // ErrorCode err = ERR_NET_OK;

    // // Sends header
    // {
    //     FileHeaderMsg* header = create_file_header_message("file", file_size);
    //     err = send_message((const Message*)header, sockfd);

    //     if (IS_NET_ERROR(err)) {
    //         fclose(file);
    //         return err;
    //     }

    //     free(header);
    // }

    // printf("Sending file \"%s\" of size %ld\n", filepath, file_size);

    // uint8_t buffer[FILE_CONTENT_SIZE];

    // // Amount of bytes read from the file
    // size_t bytes_read;
    // do {
    //     // Reads the file
    //     bytes_read = fread(buffer, 1, sizeof(buffer), file);

    //     // Creates content message
    //     FileContentMsg* content_msg = create_file_content_message((const uint8_t*)buffer, bytes_read);

    //     // Sends content
    //     err = send_message((const Message*)content_msg, sockfd);

    //     if (IS_NET_ERROR(err))
    //         break;

    // } while (bytes_read > 0);

    // fclose(file);

    // return err;
}
bool file_exists(const char* filepath)
{
    return access(filepath, F_OK);
}

ErrorCode receive_file(uint32_t sockfd)
{
    return ERR_NET_OK;

    // char buffer[FILE_CONTENT_SIZE];
    // size_t file_size;

    // NetworkStream stream;
    // init_net_stream(&stream);

    // // Waits for header
    // FileHeaderMsg* header;
    // ErrorCode err = wait_for_message_type(&stream, sockfd, (Message**)&header, MSGT_FILE_HEADER);

    // if (IS_NET_ERROR(err))
    //     return err;

    // printf("Receiving file \"%s\" of size %d\n", header->name, header->size);

    // // Create file locally
    // FILE* file = fopen(header->name, "wb");
    // if (file == NULL) {
    //     perror("Error opening file");
    //     return ERR_OPEN_FILE;
    // }

    // // Receives all the bytes
    // size_t total_bytes_received = 0;
    // while (total_bytes_received < header->size) {
    //     FileContentMsg* content;
    //     err = wait_for_message_type(&stream, sockfd, (Message**)&content, MSGT_FILE_CONTENT);

    //     if (IS_NET_ERROR(err))
    //         break;

    //     total_bytes_received += content->content_size;

    //     // Writes received content into file
    //     fwrite(content->binary_payload, 1, content->content_size, file);
    // }

    // fclose(file);

    // return err;
}
