#define FIELDNAMELENGTH 20
#define max_num_fields 10

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
    Field fields[max_num_fields];
    int num_fields;
} Table;

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
    memcpy(new_table.fields, fields, num_fields * sizeof(Field));
    new_table.num_fields = num_fields;
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