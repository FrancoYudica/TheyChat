#include <unistd.h>
#include "client/state_handler_utils.h"

extern ErrorCode handle_state_disconnect(ClientData *data, AppState *next_state)
{

    printf("Closing socket fd %d\n", data->sockfd);

    if (close(data->sockfd) == -1)
        perror("close");

    return ERR_NET_OK;
}
