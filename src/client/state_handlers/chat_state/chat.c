#include "chat_state/chat.h"

void chat_exit(ChatData* chat)
{
    pthread_mutex_lock(&chat->mutex);
    chat->active = false;
    pthread_cond_signal(&chat->exit_condition);
    pthread_mutex_unlock(&chat->mutex);
}
