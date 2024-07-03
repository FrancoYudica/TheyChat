#include "client.h"

Client* get_client()
{
    static Client data;
    return &data;
}
