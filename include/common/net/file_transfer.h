#ifndef __FILE_TRANSFER_H__
#define __FILE_TRANSFER_H__
#include <stdbool.h>
#include "net/net_error.h"
#include "messages/message_types.h"

bool file_exists(const char* filepath);
ErrorCode send_file(const char* filepath, uint32_t sockfd);
ErrorCode receive_file(uint32_t sockfd);

#endif