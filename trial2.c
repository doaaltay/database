#define FIELDNAMELENGTH 20
#define max_num_fields 10
#define maxdata 20

void create_table(char *name, Field *fields, int num_fields);
void serialize_user(User user, FILE *file);
User deserialize_user(FILE *file);
void serialize_company(Company company, FILE *file);
Company deserialize_company(FILE *file);
void add(char* command);
void parse_values(char* values, Record* record, Table* table);


FILE *databasefile;

typedef enum{
    FIELD_TYPE_INT,
    FIELD_TYPE_STRING
}FieldType;

typedef struct{
    FieldType type;
    char name[FIELDNAMELENGTH]
}Field;

typedef struct{
    char name[FIELDNAMELENGTH];
    Field* fields;
    int num_fields;
} Table;

typedef struct {
    void* values[max_num_fields];
} Record;

Field user_fields[]={
    {"name", FIELD_TYPE_STRING, maxdata},
    {"phone", FIELD_TYPE_STRING, maxdata},
    {"email", FIELD_TYPE_STRING, maxdata},
    {"birthday", FIELD_TYPE_STRING, maxdata},
    {"age", FIELD_TYPE_INT, maxdata},
    {"job", FIELD_TYPE_STRING, maxdata},
    {"city", FIELD_TYPE_STRING, maxdata},
    {"pronouns", FIELD_TYPE_STRING, maxdata},
    {"preferred", FIELD_TYPE_STRING, maxdata},
    {"availability", FIELD_TYPE_STRING, maxdata}
};
Table user_table = {user_fields, sizeof(user_fields) / sizeof(Field)};

Field company_fields[]={
    {"name", FIELD_TYPE_STRING, maxdata},
    {"phone", FIELD_TYPE_STRING, maxdata},
    {"email", FIELD_TYPE_STRING, maxdata},
    {"founded", FIELD_TYPE_STRING, maxdata},
    {"location", FIELD_TYPE_STRING, maxdata},
    {"industry", FIELD_TYPE_STRING, maxdata},
    {"hiring", FIELD_TYPE_STRING, maxdata}
};
 Table company_table = {company_fields, sizeof(company_fields) / sizeof(Field)};


typedef struct{
    int num_records;
    int record_size;
    DataType record_type;
    Table tables[max_num_tables];
    int num_tables;
} Header;

void create_table(char *name, Field *fields, int num_fields) {
    Table new_table;
    strcpy(new_table.name, name);

    // iterate over fields to set it size
    for (int i = 0; i < num_fields; i++) {
        fields[i].size = maxdata;
    }

    memcpy(new_table.fields, fields, num_fields * sizeof(Field));

    new_table.num_fields = num_fields; //set num fields

    Header *header = get_header();

    header->tables[header->num_tables] = new_table;
    
    header->num_tables++;
}

void serialize_user(User user, FILE *file) {
    fwrite(&user, sizeof(User), 1, file);
}

User deserialize_user(FILE *file) {
    User user;
    fread(&user, sizeof(User), 1, file);
    return user;
}

void serialize_company(Company company, FILE *file) {
    fwrite(&company, sizeof(Company), 1, file);
}

Company deserialize_company(FILE *file) {
    Company company;
    fread(&company, sizeof(Company), 1, file);
    return company;
}

void add(char* command) {
    
    char* table_name = strtok(command, " ");
    char* values = strtok(NULL, " ");

    // find the table
    Table* table = find_table(table_name);
    if (table == NULL) {
        printf("Table not found\n");
        return;
    }

    Record new_record;
   
    parse_values(values, &new_record);

    add_record(table, &new_record);
}

void parse_values(char* values, Record* record, Table* table) {
    /**
        * Parse the values string and set the values in the record
        
    */
    for (int i = 0; i < table->num_fields; i++) {
        char* value = strtok(values, ",");
        if (table->fields[i].type == FIELD_TYPE_INT) {
            int* int_value = malloc(sizeof(int));
            *int_value = atoi(value);
            record->values[i] = int_value;
        } else if (table->fields[i].type == FIELD_TYPE_STRING) {
            char* str_value = malloc(maxdata * sizeof(char)); 
            strncpy(str_value, value, maxdata-1); //fixed size for each data
            str_value[maxdata-1] = '\0'; // Ensure null-termination
            record->values[i] = str_value;
        }
        values = NULL;
    }
}