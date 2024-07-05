#include <unistd.h>
#include "net/file_transfer.h"
#include "net/net_communication.h"
#include "messages/message.h"

Error* send_file(
    const char* filepath,
    NetworkConnection* net_connection)
{
    // Opens file
    FILE* file = fopen(filepath, "rb");
    if (file == NULL)
        return CREATE_ERR(ERR_OPEN_FILE, "Unable to open file while sending");

    // Calculates file size in bytes
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    Error* err = CREATE_ERR_OK;

    Message message;

    // Sends header
    message = create_file_header_message("file", file_size);
    err = send_message(&message, net_connection);

    if (IS_NET_ERROR(err)) {
        fclose(file);
        return err;
    }

    printf("Sending file \"%s\" of size %ld\n", filepath, file_size);

    uint8_t buffer[FILE_CONTENT_SIZE];

    // Amount of bytes read from the file
    size_t bytes_read;
    do {
        // Reads the file
        bytes_read = fread(buffer, 1, sizeof(buffer), file);

        message = create_file_content_message((const uint8_t*)buffer, bytes_read);
        err = send_message(&message, net_connection);

        if (IS_NET_ERROR(err))
            break;

    } while (bytes_read > 0);

    fclose(file);

    // Not necessary, since receive keeps track of received bytes
    // message = create_file_end_message();
    // err = send_message(&message, net_connection);

    return err;
}
bool file_exists(const char* filepath)
{
    return access(filepath, F_OK);
}

Error* receive_file(NetworkConnection* net_connection)
{

    // Waits for header
    Message message;
    Error* err = wait_for_message_type(net_connection, &message, MSGT_FILE_HEADER);

    if (IS_NET_ERROR(err))
        return err;

    FileHeaderPayload* header = &message.payload.file_header;

    printf("Receiving file \"%s\" of size %d\n", header->name, header->size);

    // Create file locally
    FILE* file = fopen(header->name, "wb");
    if (file == NULL)
        return CREATE_ERRNO(ERR_OPEN_FILE, "Error while opening receive file");

    // Receives all the bytes
    size_t total_bytes_received = 0;
    while (total_bytes_received < header->size) {
        err = wait_for_message_type(net_connection, &message, MSGT_FILE_CONTENT);

        if (IS_NET_ERROR(err))
            break;

        FileContentPayload* content = &message.payload.file_content;
        total_bytes_received += content->content_size;

        // Writes received content into file
        fwrite(content->binary_payload, 1, content->content_size, file);
    }

    fclose(file);

    return err;
}
