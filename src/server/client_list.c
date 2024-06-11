#include "client_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"

// Node structure to hold client and the next pointer
typedef struct Node {
    Client client;
    struct Node* next;
} Node;

struct ClientList {
    Node* front;
    Node* rear;
    Node* iterator_node;
    size_t element_size;
    size_t size;
    uint32_t next_client_id;
};

ClientList* client_list_create()
{
    ClientList* list = (ClientList*)malloc(sizeof(ClientList));
    if (list == NULL) {
        perror("Failed to initialize list");
        exit(EXIT_FAILURE);
    }
    list->front = list->rear = NULL;
    list->element_size = sizeof(Client);
    list->size = 0;
    list->next_client_id = 0;
    return list;
}

void client_list_destroy(ClientList* client_list)
{
    Node* current = client_list->front;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    client_list->iterator_node = NULL;
    free(client_list);
}

Client* client_list_add(ClientList* client_list)
{
    // Allocates memory for next node
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) {
        perror("Failed to allocate memory for new node");
        exit(EXIT_FAILURE);
    }

    memset(&new_node->client, 0, client_list->element_size);
    new_node->next = NULL;

    // Sets client ID
    new_node->client.id = client_list->next_client_id++;

    if (client_list->size == 0)
        client_list->front = client_list->rear = new_node;
    else {
        // Adds node at the end
        client_list->rear->next = new_node;
        client_list->rear = new_node;
    }
    client_list->size++;

    return &new_node->client;
}

bool client_list_remove(ClientList* client_list, uint32_t client_id)
{
    if (client_list->size == 0) {
        fprintf(stderr, "ClientList underflow\n");
        exit(EXIT_FAILURE);
    }

    // Loops through all the nodes until it finds the client with the specified ID
    Node* previous = NULL;
    Node* current = client_list->front;
    while (current != NULL) {

        // When node is found
        if (current->client.id == client_id) {

            // 1 - Unlinks node
            // When current is front
            if (current == client_list->front) {
                client_list->front = current->next;
            }

            // When current is rear
            else if (current == client_list->rear) {
                previous->next = NULL;
                client_list->rear = previous;
            }

            // When current is inside the edges of the list
            else {
                previous->next = current->next;
            }
            if (current == client_list->iterator_node)
                client_list->iterator_node = current->next;

            // Frees node, alongside with client
            free(current);

            if (--client_list->size == 0) {
                client_list->front = NULL;
                client_list->rear = NULL;
            }

            return true;
        }

        Node* next = current->next;
        previous = current;
        current = next;
    }

    return false;
}

Client* client_list_find_by_id(ClientList* client_list, uint32_t client_id)
{
    // Loops through all the nodes until it finds the client with the specified ID
    Node* current = client_list->front;
    while (current != NULL) {
        Node* next = current->next;
        if (current->client.id == client_id)
            return &current->client;

        current = next;
    }

    // Not found
    return NULL;
}

Client* client_list_find_by_name(ClientList* client_list, const char* name)
{
    // Loops through all the nodes until it finds the client with the specified ID
    Node* current = client_list->front;
    while (current != NULL) {
        Node* next = current->next;
        if (!strcmp(current->client.name, name))
            return &current->client;

        current = next;
    }

    // Not found
    return NULL;
}

size_t client_list_length(ClientList* client_list)
{
    return client_list->size;
}

Client* client_list_interator_next(ClientList* client_list)
{
    if (client_list->size == 0)
        return NULL;

    Node* current = client_list->iterator_node;
    if (current == NULL)
        return NULL;

    // Advances
    client_list->iterator_node = current->next;

    return &current->client;
}

void client_list_interator_rewind(ClientList* client_list)
{
    client_list->iterator_node = client_list->front;
}
