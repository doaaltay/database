#define FIELDNAMELENGTH 20
#define max_num_fields 10
#define maxdata 20
#define max_num_tables 10
#define buffer_size 1000
#define EXIT_FAILURE 1
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


typedef enum{
    FIELD_TYPE_INT,
    FIELD_TYPE_STRING
}FieldType;

typedef struct{
    FieldType type;
    char name[FIELDNAMELENGTH];
    int size;
}Field;

typedef struct{
    char name[FIELDNAMELENGTH];
    Field fields[max_num_fields];
    int num_fields;
    
} Table;

typedef struct {
    void* values[max_num_fields];
} Record;

typedef struct{
    int num_records;
    int record_size;
    Table tables[max_num_tables];
    int num_tables;
} Header;


void write_to_binary_file(void* data, size_t size, const char* filename);
void read_from_binary_file(Record *recordDatabase, int *num_records, Table* table);
void serialize_record(Record* record, char* buffer, Table* table);
void deserialize_record(Record* record, char* buffer, Table* table);
void add_record(Table* table, Record* record);
void create_table(char *name, Field *fields, int num_fields);
void add(char* table_name, char* values);
void parse_values(char* values, Record* record, Table* table);
Header* get_header();
Table* find_table(char* name);
void search (char* table_name, char* search_value);
void print_second_field(Record* record, Table* table);
void* get_second_field(Record* record, Table* table);
void print_record(Record* record, Table* table);
void free_record(Record* record, Table* table);




Field user_fields[]={
    {FIELD_TYPE_STRING, "name", maxdata},
    {FIELD_TYPE_STRING, "phone", maxdata},
    {FIELD_TYPE_STRING, "email", maxdata},
    {FIELD_TYPE_STRING, "birthday", maxdata},
    {FIELD_TYPE_INT, "age", maxdata},
    {FIELD_TYPE_STRING, "job", maxdata},
    {FIELD_TYPE_STRING, "city", maxdata},
    {FIELD_TYPE_STRING, "pronouns", maxdata},
    {FIELD_TYPE_STRING, "preferred", maxdata},
    {FIELD_TYPE_STRING, "availability", maxdata}
};

Field company_fields[]={
    {FIELD_TYPE_STRING, "name", maxdata},
    {FIELD_TYPE_STRING, "phone", maxdata},
    {FIELD_TYPE_STRING, "email", maxdata},
    {FIELD_TYPE_STRING, "founded", maxdata},
    {FIELD_TYPE_STRING, "location", maxdata},
    {FIELD_TYPE_STRING, "industry", maxdata},
    {FIELD_TYPE_STRING, "hiring", maxdata}
};

void* get_second_field(Record* record, Table* table) {
    void* second_field_value = NULL;
    if (table->fields[1].type == FIELD_TYPE_INT) {
        second_field_value = malloc(sizeof(int));
        memcpy(second_field_value, record->values[1], sizeof(int));
    } else if (table->fields[1].type == FIELD_TYPE_STRING) {
        second_field_value = malloc(maxdata * sizeof(char));
        memcpy(second_field_value, record->values[1], maxdata);
    }
    return second_field_value;
    free(second_field_value);
}

void print_second_field(Record* record, Table* table) {
    void* second_field_value = get_second_field(record, table);
    if (table->fields[1].type == FIELD_TYPE_INT) {
        printf("Second field: %d\n", *(int*)second_field_value);
    } else if (table->fields[1].type == FIELD_TYPE_STRING) {
        printf("Second field: %s\n", (char*)second_field_value);
    }
    free(second_field_value);
}
Header header = {0, 0, {{0}}, 0};

void create_table(char *name, Field *fields, int num_fields) {
    Table new_table;
    strcpy(new_table.name, name);

    for (int i = 0; i < num_fields; i++) {
        fields[i].size = maxdata;
        new_table.fields[i] = fields[i];
    }

    new_table.num_fields = num_fields;

    header.tables[header.num_tables] = new_table;
    header.num_tables++;
}

void add(char* table_name, char* values) {
    Table* table = find_table(table_name);

    if (table == NULL) {
        printf("Table not found\n");
        return;
    }

    Record new_record;
    parse_values(values, &new_record, table);
    add_record(table, &new_record);
    free_record(&new_record, table);
}

void parse_values(char* values, Record* record, Table* table) {
     deserialize_record(record, values, table);
}



Header* get_header() {
    return &header;
}

Table* find_table(char* name) {
    for (int i = 0; i < header.num_tables; i++) {
        if (strcmp(header.tables[i].name, name) == 0) {
            return &header.tables[i];
        }
    }
    return NULL;
}

void serialize_record(Record* record, char* buffer, Table* table) {
    int offset = 0;
    for (int i = 0; i < table->num_fields; i++) {

        if (offset + sizeof(int) > buffer_size) { 
            fprintf(stderr, "Buffer overflow in serialize_record\n"); 
            exit(EXIT_FAILURE); 
        }

        if (table->fields[i].type == FIELD_TYPE_INT) {
            memcpy(buffer + offset, record->values[i], sizeof(int));
            offset += sizeof(int);
        } else if (table->fields[i].type == FIELD_TYPE_STRING) {
            memcpy(buffer + offset, record->values[i], maxdata);
            offset += maxdata;
        }
    }
}

