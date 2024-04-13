#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <assert.h>
#include <unistd.h>
#include "sockutils.h"
#include "db_library.h"

//nc localhost 8080

/**
 * doaaltay@DESKTOP-VJGGIDH:~/project3_sqlite$ gcc -o database database.c sockutils.c
doaaltay@DESKTOP-VJGGIDH:~/project3_sqlite$ ./database
Welcome to the networking system!

Usage:
  execute <command> [options]

Commands:
  adduser                                   Add a new user.
  adduserhelp                               Show the format to add a new user.
  searchuser <phone number>                 Search their name by entering the phone number.
  updateuser <phone number> <type> <change> Update <type> by phone number. Enter types to see types you can change.
  deleteuser <phone number>                 Delete user info by phone number.
  users                                     List all users.
  usertypes                                 List types used in update.
  addcompany                                Add a new company.
  addcompanyhelp                            Show the format to add a new company.
  searchcompany <name>                      Search the company by entering the name.
  updatecompany <phone> <type> <change>     Update <type> by company phone. Enter companytypes to see types you can change.
  deletecompany <phone>                     Delete company info by phone number.
  help                                      Show this list of commands.
  exit                                      Close this program and save updates.


*/
#define maxname 20
#define maxphone 11
#define maxemail 20
#define maxbirthday 10
#define maxage 3
#define maxjob 20
#define maxcity 15
#define maxpronouns 15
#define maxpreferred 15
#define maxavailability 15
#define MAX_DATA 100
#define MAX_USER_DATABASE 1000
#define MAX_COMPANY_DATABASE 1000


void add(char* user_info);
void users();
void search (char* tokens[], int tokens_length);
char *trim(char *str);
void update(char* tokens[4]);
void types();
void delete(char* tokens[2]);
void help();
void addhelp();
char* field_to_string(int field);

int num_users = 0;


int num_companies=0;

void addcompany(char* company_info);
void companies();
void searchcompany(char* tokens[], int tokens_length);
void updatecompany(char* tokens[4]);
void companytypes(); //types to update
void deletecompany(char* tokens[2]);
void addcompanyhelp();

FILE *users_binary;

//user

typedef struct{
    char name[maxname];
    char phone[maxphone];
    char email[maxemail];
    char birthday[maxbirthday];
    char age[maxage];
    char job[maxjob];
    char city[maxcity];// city they're based on
    char pronouns[maxpronouns];
    char preferred[MAX_DATA]; // preferred communication method
    char availability[MAX_DATA]; //person's availability 
} User;



typedef struct {
    char name[maxname];
    char phone[maxphone];
    char email[maxemail];
    char founded[maxbirthday];
    char location[maxcity];
    char industry[maxjob];
    char hiring[maxavailability];
} Company;

User userDatabase[MAX_USER_DATABASE];
Company companyDatabase[MAX_USER_DATABASE];

//header structure
typedef struct {
    int num_records;
    int record_size;
    char record_type[50];
} Header;

