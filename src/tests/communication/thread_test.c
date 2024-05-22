#include <thread_pool.h>

int main()
{
    thpool_t *thpool;

    thpool = thpool_create(4);
    thpool_destroy(thpool);
    return 0;
}