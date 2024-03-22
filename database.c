#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 100

typedef struct{
    char data[MAX_SIZE];
}DataType;

typedef struct{
    DataType items[MAX_SIZE];
    int size;
}Database;

//CRUD

//create

void create ( Database* db, DataType item){
    if (db->size < MAX_SIZE){
        db->items[db->size] = item;
        db->size++;
    }
}

//read

DataType read(Database* db, int index){
    if(index < 0 || index >= db->size){
        printf("Invalid index\n");
        DataType errorData;
        errorData.data[0] = '\0'; //empty datatype
        return errorData;
    }
    return db->items[index];
}

void update( Database* db, int index, DataType item){
    if (index < db->size){
        db->items[index] = item;
    }
}

void delete (Database* db, int index){
    if (index < db->size){
        for (int i = index; i < db->size - 1; i++){
            db->items[i] = db->items[i+1];
        }
        db->size--;
    }
}


//file operations

void write_to_file(Database* db, char* filename){
    FILE* file = fopen(filename, "w");
    if (file == NULL){
        printf("Error opening file-write\n");
        return;
    }
    for(int i=0; i< db->size;i++){
        fprintf(file, "%s\n", db->items[i].data);
    }

    fclose(file);
}

void read_from_file(Database* db, char* filename){
    FILE* file = fopen(filename, "r");
    if (file == NULL){
        printf("Error opening file-read\n");
        return;
    }
    char line[MAX_SIZE];
    while(fgets(line, MAX_SIZE, file) != NULL){
        //line[strcspn(line, "\n")] = 0; //remove newline char
        DataType item;
        strcpy(item.data, line);
        create(db, item);
    }
    fclose(file);
}

int main(){

    Database db; //create a database
    db.size = 0; //initializing size of database

    DataType item1, item2;

    //setting data
    strcpy(item1.data, "item1"); 
    strcpy(item2.data, "item2");

    //add to database
    create(&db, item1);
    create(&db, item2);

    DataType item1_updated;
    strcpy(item1_updated.data, "item1_updated");//updating
    update( &db, 0, item1_updated);

    delete(&db, 1);

    write_to_file(&db, "data.txt");

    Database db_from_file;
    db_from_file.size = 0;
    read_from_file(&db_from_file, "database.txt");


    for(int i=0; i< db_from_file.size; i++){
        printf("%s\n", db_from_file.items[i].data);
    }

    return 0;


}