void serialize_user( User user, FILE *file) {
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

void write_to_binary_file(User *userDatabase, int num_users) {
    FILE *file = fopen("users.bin", "wb");
    if (file == NULL) {
        printf("Could not open file for writing\n");
        return;
    }

    Header header = {num_users, sizeof(User), "User"};
    fwrite(&header, sizeof(Header), 1, file);

    for(int i = 0; i < num_users; i++) {
        serialize_user(userDatabase[i], file);
    }

    fclose(file);

  
}

void read_from_binary_file(User *userDatabase, int *num_users) {
    FILE *file = fopen("users.bin", "rb");
    if (file == NULL) {
        printf("Could not open file for reading\n");
        return;
    }

    Header header;
    fread(&header, sizeof(Header), 1, file);
    *num_users = header.num_records;

    for(int i = 0; i < header.num_records; i++) {
        userDatabase[i] = deserialize_user(file);
    }

    fclose(file);
}

void write_to_binary_file_company(Company *companyDatabase, int num_companies) {
    FILE *file = fopen("companies.bin", "wb");
    if (file == NULL) {
        printf("Could not open file for writing\n");
        return;
    }

    Header header = {num_companies, sizeof(Company), "Company"};
    fwrite(&header, sizeof(Header), 1, file);

    for(int i = 0; i < num_companies; i++) {
        serialize_company(companyDatabase[i], file);
    }

    fclose(file);
}

void read_from_binary_file_company(Company *companyDatabase, int *num_companies) {
    FILE *file = fopen("companies.bin", "rb");
    if (file == NULL) {
        printf("Could not open file for reading\n");
        return;
    }

    Header header;
    fread(&header, sizeof(Header), 1, file);
    *num_companies = header.num_records;

    for(int i = 0; i < header.num_records; i++) {
        companyDatabase[i] = deserialize_company(file);
    }

    fclose(file);
}

void add(char* user_info){
    if(num_users >= MAX_USER_DATABASE){
        printf("Database is full\n");
        return;
    }   

    User new_user={0}; //initialize all fields to 0
    memset(&new_user, 0, sizeof(User)); // clear the struct
    new_user.age[0] = '\0'; // set age to an empty string
    char* token;
    char* rest=user_info;

    int field = 0;
    for(field = 0; field < 10; field++) {
        char formatted[MAX_DATA];
        int max_size;
        token = strsep(&rest, ",");
        if (token == NULL) {
            token = "";
        }
        switch (field) {
            case 0: max_size = maxname; strncpy(new_user.name, token, maxname); break;
            case 1: max_size = maxphone; strncpy(new_user.phone, token, maxphone); break;
            case 2: max_size = maxbirthday; strncpy(new_user.birthday, token, maxbirthday); break;
            case 3: max_size = maxemail; strncpy(new_user.email, token, maxemail); break;
            case 4: 
                max_size = maxage; 
                int age = atoi(token);
                snprintf(new_user.age, sizeof(new_user.age), "%d", age);
                break;
            case 5: max_size = maxjob; strncpy(new_user.job, token, maxjob); break;
            case 6: max_size = maxcity; strncpy(new_user.city, token, maxcity); break;
            case 7: max_size = maxpronouns; strncpy(new_user.pronouns, token, maxpronouns); break;
            case 8: max_size = maxpreferred; strncpy(new_user.preferred, token, maxpreferred); break;
            case 9: max_size = maxavailability; strncpy(new_user.availability, token, maxavailability); break;
            default: max_size = MAX_DATA; break;
        }
    }
    userDatabase[num_users++] = new_user;
    write_to_binary_file(userDatabase, num_users);
}   
void addcompany(char* company_info){
    if(num_companies >= MAX_COMPANY_DATABASE){
        printf("Database is full\n");
        return;
    }   

    Company new_company={0}; //initialize all fields to 0
    memset(&new_company, 0, sizeof(Company)); // clear the struct
    char* token;
    char* rest=company_info;

    int field = 0;
    for(field = 0; field < 7; field++) {
        char formatted[MAX_DATA];
        int max_size;
        token = strsep(&rest, ",");
        if (token == NULL) {
            token = "";
        }
        switch (field) {
            case 0: max_size = maxname; strncpy(new_company.name, token, maxname); break;

            case 1: max_size = maxphone; strncpy(new_company.phone, token, maxphone); break;
            case 2: max_size = maxemail; strncpy(new_company.email, token, maxemail); break;
            case 3: max_size = maxbirthday; strncpy(new_company.founded, token, maxbirthday); break;
            case 4: max_size = maxcity; strncpy(new_company.location, token, maxcity); break;
            case 5: max_size = maxjob; strncpy(new_company.industry, token, maxjob); break;
            case 6: max_size = maxavailability; strncpy(new_company.hiring, token, maxavailability); break;
            default: max_size = MAX_DATA; break;
        }
    }
    companyDatabase[num_companies++] = new_company;
    write_to_binary_file_company(companyDatabase, num_companies);
}
void update(char* tokens[]) {
    char* phone_numb = tokens[1];
    char* type = tokens[2];
    char* change = tokens[3];

    User userDatabase[MAX_USER_DATABASE];
    int num_users = 0;

    read_from_binary_file(userDatabase, &num_users);

    int found = 0;

    for(int i = 0; i < num_users; i++) {
        if(strcmp(userDatabase[i].phone, phone_numb) == 0) {
            found = 1;
            printf("Matched phone number\n");

            if(strcmp(type, "name") == 0) {
                strncpy(userDatabase[i].name, change, maxname);
            } else if(strcmp(type, "phone") == 0) {
                strncpy(userDatabase[i].phone, change, maxphone);
            } else if(strcmp(type, "birthday") == 0) {
                strncpy(userDatabase[i].birthday, change, maxbirthday);
            } else if(strcmp(type, "email") == 0) {
                strncpy(userDatabase[i].email, change, maxemail);
            } else if(strcmp(type, "age") == 0) {
                int age = atoi(change);
                snprintf(userDatabase[i].age, sizeof(userDatabase[i].age), "%d", age);
            } else if(strcmp(type, "job") == 0) {
                strncpy(userDatabase[i].job, change, maxjob);
            } else if(strcmp(type, "city") == 0) {
                strncpy(userDatabase[i].city, change, maxcity);
            } else if(strcmp(type, "pronouns") == 0) {
                strncpy(userDatabase[i].pronouns, change, maxpronouns);
            } else if(strcmp(type, "preferred") == 0) {
                strncpy(userDatabase[i].preferred, change, maxpreferred);
            } else if(strcmp(type, "availability") == 0) {
                strncpy(userDatabase[i].availability, change, maxavailability);
            }

            printf("Updated field %s\n", type);
            break;
        }
    }

    if (!found) {
        printf("Phone number not found\n");
        return;
    }

   
    write_to_binary_file(userDatabase, num_users);
}

void updatecompany(char* tokens[]) {
    char* company_phone = tokens[1];
    char* type = tokens[2];
    char* change = tokens[3];

    Company companyDatabase[MAX_COMPANY_DATABASE];
    int num_companies = 0;

    read_from_binary_file_company(companyDatabase, &num_companies);

    int found = 0;

    for(int i = 0; i < num_companies; i++) {
        if(strcmp(companyDatabase[i].phone, company_phone) == 0) {
            found = 1;
            printf("Matched company phone\n");

            if(strcmp(type, "name") == 0) {
                strncpy(companyDatabase[i].name, change, maxname);
            } else if(strcmp(type, "phone") == 0) {
                strncpy(companyDatabase[i].phone, change, maxphone);
            } else if(strcmp(type, "founded") == 0) {
                strncpy(companyDatabase[i].founded, change, maxbirthday);
            } else if(strcmp(type, "email") == 0) {
                strncpy(companyDatabase[i].email, change, maxemail);
            } else if(strcmp(type, "location") == 0) {
                strncpy(companyDatabase[i].location, change, maxcity);
            } else if(strcmp(type, "industry") == 0) {
                strncpy(companyDatabase[i].industry, change, maxjob);
            } else if(strcmp(type, "hiring") == 0) {
                strncpy(companyDatabase[i].hiring, change, maxavailability);
            }

            printf("Updated field %s\n", type);
            break;
        }
    }

    if (!found) {
        printf("Company phone not found\n");
        return;
    }

    write_to_binary_file_company(companyDatabase, num_companies);
}


void addcompanyhelp(){
    printf("Enter company information in the following format:\n\n");
    printf("name,phone,email,founded,location,industry,hiring\n\n");
    printf("To skip information, enter two consecutive commas\n");
    printf("Example: Google,1234567890\n");
}


void addhelp(){
    printf("Enter user information in the following format:\n\n");
    printf("name,phone,birthday,email,age,job,city,pronouns,preferred(communication method),availability\n\n");
    printf("To skip information, enter two consecutive commas\n");
    printf("Example: Alex,1234567890,01/01/2000,,20,Engineer,New_York,he/him,phone,weekdays\n\n");

    printf("If you want to enter lastname enter it in this format: Alex-Smith\n");
    printf("DO NOT enter spaces when entering data\n");
}

char *trim(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0) 
        return str;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';

    return str;
}


