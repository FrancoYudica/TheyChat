#include "client_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client.h"

// Node structure to hold client and the next pointer
typedef struct Node 
{
    Client *client;
    struct Node *next;
} Node;

struct ClientList 
{
    Node *front;
    Node *rear;
    size_t element_size;
    size_t size;
    uint32_t next_client_id;
};

ClientList* client_list_create() 
{
    ClientList *list = (ClientList*)malloc(sizeof(ClientList));
    if (list == NULL) {
        perror("Failed to initialize list");
        exit(EXIT_FAILURE);
    }
    list->front = list->rear = NULL;
    list->element_size = sizeof(ClientList);
    list->size = 0;
    list->next_client_id = 0;
    return list;
}

void client_list_destroy(ClientList *client_list) 
{
    Node *current = client_list->front;
    while (current != NULL) 
    {
        Node *next = current->next;
        free(current->client);
        free(current);
        current = next;
    }
    free(client_list);
}

Client* client_list_add(ClientList *client_list) 
{
    // Allocates memory for next node
    Node *new_node = (Node*)malloc(sizeof(Node));
    if (new_node == NULL) 
    {
        perror("Failed to allocate memory for new node");
        exit(EXIT_FAILURE);
    }

    // Allocates memory for Client
    new_node->client = malloc(client_list->element_size);
    if (new_node->client == NULL) 
    {
        free(new_node);
        perror("Failed to allocate memory for node client");
        exit(EXIT_FAILURE);
    }

    memset(new_node->client, 0, client_list->element_size);
    new_node->next = NULL;

    // Sets client ID
    new_node->client->id = client_list->next_client_id++;

    if (client_list->size == 0) 
        client_list->front = client_list->rear = new_node;
    else 
    {
        // Adds node at the end
        client_list->rear->next = new_node;
        client_list->rear = new_node;
    }
    client_list->size++;
    
    return new_node->client;
}

bool client_list_remove(ClientList* client_list, uint32_t client_id) 
{
    if (client_list->size == 0) 
    {
        fprintf(stderr, "ClientList underflow\n");
        exit(EXIT_FAILURE);
    }

    // Loops through all the nodes until it finds the client with the specified ID
    Node *previous = NULL; 
    Node *current = client_list->front;
    while (current != NULL) 
    {

        // When node is found
        if (current->client->id == client_id)
        {
            
            // 1 - Unlinks node
            // When current is front
            if (current == client_list->front)
            {
                client_list->front = current->next;
            }

            // When current is rear
            else if (current == client_list->rear)
            {
                previous->next = NULL;
            }

            // When current is inside the edges of the list
            else
            {
                previous->next = current->next;
            }
            
            // 2 - Frees client and node memory
            free(current->client);
            free(current);
            client_list->size--;
            return true;
        }

        Node *next = current->next;
        previous = current;
        current = next;
    }

    return false;
}

Client* client_list_find_by_id(ClientList *client_list, uint32_t client_id)
{
    // Loops through all the nodes until it finds the client with the specified ID
    Node *current = client_list->front;
    while (current != NULL) 
    {
        Node *next = current->next;
        if (current->client->id == client_id)
            return current->client;

        current = next;
    }

    // Not found
    return NULL;
}



size_t client_list_length(ClientList* client_list) 
{
    return client_list->size;
}