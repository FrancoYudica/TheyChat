#include "collections/linked_list.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "collections/linked_list.h"

// Item structure for testing
typedef struct {
    int id;
    char name[50];
} Item;

// Test functions
int test_add_and_iterate()
{
    GenericList* list = generic_list_create(sizeof(Item));

    // Add items
    Item* item1 = generic_list_add(list);
    item1->id = 1;
    strcpy(item1->name, "Alice");

    Item* item2 = generic_list_add(list);
    item2->id = 2;
    strcpy(item2->name, "Bob");

    // Create iterator and check items
    GenericListIterator* iterator = generic_list_iterator_create(list);

    Item* item = (Item*)generic_list_iterator_next(iterator);
    assert(item != NULL);
    assert(item->id == 2);
    assert(strcmp(item->name, "Bob") == 0);

    item = (Item*)generic_list_iterator_next(iterator);
    assert(item != NULL);
    assert(item->id == 1);
    assert(strcmp(item->name, "Alice") == 0);

    item = (Item*)generic_list_iterator_next(iterator);
    assert(item == NULL);

    generic_list_iterator_destroy(iterator);
    generic_list_destroy(list);
    return 0;
}

int test_remove()
{
    GenericList* list = generic_list_create(sizeof(Item));

    // Add items
    Item* item1 = generic_list_add(list);
    item1->id = 1;
    strcpy(item1->name, "Alice");

    Item* item2 = generic_list_add(list);
    item2->id = 2;
    strcpy(item2->name, "Bob");

    // Remove second item (index 0)
    assert(generic_list_remove(list, 0) == true);

    // Create iterator and check remaining item
    GenericListIterator* iterator = generic_list_iterator_create(list);

    Item* item = (Item*)generic_list_iterator_next(iterator);
    assert(item != NULL);
    assert(item->id == 1);
    assert(strcmp(item->name, "Alice") == 0);

    item = (Item*)generic_list_iterator_next(iterator);
    assert(item == NULL);

    generic_list_iterator_destroy(iterator);
    generic_list_destroy(list);
    return 0;
}

int test_length()
{
    GenericList* list = generic_list_create(sizeof(Item));

    // Add items
    generic_list_add(list);
    generic_list_add(list);
    generic_list_add(list);

    // Check length
    assert(generic_list_length(list) == 3);

    // Remove one item and check length
    generic_list_remove(list, 1);
    assert(generic_list_length(list) == 2);

    generic_list_destroy(list);
    return 0;
}

int main()
{
    if (test_add_and_iterate() != 0) {
        printf("test_add_and_iterate failed\n");
        return 1;
    }

    if (test_remove() != 0) {
        printf("test_remove failed\n");
        return 1;
    }

    if (test_length() != 0) {
        printf("test_length failed\n");
        return 1;
    }

    printf("All tests passed!\n");
    return 0;
}