char* field_to_string(int field) {
    switch(field) {
        case 0: return "name";
        case 1: return "phone";
        case 2: return "birthday";
        case 3: return "email";
        case 4: return "age";
        case 5: return "job";
        case 6: return "city";
        case 7: return "pronouns";
        case 8: return "preferred";
        case 9: return "availability";
        default: return "";
    }
}

void search (char* tokens[], int tokens_length){
    char* phone_numb = tokens[1];

    FILE *file = fopen("users.bin", "rb");
    if(file == NULL){
        printf("Could not open file for reading-search\n");
        return;
    }

    Header header;
    fread(&header, sizeof(Header), 1, file);

    User user;
    int match = 0;

    for(int i = 0; i < header.num_records; i++) {
        user = deserialize_user(file);
        char* trimmed_phone = trim(user.phone); // trim the phone number
        if(strcmp(trimmed_phone, phone_numb) == 0){
            match = 1;
            printf("Name: %s\n", user.name);
            printf("Phone: %s\n", user.phone);
            printf("Email: %s\n", user.email);
            printf("Birthday: %s\n", user.birthday);
            printf("Age: %s\n", user.age);
            printf("Job: %s\n", user.job);
            printf("City: %s\n", user.city);
            printf("Pronouns: %s\n", user.pronouns);
            printf("Preferred: %s\n", user.preferred);
            printf("Availability: %s\n", user.availability);
            break;
        }
    }

    if(!match){
        printf("No user found with phone number %s\n", phone_numb);
    }

    fclose(file);
}

