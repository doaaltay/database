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
    //char file_name[FIELDNAMELENGTH];
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

void add(char *table_name, char *data);
void create_table(char *name, Field fields[], int num_fields);

Field user_fields[]={
    {FIELD_TYPE_STRING, "name", maxdata},
    {FIELD_TYPE_STRING, "phone", maxdata},
    {FIELD_TYPE_STRING, "email", maxdata},
    {FIELD_TYPE_STRING, "birthday", maxdata},
    {FIELD_TYPE_STRING, "age", maxdata},
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

Header header;
void write_to_file(Table *table, Record *record) {
    FILE *file = fopen("databasenew.txt", "a");
    if (file == NULL) {
        printf("Failed to open file\n");
        return;
    }

    for (int i = 0; i < table->num_fields; i++) {
        if (table->fields[i].type == FIELD_TYPE_INT) {
            fprintf(file, "%d ", *((int *)record->values[i]));
        } else if (table->fields[i].type == FIELD_TYPE_STRING) {
           fprintf(file, "%s ", (char *)record->values[i]);

        //fwrite(record->values[i], 1, maxdata, file);
        }
    }

    fprintf(file, "\n");

    fclose(file);
}


void create_table(char *name, Field fields[], int num_fields) {
  
    if (header.num_tables >= max_num_tables) {
        printf("Maximum number of tables reached\n");
        return;
    }

 
    Table *table = &header.tables[header.num_tables++];

 
    strncpy(table->name, name, FIELDNAMELENGTH);

    // set the fields
    if (num_fields > max_num_fields) {
        printf("Too many fields\n");
        return;
    }
    for (int i = 0; i < num_fields; i++) {
        table->fields[i] = fields[i];
    }
    table->num_fields = num_fields;
}
void add(char *table_name, char *data) {
    // find the table
    Table *table = NULL;
    for (int i = 0; i < header.num_tables; i++) {
        if (strcmp(header.tables[i].name, table_name) == 0) {
            table = &header.tables[i];
            break;
        }
    }
    if (table == NULL) {
        printf("Table not found\n");
        return;
    }

    Record record;
    for (int i = 0; i < table->num_fields; i++) {
        record.values[i] = malloc(maxdata);
        if (record.values[i] == NULL) {
            printf("Memory allocation failed\n");
            return;
        }
        if (table->fields[i].type == FIELD_TYPE_INT) {
            *((int *)record.values[i]) = 0;  // intialize integer fields to 0
        } else if (table->fields[i].type == FIELD_TYPE_STRING) {
            ((char *)record.values[i])[0] = '\0';  // initialze string fields to empty string
        }
    }

    // Parse the data
    char *token, *tofree, *string;
    tofree = string = strdup(data);  
    for (int i = 0; i < table->num_fields; i++) {
        token = strsep(&string, ",");
        if (token != NULL && strlen(token) > 0) {
            if (table->fields[i].type == FIELD_TYPE_INT) {
                // parse the integer value and store it in the record
                *((int *)record.values[i]) = atoi(token);
            } else if (table->fields[i].type == FIELD_TYPE_STRING) {
                // copy the string value to the record and pad with spaces
                strncpy(record.values[i], token, maxdata);
                for (int j = strlen(token); j < maxdata; j++) {
                    ((char *)record.values[i])[j] = ' ';
                }
            }
        } else {
            // fill the field with spaces or zero
            if (table->fields[i].type == FIELD_TYPE_INT) {
                *((int *)record.values[i]) = 0;
            } else if (table->fields[i].type == FIELD_TYPE_STRING) {
                memset(record.values[i], ' ', maxdata);
            }
        }
    }

        // add the record to table
    write_to_file(table, &record);

    //free the memory allocated for the record
    for (int i = 0; i < table->num_fields; i++) {
        free(record.values[i]);
    }

    free(tofree);
}
void search(char *table_name, char *search_string) {
    
    Table *table = NULL;
    for (int i = 0; i < header.num_tables; i++) {
        if (strcmp(header.tables[i].name, table_name) == 0) {
            table = &header.tables[i];
            break;
        }
    }

    if (table == NULL) {
        printf("Table not found\n");
        return;
    }

    FILE *file = fopen("databasenew.txt", "r");
    if (file == NULL) {
        printf("Failed to open file\n");
        return;
    }

    char buffer[256];
    int match = 0;

    
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        
        char *field = strtok(buffer, " ");

        // get the second field
        if (field != NULL) {
            field = strtok(NULL, " ");
        }

        // compare field to the search string
        if (field != NULL && strcmp(field, search_string) == 0) {
            match = 1;
            printf("Record found: %s\n", buffer);
            break;
        }
    }

    if (!match) {
        printf("No record found with %s in the second field\n", search_string);
    }

    fclose(file);
}

void write_padded_value(FILE* file, char* new_value) {
    // Create a padded string with the new value
    char padded_string[maxdata + 1];
    int new_value_length = strlen(new_value);
    if (new_value_length > maxdata) {
        // Truncate the new value if it's too long
        new_value_length = maxdata;
    }
    int padding_length = maxdata - new_value_length;
    snprintf(padded_string, sizeof(padded_string), "%*s%.*s", padding_length, "", new_value_length, new_value);
    // Write the padded string to the file
    fputs(padded_string, file);
}



void update(char* table_name, char* search_value, char* field_name, char* new_value) {
   
    Table* table = NULL;
    for (int i = 0; i < header.num_tables; i++) {
        if (strcmp(header.tables[i].name, table_name) == 0) {
            table = &header.tables[i];
            break;
        }
    }
    if (table == NULL) {
        printf("Table not found\n");
        return;
    }

    
    FILE* file = fopen("databasenew.txt", "r+");
    if (file == NULL) {
        printf("Failed to open file\n");
        return;
    }

    //read each line
    char line[buffer_size];
    long int start_of_line = ftell(file);
    while (fgets(line, sizeof(line), file)) {
        // get second field which is used for update
        char* token = strtok(line, " ");
        if (token != NULL) {
            token = strtok(NULL, " ");
        }
        // check if this is the record to update
        if (token != NULL && strcmp(token, search_value) == 0) {
            // found the record 
            // now find the field to update
            for (int i = 0; i < table->num_fields; i++) {
                if (strcmp(table->fields[i].name, field_name) == 0) {
                    // found the field to update
                    // calculate the start position of the field in the line
                    int start_pos = i * maxdata;
                  
                    fseek(file, start_of_line + start_pos, SEEK_SET);
     
                    char padded_string[maxdata + 1];
                    snprintf(padded_string, sizeof(padded_string), "%-*s", maxdata, new_value);


                    fputs(padded_string, file);
                    fclose(file);
                    return;
                }
            }
            printf("Field not found\n");
            fclose(file);
            return;
        }
        start_of_line = ftell(file);
    }

    printf("Record not found\n");
    fclose(file);
}


void main() {

    FILE *file;
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
    else if (strcmp(cmd, "update") == 0) {
        if(tokens_length < 5){
            printf("Invalid number of arguments\n");
            printf("Usage: update <table_name> <search_value> <field_name> <new_value>\n");
            continue;
        }
     update(tokens[1], tokens[2], tokens[3], tokens[4]);
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

