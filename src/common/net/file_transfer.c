#include <unistd.h>
#include "net/file_transfer.h"
#include "net/net_communication.h"
#include "messages/message.h"
#include "file.h"

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
    const char* filename = filepath_get_filename(filepath);
    message = create_file_header_message(filename, file_size);
    err = send_message(&message, net_connection);

    if (IS_NET_ERROR(err)) {
        fclose(file);
        return err;
    }

    uint8_t buffer[FILE_CONTENT_SIZE];

    // Amount of bytes read from the file
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {

        message = create_file_content_message((const uint8_t*)buffer, bytes_read);
        err = send_message(&message, net_connection);

        if (IS_NET_ERROR(err))
            break;
    }

    fclose(file);

    // Sends end of file confirmation message
    message = create_file_end_message();
    err = send_message(&message, net_connection);

    return err;
}

Error* receive_file(NetworkConnection* net_connection)
{

    // Waits for header
    Message message;
    Error* err = wait_for_message_type(net_connection, &message, MSGT_FILE_HEADER);

    if (IS_NET_ERROR(err))
        return err;

    // Copies header, since message memory will be overwritten
    FileHeaderPayload header = message.payload.file_header;

    // Create file locally
    FILE* file = fopen(header.name, "wb");
    if (file == NULL)
        return CREATE_ERRNO(ERR_OPEN_FILE, "Error while opening receive file");

    // Receives all the bytes
    size_t total_bytes_received = 0;
    while (total_bytes_received < header.size) {
        err = wait_for_message_type(net_connection, &message, MSGT_FILE_CONTENT);

        if (IS_NET_ERROR(err))
            break;

        // Ensures that all bytes are written
        FileContentPayload* content = &message.payload.file_content;
        uint32_t written = 0;
        while (written < content->content_size) {

            // Writes received content into file
            written += fwrite(
                content->binary_payload + written,
                1,
                content->content_size - written,
                file);
        }

        total_bytes_received += written;
    }

    fclose(file);

    // Waits for end of file synchronization
    err = wait_for_message_type(
        net_connection,
        &message,
        MSGT_FILE_END);

    return err;
}