void searchcompany(char* tokens[], int tokens_length){
    char* company_name = tokens[1];

    FILE *file = fopen("companies.bin", "rb");
    if(file == NULL){
        printf("Could not open file for reading-search\n");
        return;
    }

    Header header;
    fread(&header, sizeof(Header), 1, file);

    Company company;
    int match = 0;

    for(int i = 0; i < header.num_records; i++) {
        company = deserialize_company(file);
        char* trimmed_name = trim(company.name); // trim the company name
        if(strcmp(trimmed_name, company_name) == 0){
            match = 1;
            printf("Name: %s\n", company.name);
            printf("Phone: %s\n", company.phone);
            printf("Email: %s\n", company.email);
            printf("Founded: %s\n", company.founded);
            printf("Location: %s\n", company.location);
            printf("Industry: %s\n", company.industry);
            printf("Hiring: %s\n", company.hiring);
            break;
        }
    }

    if(!match){
        printf("No company found with name %s\n", company_name);
    }

    fclose(file);
}

void users() {
    User userDatabase[MAX_USER_DATABASE];
    int num_users = 0;
    read_from_binary_file(userDatabase, &num_users);
    for(int i = 0; i < num_users; i++) {
        printf("%s, %s \n", userDatabase[i].name, userDatabase[i].phone);
    }
}
void companies() {
    Company companyDatabase[MAX_COMPANY_DATABASE];
    int num_companies = 0;
    read_from_binary_file_company(companyDatabase, &num_companies);
    for(int i = 0; i < num_companies; i++) {
        printf("%s, %s \n", companyDatabase[i].name, companyDatabase[i].phone);
    }
}

void types(){
    
     printf("name\nemail\nbirthday\nage\njob\ncity\npronouns\npreferred\navailability\n");

}

void companytypes(){
    printf("name\nphone\nemail\nfounded\nlocation\nindustry\nhiring\n");
}

void delete(char* tokens[2]) {
    char* phone_numb = tokens[1];
    User userDatabase[MAX_USER_DATABASE];
    int num_users = 0;

    read_from_binary_file(userDatabase, &num_users);

    int found = 0;

    for(int i = 0; i < num_users; i++) {
        if(strcmp(userDatabase[i].phone, phone_numb) == 0) {
            found = 1;
            printf("Matched phone number, deleting user\n");

          
            for(int j = i; j < num_users - 1; j++) {
                userDatabase[j] = userDatabase[j + 1];
            }

            num_users--;
            break;
        }
    }

    if (!found) {
        printf("Phone number not found\n");
        return;
    }

    write_to_binary_file(userDatabase, num_users);
}

void deletecompany(char* tokens[2]) {
    char* phone_numb = tokens[1];
    Company companyDatabase[MAX_COMPANY_DATABASE];
    int num_companies = 0;

    read_from_binary_file_company(companyDatabase, &num_companies);

    int found = 0;

    for(int i = 0; i < num_companies; i++) {
        if(strcmp(companyDatabase[i].phone, phone_numb) == 0) {
            found = 1;
            printf("Matched phone number, deleting company\n");

            for(int j = i; j < num_companies - 1; j++) {
                companyDatabase[j] = companyDatabase[j + 1];
            }

            num_companies--;
            break;
        }
    }

    if (!found) {
        printf("Phone number not found\n");
        return;
    }

    write_to_binary_file_company(companyDatabase, num_companies);
}