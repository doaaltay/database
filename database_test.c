#include <stdio.h>
#include <string.h>
#include "database.c"

void test_create() {
    Database db;
    db.size = 0;

    DataType item;
    strcpy(item.data, "test");

    create(&db, item);

    if (strcmp(db.items[0].data, "test") == 0) {
        printf("test_create passed\n");
    } else {
        printf("test_create failed\n");
    }
}

void test_read() {
    Database db;
    db.size = 0;

    DataType item;
    strcpy(item.data, "test");

    create(&db, item);

    DataType read_item = read(&db, 0);

    if (strcmp(read_item.data, "test") == 0) {
        printf("test_read passed\n");
    } else {
        printf("test_read failed\n");
    }
}

void test_update() {
    Database db;
    db.size = 0;

    DataType item;
    strcpy(item.data, "test");

    create(&db, item);

    DataType new_item;
    strcpy(new_item.data, "updated");

    update(&db, 0, new_item);

    if (strcmp(db.items[0].data, "updated") == 0) {
        printf("test_update passed\n");
    } else {
        printf("test_update failed\n");
    }
}

void test_delete() {
    Database db;
    db.size = 0;

    DataType item1, item2;
    strcpy(item1.data, "test1");
    strcpy(item2.data, "test2");

    create(&db, item1);
    create(&db, item2);

    delete(&db, 0);

    if (db.size == 1 && strcmp(db.items[0].data, "test2") == 0) {
        printf("test_delete passed\n");
    } else {
        printf("test_delete failed\n");
    }
}

int main() {
    test_create();
    test_read();
    test_update();
    test_delete();

    return 0;
}