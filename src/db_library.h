#ifndef DB_LIBRARY_H
#define DB_LIBRARY_H


void add(char* user_info);
void users();
void search (char* tokens[], int tokens_length);
char *trim(char *str);
void update(char* tokens[4]);
void types();
void help();
void addhelp();
char* field_to_string(int field);


void addcompany(char* company_info);
void companies();
void searchcompany(char* tokens[], int tokens_length);
void updatecompany(char* tokens[4]);
void companytypes(); //types to update
void deletecompany(char* tokens[2]);
void addcompanyhelp();

#endif