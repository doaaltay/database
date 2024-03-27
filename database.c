#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <ctype.h>

#define MAX_USER_DATABASE 100
#define NUMBER_NOT_FOUND "This phone number doesn't belong to any user"
#define NUMBER_FOUND "Number is found"
#define MAX_DATA 100


void add();
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

FILE *users_file;



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

/**
 * if gets too complicated keep the names same and makes them equal to the same size for each data field
*/

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

User userDatabase[MAX_USER_DATABASE];

void main() {

    printf("Welcome to the networking system!\n");

    users_file = fopen("users.csv", "r");
    if (users_file == NULL) {
        printf("Error opening file\n");
        return;
    }

 

    char *cmds = 
    "\nUsage:\n"
    "  execute <command> [options]\n\n"
    "Commands:\n"
    "  add                                   Add a new user.\n"
    "  addhelp                               Show the format to add a new user.\n"
    "  search <phone number>                 Search their name by entering the phone number.\n"
    "  update <phone number> <type> <change> Update <type> by phone number. Enter type to see types you can change.\n"
    "  delete <phone number>                 Delete user info by phone number.\n"
    "  users                                 List all users.\n"
    "  types                                 List types used in update.\n"
    "  help                                  Show this list of commands.\n"
    "  exit                                  Close this program and save updates.\n\n";
    printf("%s", cmds);
    
    char command[100];
    char *tokens[100];
    int tokens_length;

    
       while (1) {
        printf("Enter a command: ");
        fgets(command, sizeof(command), stdin);

       
        command[strcspn(command, "\n")] = 0; // remove newline character

//split to tokens

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
            if(tokens_length < 2){
                printf("Invalid number of arguments\n");
                printf("Usage: add <user info>\n");
                continue;
            }
            add(tokens[1]);
        } else if(strcmp(cmd, "types") == 0){
            types();
        } else if (strcmp(cmd, "help") == 0) {
            help();
        }
        else if(strcmp(cmd, "addhelp") == 0){
            addhelp();
        }
        
        else if (strcmp(cmd, "users") == 0) {
            users();
        }else if(strcmp(cmd, "search")== 0){
            search(tokens, tokens_length);
        }
        else if(strcmp(cmd, "update")== 0){
            update(tokens);
        }
        else if(strcmp(cmd, "delete")== 0){
            delete(tokens);
        }
         else if (strcmp(cmd, "exit") == 0) {
            break;
        } else {
            printf("Invalid command\n");
        }

        printf("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    }

    printf("Changes saved, thank you for using the network system!\n");



    fclose(users_file);
 
 
}

void addhelp(){
    printf("Enter user information in the following format:\n\n");
    printf("name,phone,birthday,email,age,job,city,pronouns,preferred(communication method),availability\n\n");
    printf("To skip information, enter two consecutive commas\n");
    printf("Example: Alex,1234567890,01/01/2000,,20,Engineer,New_York,he/him,phone,weekdays\n\n");

    printf("If you want to enter lastname enter it in this format: Alex-Smith\n");
    printf("DO NOT enter spaces when entering data\n");
}

void add(char* user_info){

    if(num_users >= MAX_USER_DATABASE){
        printf("Database is full\n");
        return;
    }   

    User new_user={0}; //initialize all fields to 0
    char* token;
    char* rest=user_info;

    FILE *file = fopen("users.csv", "a");
    if (file == NULL) {
        printf("Could not open file for writing\n");
        return;
    }
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
        // fixed width format
        sprintf(formatted, "%-*s", max_size, token);
        fprintf(file, "%s,", formatted);
    }

    fprintf(file, "\n");
    fclose(file);

    userDatabase[num_users++] = new_user;

}

