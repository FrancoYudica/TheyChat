#ifndef __FILE_TRANSFER_H__
#define __FILE_TRANSFER_H__
#include <stdbool.h>
#include "they_chat_error.h"
#include "messages/message_types.h"

bool file_exists(const char* filepath);
Error* send_file(const char* filepath, uint32_t sockfd);
Error* receive_file(uint32_t sockfd);

#endif