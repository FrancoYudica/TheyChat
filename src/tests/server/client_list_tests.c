#include <stdio.h>
#include <stdbool.h>
#include "client_list.h" // Include the header file for the client list

// Test creating a new client list
bool test_create_client_list()
{
    ClientList* list = client_list_create();
    bool passed = list != NULL;
    if (passed) {
        printf("Test: Creating a new client list... PASSED\n");
    } else {
        printf("Test: Creating a new client list... FAILED\n");
    }
    return passed;
}

// Test adding a client to the list
bool test_add_client()
{
    ClientList* list = client_list_create();
    // Initialize client...
    Client* client = client_list_add(list);
    bool passed = client_list_length(list) == 1;
    if (passed) {
        printf("Test: Adding a client to the list... PASSED\n");
    } else {
        printf("Test: Adding a client to the list... FAILED\n");
    }
    // Clean up resources...
    client_list_destroy(list);
    return passed;
}

// Test removing a client from the list
bool test_remove_client()
{
    ClientList* list = client_list_create();
    // Initialize client...
    Client* client = client_list_add(list);
    bool removed = client_list_remove(list, client->id);
    bool passed = removed && client_list_length(list) == 0;
    if (passed) {
        printf("Test: Removing a client from the list... PASSED\n");
    } else {
        printf("Test: Removing a client from the list... FAILED\n");
    }
    // Clean up resources...
    client_list_destroy(list);
    return passed;
}

// Test finding a client by ID
bool test_find_client_by_id()
{
    ClientList* list = client_list_create();
    // Initialize client...
    Client* client = client_list_add(list);
    Client* found_client = client_list_find_by_id(list, client->id);
    bool passed = found_client != NULL;
    if (passed) {
        printf("Test: Finding a client by ID... PASSED\n");
    } else {
        printf("Test: Finding a client by ID... FAILED\n");
    }
    // Clean up resources...
    client_list_remove(list, client->id);
    client_list_destroy(list);
    return passed;
}

// Test getting the length of the client list
bool test_client_list_length()
{
    ClientList* list = client_list_create();

    // Initialize client1...
    Client* client1 = client_list_add(list);
    Client* client2 = client_list_add(list);

    bool passed = client_list_length(list) == 2;
    if (passed) {
        printf("Test: Getting the length of the client list... PASSED\n");
    } else {
        printf("Test: Getting the length of the client list... FAILED\n");
    }
    // Clean up resources...
    client_list_remove(list, client1->id);
    client_list_remove(list, client2->id);
    client_list_destroy(list);
    return passed;
}

int main()
{
    // Run the test cases
    bool passed_all = true;
    passed_all &= test_create_client_list();
    passed_all &= test_add_client();
    passed_all &= test_remove_client();
    passed_all &= test_find_client_by_id();
    passed_all &= test_client_list_length();

    if (passed_all) {
        printf("All tests PASSED\n");
    } else {
        printf("Some tests FAILED...\n");
    }

    return !passed_all;
}