void addold(char* user_info){

    if(num_users >= MAX_USER_DATABASE){
        printf("Database is full\n");
        return;
    }   

    User new_user={0}; //initialize all fields to 0
    char* token;
    char* rest=user_info;

    FILE *file = fopen("users.csv", "a");
    if (file == NULL) {
        printf("Could not open file for writing\n");
        return;
    }
    int field = 0;
    while ((token = strsep(&rest, ",")) != NULL) {
        char formatted[MAX_DATA];
        int max_size;
        switch (field) {
            case 0: max_size = maxname; break;
            case 1: max_size = maxphone; break;
            case 2: max_size = maxbirthday; break;
            case 3: max_size = maxemail; break;
            case 4: max_size = maxage; break;
            case 5: max_size = maxjob; break;
            case 6: max_size = maxcity; break;
            case 7: max_size = maxpronouns; break;
            case 8: max_size = maxpreferred; break;
            case 9: max_size = maxavailability; break;
            default: max_size = MAX_DATA; break;
        }
        // format the token with a fixed width
        sprintf(formatted, "%-*s", max_size, token);
        fprintf(file, "%s,", formatted);
        field++;
    }

    fprintf(file, "\n");
    fclose(file);

    userDatabase[num_users++] = new_user;

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


void users(){
    if(num_users == 0){
        printf("No users in the database\n");
        return;
    }
    FILE *file = fopen("users.csv", "r");
    if (file == NULL) {
        printf("Could not open file\n");
        return;
    }

    char line[1024];
    printf("List of users:\n");
    while (fgets(line, sizeof(line), file)) {
        char *name = strtok(line, ",");
        if (name != NULL) {
            printf("%s\n", name);
        }
    }

    fclose(file);
}

void search (char* tokens[], int tokens_length){
    char* phone_numb = tokens[1];

    FILE *file = fopen("users.csv", "r");
    if(file == NULL){
        printf("Could not open file for reading-search\n");
        return;
    }

    char line[1024];

    while(fgets(line, sizeof(line), file)){

        char* tmp= strdup(line); //temp holder
        char* token= strtok(tmp, ",");
        int field= 0;
        int match= 0;

        while (token!=NULL){
            if(field == 1){
                char* trimmed_token = trim(token);
                if(strcmp(trimmed_token, phone_numb) == 0){
                    match= 1;
                }
            }
            token= strtok(NULL, ",");
            field++;
        }
        if(match){
            printf("%s\n", line);
        }
        free(tmp);
    }

    fclose(file);
}

void types(){
    
     printf("name\nemail\nbirthday\nage\njob\ncity\npronouns\npreferred\navailability\n");

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

void update_best(char* tokens[]) {
    char* phone_numb = tokens[1];
    char* type = tokens[2];
    char* change = tokens[3];

    FILE *file = fopen("users.csv", "r");
    if(file == NULL){
        printf("Could not open file for reading-update\n");
        return;
    }

    char line[1024];
    char output[1024 * MAX_USER_DATABASE] = ""; //char array to hold the updated data
    int found = 0;

    while(fgets(line, sizeof(line), file)){
        char* tmp = strdup(line); // temp holder of current line
        char* token = strtok(tmp, ",");
        int field = 0; //counter for field
        int match = 0;

        char updated_line[1024] = ""; //hold updateds line

        while (token != NULL){
            char formatted_phone_numb[maxphone + 1]; 
            sprintf(formatted_phone_numb, "%-*s", maxphone, phone_numb);

            if(field == 1 && strcmp(token, formatted_phone_numb) == 0){ 
                match = 1;
                found = 1;
                printf("Matched phone number\n");
            }

            if(match && strcmp(type, field_to_string(field)) == 0){
                printf("Updating field %s\n", type);
                char formatted[MAX_DATA];
                int max_size;
                switch (field) {
                    case 0: max_size = maxname; break;
                    case 1: max_size = maxphone; break;
                    case 2: max_size = maxbirthday; break;
                    case 3: max_size = maxemail; break;
                    case 4: max_size = maxage; break;
                    case 5: max_size = maxjob; break;
                    case 6: max_size = maxcity; break;
                    case 7: max_size = maxpronouns; break;
                    case 8: max_size = maxpreferred; break;
                    case 9: max_size = maxavailability; break;
                    default: max_size = MAX_DATA; break;
                }
             
                sprintf(formatted, "%-*s", max_size, change);
                strcat(updated_line, formatted);
            } else {
                strcat(updated_line, token);
            }

            strcat(updated_line, ",");
            token = strtok(NULL, ",");
            field++;
        }

        strcat(output, updated_line);
        strcat(output, "\n");
        free(tmp);
    }

    fclose(file);

    if (!found) {
        printf("Phone number not found\n");
        return;
    }

    file = fopen("users.csv", "w");
    if(file == NULL){
        printf("Could not open file for writing-update\n");
        return;
    }

    fputs(output, file);
    fclose(file);

    // Update userDatabase array
    for (int i = 0; i < num_users; i++) {
        if (strcmp(userDatabase[i].phone, phone_numb) == 0) {
            // Update the field in the userDatabase array
            // ...
            break;
        }
    }
}
void update(char* tokens[]) {
    char* phone_numb = tokens[1];
    char* type = tokens[2];
    char* change = tokens[3];

    FILE *file = fopen("users.csv", "r");
    if(file == NULL){
        printf("Could not open file for reading-update\n");
        return;
    }

    char line[1024];
    char output[1024 * MAX_USER_DATABASE] = ""; //char array to hold the updated data
    int found = 0;

    while(fgets(line, sizeof(line), file)){
        char* tmp = strdup(line); // temp holder of current line
        char* token = strtok(tmp, ",");
        int field = 0; //counter for field
        int match = 0;

        char updated_line[1024] = ""; //hold updateds line

        while (token != NULL){
            char formatted_phone_numb[maxphone + 1]; 
            sprintf(formatted_phone_numb, "%-*s", maxphone, phone_numb);

            if(field == 1 && strcmp(token, formatted_phone_numb) == 0){ 
                match = 1;
                found = 1;
                printf("Matched phone number\n");
            }

            if(match && strcmp(type, field_to_string(field)) == 0){
                printf("Updating field %s\n", type);
                char formatted[MAX_DATA];
                int max_size;
                switch (field) {
                    case 0: max_size = maxname; break;
                    case 1: max_size = maxphone; break;
                    case 2: max_size = maxbirthday; break;
                    case 3: max_size = maxemail; break;
                    case 4: max_size = maxage; break;
                    case 5: max_size = maxjob; break;
                    case 6: max_size = maxcity; break;
                    case 7: max_size = maxpronouns; break;
                    case 8: max_size = maxpreferred; break;
                    case 9: max_size = maxavailability; break;
                    default: max_size = MAX_DATA; break;
                }
             
                sprintf(formatted, "%-*s", max_size, change);
                strcat(updated_line, formatted);
            } else {
                strcat(updated_line, token);
            }

            token = strtok(NULL, ",");
            if (token != NULL) {
                strcat(updated_line, ",");
            }
            field++;
        }

        strcat(output, updated_line);
        strcat(output, "\n");
        free(tmp);
    }

    fclose(file);

    if (!found) {
        printf("Phone number not found\n");
        return;
    }

    file = fopen("users.csv", "w");
    if(file == NULL){
        printf("Could not open file for writing-update\n");
        return;
    }

    fputs(output, file);
    fclose(file);

    // Update userDatabase array
    for (int i = 0; i < num_users; i++) {
        if (strcmp(userDatabase[i].phone, phone_numb) == 0) {
            // Update the field in the userDatabase array
            // ...
            break;
        }
    }
}
void update2(char* tokens[]) {
    
    /**
    update: when number is not matched make sure its printing out error, also its adding unneseccary commas
     * #define maxname 20
#define maxphone 11
#define maxemail 20
#define maxbirthday 10
#define maxage 3
#define maxjob 20
#define maxcity 15
#define maxpronouns 15
#define maxpreferred 15
#define maxavailability 15


    */
    char* phone_numb = tokens[1];
    char* type = tokens[2];
    char* change = tokens[3];

    FILE *file = fopen("users.csv", "r");
    if(file == NULL){
        printf("Could not open file for reading-update\n");
        return;
    }

    char line[1024];
    char output[1024 * MAX_USER_DATABASE] = ""; //char array to hold the updated data

    while(fgets(line, sizeof(line), file)){
        char* tmp = strdup(line); // temp holder of current line
        char* token = strtok(tmp, ",");
        int field = 0; //counter for field
        int match = 0;

        char updated_line[1024] = ""; //hold updateds line

        while (token != NULL){
            char formatted_phone_numb[maxphone + 1]; 
            sprintf(formatted_phone_numb, "%-*s", maxphone, phone_numb);

            if(field == 1 && strcmp(token, formatted_phone_numb) == 0){ 
                match = 1;
                printf("Matched phone number\n");
            }

            if(match && strcmp(type, field_to_string(field)) == 0){
                printf("Updating field %s\n", type);
                char formatted[MAX_DATA];
                int max_size;
                switch (field) {
                    case 0: max_size = maxname; break;
                    case 1: max_size = maxphone; break;
                    
                    case 3: max_size = maxemail; break;
                    case 4: max_size = maxage; break;
                    case 5: max_size = maxjob; break;
                    case 6: max_size = maxcity; break;
                    case 7: max_size = maxpronouns; break;
                    case 8: max_size = maxpreferred; break;
                    case 9: max_size = maxavailability; break;
                    default: max_size = MAX_DATA; break;
                }
             
                sprintf(formatted, "%-*s", max_size, change);
                strcat(updated_line, formatted);
            } else {
                strcat(updated_line, token);
            }

            strcat(updated_line, ",");
            token = strtok(NULL, ",");
            field++;
        }

        strcat(output, updated_line);
        strcat(output, "\n");
        free(tmp);
    }

    fclose(file);

    file = fopen("users.csv", "w");
    if(file == NULL){
        printf("Could not open file for writing-update\n");
        return;
    }

    fputs(output, file);
    fclose(file);
}

void delete(char* tokens[2]){
    if(tokens[1] == NULL){
        printf("Invalid number of arguments\n");
        printf("Usage: delete <phone number>\n");
        return;
    }

    char* phone_number = tokens[1];

    FILE *file = fopen("users.csv", "r");
    if(file == NULL){
        printf("Could not open file for reading\n");
        return;
    }

    FILE *tempfile = fopen("temp.csv", "w");
    if(tempfile == NULL){
        printf("Could not open temporary file for writing\n");
        fclose(file);
        return;
    }

    char line[1024];
    int found = 0;

    while(fgets(line, sizeof(line), file)){
        char* tmp = strdup(line); //temp holder
        char* token = strtok(tmp, ",");
        int field = 0;
        int match = 0;

        while (token != NULL){
            if(field == 1){
                char* trimmed_token = trim(token);
                if(strcmp(trimmed_token, phone_number) == 0){
                    match = 1;
                    found = 1;
                }
            }
            token = strtok(NULL, ",");
            field++;
        }

        if(!match){
            fprintf(tempfile, "%s", line);
        }

        free(tmp);
    }

    fclose(file);
    fclose(tempfile);

    if(found){
        
        remove("users.csv");
        rename("temp.csv", "users.csv");
    } else {
        printf("No user found with phone number: %s\n", phone_number);
        remove("temp.csv"); 
    }
}


void help(){
    printf("Commands:\n");
    printf("  add                                   Add a new user.\n");
    printf("  search <phone number>                 Search their name by entering the phone number.\n");
    printf("  update <phone number> <type> <change> Update <type> by phone number. Enter types to see types you can change.\n");
    printf("  delete <phone number>                 Delete user info by phone number.\n");
    printf("  users                                 List all users.\n");
    printf("  types                                 List types used in update.\n");
    printf("  help                                  Show this list of commands.\n");
    
    }
