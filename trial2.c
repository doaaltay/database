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

void serialize_record(Record* record, char* buffer, Table* table) {
    int offset = 0;
    for (int i = 0; i < table->num_fields; i++) {
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

void write_to_binary_file(void* data, size_t size, const char* filename) {
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Unable to open file\n");
        return;
    }

    fwrite(data, size, 1, file);
    fclose(file);
}

void read_from_binary_file(void* data, size_t size, const char* filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Unable to open file\n");
        return;
    }

void add(char* command) {
    /**
    data types: user, company, (data type created by user)
    should find which data type were adding
    get its table
    based on maxdata defined, put data for each field with fixed width(all data fields have same width)
    add it to binary file with write_to_file, serializer/deserializer methods*/
    
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
    

    deserialize_record(record,values,table);
}

void add_record(Table* table, Record* record) {
    // Serialize the record
    char* buffer = malloc(table->num_fields * maxdata);
    serialize_record(record, buffer, table);

    // Write the serialized record to the binary file
    write_to_binary_file(buffer, table->num_fields * maxdata, "database.bin");

    free(buffer);
}