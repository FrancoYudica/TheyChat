#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "client_list.h"
#include "client.h"

void test_client_list_create_and_destroy()
{
    ClientList* client_list = client_list_create();
    assert(client_list != NULL);
    client_list_destroy(client_list);
    printf("test_client_list_create_and_destroy passed.\n");
}

void test_client_list_add_and_remove()
{
    ClientList* client_list = client_list_create();
    assert(client_list != NULL);

    // Add clients
    Client* client1 = client_list_add(client_list);
    strcpy(client1->name, "Client1");

    Client* client2 = client_list_add(client_list);
    strcpy(client2->name, "Client2");

    Client* client3 = client_list_add(client_list);
    strcpy(client3->name, "Client3");

    assert(client_list_length(client_list) == 3);

    // Remove client by ID
    bool removed = client_list_remove(client_list, client2->id);
    assert(removed == true);
    assert(client_list_length(client_list) == 2);

    removed = client_list_remove(client_list, client2->id); // Already removed
    assert(removed == false);

    client_list_destroy(client_list);
    printf("test_client_list_add_and_remove passed.\n");
}

void test_client_list_find_by_id()
{
    ClientList* client_list = client_list_create();
    assert(client_list != NULL);

    // Add clients
    Client* client1 = client_list_add(client_list);
    strcpy(client1->name, "Client1");

    Client* client2 = client_list_add(client_list);
    strcpy(client2->name, "Client2");

    Client* client3 = client_list_add(client_list);
    strcpy(client3->name, "Client3");

    // Find clients by ID
    Client* found_client = client_list_find_by_id(client_list, client2->id);
    assert(found_client != NULL);
    assert(strcmp(found_client->name, "Client2") == 0);

    found_client = client_list_find_by_id(client_list, 9999); // Non-existent ID
    assert(found_client == NULL);

    client_list_destroy(client_list);
    printf("test_client_list_find_by_id passed.\n");
}

void test_client_list_find_by_name()
{
    ClientList* client_list = client_list_create();
    assert(client_list != NULL);

    // Add clients
    Client* client1 = client_list_add(client_list);
    strcpy(client1->name, "Client1");

    Client* client2 = client_list_add(client_list);
    strcpy(client2->name, "Client2");

    Client* client3 = client_list_add(client_list);
    strcpy(client3->name, "Client3");

    // Find clients by name
    Client* found_client = client_list_find_by_name(client_list, "Client2");
    assert(found_client != NULL);
    assert(found_client->id == client2->id);

    found_client = client_list_find_by_name(client_list, "NonExistentClient");
    assert(found_client == NULL);

    client_list_destroy(client_list);
    printf("test_client_list_find_by_name passed.\n");
}

void test_client_list_iterator()
{
    ClientList* client_list = client_list_create();
    assert(client_list != NULL);

    // Add clients
    Client* client1 = client_list_add(client_list);
    strcpy(client1->name, "Client1");

    Client* client2 = client_list_add(client_list);
    strcpy(client2->name, "Client2");

    Client* client3 = client_list_add(client_list);
    strcpy(client3->name, "Client3");

    // Use iterator
    client_list_interator_rewind(client_list);
    Client* client = client_list_interator_next(client_list);
    assert(client != NULL);
    assert(strcmp(client->name, "Client1") == 0);

    client = client_list_interator_next(client_list);
    assert(client != NULL);
    assert(strcmp(client->name, "Client2") == 0);

    client = client_list_interator_next(client_list);
    assert(client != NULL);
    assert(strcmp(client->name, "Client3") == 0);

    client = client_list_interator_next(client_list);
    assert(client == NULL); // End of list

    client_list_destroy(client_list);
    printf("test_client_list_iterator passed.\n");
}

int main()
{
    test_client_list_create_and_destroy();
    test_client_list_add_and_remove();
    test_client_list_find_by_id();
    test_client_list_find_by_name();
    test_client_list_iterator();
    return 0;
}