void deserialize_record(Record* record, char* buffer, Table* table) {
    int offset = 0;
    for (int i = 0; i < table->num_fields; i++) {

        if (offset + sizeof(int) > buffer_size) { 
            fprintf(stderr, "Buffer overflow in deserialize_record\n");
            exit(EXIT_FAILURE);
        }
        if (table->fields[i].type == FIELD_TYPE_INT) {
            record->values[i] = malloc(sizeof(int));
            memcpy(record->values[i], buffer + offset, sizeof(int));
            offset += sizeof(int);
        } else if (table->fields[i].type == FIELD_TYPE_STRING) {
            record->values[i] = malloc(maxdata * sizeof(char));
            memcpy(record->values[i], buffer + offset, maxdata);
            offset += maxdata;
            
        }
    }
}

void free_record(Record* record, Table* table) {
    for (int i = 0; i < table->num_fields; i++) {
        free(record->values[i]);
    }
    free(record->values);
    free(record);
}



void print_record(Record* record, Table* table) {
    for(int i = 0; i < table->num_fields; i++) {
        printf("%s: ", table->fields[i].name);
        if(table->fields[i].type == FIELD_TYPE_INT) {
            printf("%d\n", *((int*)record->values[i]));
        } else if(table->fields[i].type == FIELD_TYPE_STRING) {
            printf("%s\n", (char*)record->values[i]);
        }
    }
}

void write_to_binary_file(void* data, size_t size, const char* filename) {
    filename = "databasetrial.bin";
    FILE *file = fopen(filename, "ab");
    if (file == NULL) {
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }

    size_t writeCount = fwrite(data, size, 1, file);
    if (writeCount != 1) {
        printf("Failed to write data to file\n");
    }

    fclose(file);
}

void read_from_binary_file(Record *recordDatabase, int *num_records, Table* table) {
    const char* filename = "databasetrial.bin";
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }

    Header header;
    fread(&header, sizeof(Header), 1, file);
    *num_records = header.num_records;

    char* buffer = malloc(table->num_fields * maxdata);

    for(int i = 0; i < header.num_records; i++) {
        fread(buffer, table->num_fields * maxdata, 1, file);
        deserialize_record(&recordDatabase[i], buffer, table);
    }

    free(buffer);
    fclose(file);
}

void add_record(Table* table, Record* record) {
 
    char* buffer = malloc(table->num_fields * maxdata);
    serialize_record(record, buffer, table);

   
    write_to_binary_file(buffer, table->num_fields * maxdata, "databasetrial.bin");

    free(buffer);
}

void search (char* table_name, char* search_value){
    Table* table = find_table(table_name);
    if(table == NULL){
        printf("Table not found\n");
        return;
    }

    FILE *file = fopen("databasetrial.bin", "rb");
    if(file == NULL){
        printf("Could not open file for reading-search\n");
        return;
    }

    Header header;
    fread(&header, sizeof(Header), 1, file);

    Record record;
    int match = 0;

    char* buffer = malloc(table->num_fields * maxdata);  

    for(int i = 0; i < header.num_records; i++) {
        fread(buffer, sizeof(Record), 1, file); 
        deserialize_record(&record, buffer, table);
        char* second_field = get_second_field(&record, table);
        if(strcmp(second_field, search_value) == 0){
            match = 1;
            print_record(&record, table);
           
            break;
        }
        free(second_field); 
        free_record(&record, table);
    }

    if(!match){
        printf("No record found with second field %s in table %s\n", search_value, table_name);
    }

    free(buffer); 
    fclose(file);
}
void main() {
    create_table("user", user_fields, 10);
    create_table("company", company_fields, 7);

    printf("Welcome to the system!\n");

    char *cmds = 
    "\nUsage:\n"
    "  execute <command> [options]\n\n"
    "Commands:\n"
    "  add  company <data>                       Add a new record.\n"
    "  add  user <data>                          Add a new record.\n"
    "  search company <search_value>             Search for a company record.\n"
    "  search user <search_value>                Search for a user record.\n"
    "  help                                      Show this list of commands.\n"
    "  exit                                      Close this program and save updates.\n\n";
    printf("%s", cmds);
    
    char command[100];
    char *tokens[100];
    int tokens_length;

    while (1) {
        printf("Enter a command: ");
        fgets(command, sizeof(command), stdin);

        command[strcspn(command, "\n")] = 0;

        char *token = strtok(command, " ");
        tokens_length = 0;
        while (token != NULL) {
            tokens[tokens_length++] = token;
            token = strtok(NULL, " ");
        }
        if(tokens_length == 0){
            continue;
        }
        char *cmd = tokens[0];

        if (strcmp(cmd, "add") == 0) {
            if(tokens_length < 3){
                printf("Invalid number of arguments\n");
                printf("Usage: add <table_name> <data>\n");
            continue;
    }
    add(tokens[1], tokens[2]);
}
else if (strcmp(cmd, "search") == 0) {
    if(tokens_length < 3){
        printf("Invalid number of arguments\n");
        printf("Usage: search <table_name> <search_value>\n");
        continue;
    }
    search(tokens[1], tokens[2]);
}
        else if(strcmp(cmd,"help")==0){
            printf("%s", cmds);
        }
        else if(strcmp(cmd,"exit")==0){
            break;
        }
        else{
            printf("Invalid command\n");
        }

        printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    }

    printf("Changes saved, thank you for using the system!\n");
}
