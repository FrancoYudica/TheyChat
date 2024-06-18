#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "collections/linked_list.h"

typedef struct {
    int id;
    char name[50];
} TestData;

void test_generic_list_add_and_remove()
{
    GenericList* list = generic_list_create(sizeof(TestData));

    TestData data1 = { 1, "Item1" };
    TestData data2 = { 2, "Item2" };
    TestData data3 = { 3, "Item3" };

    TestData* added_data1 = (TestData*)generic_list_add(list);
    memcpy(added_data1, &data1, sizeof(TestData));

    TestData* added_data2 = (TestData*)generic_list_add(list);
    memcpy(added_data2, &data2, sizeof(TestData));

    TestData* added_data3 = (TestData*)generic_list_add(list);
    memcpy(added_data3, &data3, sizeof(TestData));

    assert(generic_list_length(list) == 3);

    bool removed = generic_list_remove(list, 1);
    assert(removed == true);
    assert(generic_list_length(list) == 2);

    removed = generic_list_remove(list, 10); // Invalid index
    assert(removed == false);
    assert(generic_list_length(list) == 2);

    removed = generic_list_remove(list, 0);
    assert(removed == true);
    assert(generic_list_length(list) == 1);

    removed = generic_list_remove(list, 0);
    assert(removed == true);
    assert(generic_list_length(list) == 0);

    generic_list_destroy(list);
    printf("test_generic_list_add_and_remove passed.\n");
}

void test_generic_list_iterator()
{
    GenericList* list = generic_list_create(sizeof(TestData));

    TestData data1 = { 1, "Item1" };
    TestData data2 = { 2, "Item2" };
    TestData data3 = { 3, "Item3" };

    TestData* added_data1 = (TestData*)generic_list_add(list);
    memcpy(added_data1, &data1, sizeof(TestData));

    TestData* added_data2 = (TestData*)generic_list_add(list);
    memcpy(added_data2, &data2, sizeof(TestData));

    TestData* added_data3 = (TestData*)generic_list_add(list);
    memcpy(added_data3, &data3, sizeof(TestData));

    GenericListIterator* it = generic_list_iterator_create(list);

    TestData* data = (TestData*)generic_list_iterator_next(it);
    assert(data != NULL && data->id == 1 && strcmp(data->name, "Item1") == 0);

    data = (TestData*)generic_list_iterator_next(it);
    assert(data != NULL && data->id == 2 && strcmp(data->name, "Item2") == 0);

    data = (TestData*)generic_list_iterator_next(it);
    assert(data != NULL && data->id == 3 && strcmp(data->name, "Item3") == 0);

    data = (TestData*)generic_list_iterator_next(it);
    assert(data == NULL);

    generic_list_iterator_destroy(it);
    generic_list_destroy(list);
    printf("test_generic_list_iterator passed.\n");
}

int main()
{
    test_generic_list_add_and_remove();
    test_generic_list_iterator();
    return 0;
}
