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
int num_users = 0;

FILE *users_file;



typedef struct{
    char name[MAX_DATA];
    char phone[MAX_DATA];
    char email[MAX_DATA];
    char birthday[MAX_DATA];
    char age[MAX_DATA];
    char job[MAX_DATA];
    char city[MAX_DATA];// city they're based on
    char pronouns[MAX_DATA];
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
    printf("Example: Alex,1234567890,01/01/2000,,20,Engineer,New York,he/him,phone,weekdays\n\n");

    printf("If you want to enter lastname enter it in this format: Alex-Smith\n");
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
    while ((token = strsep(&rest, ",")) != NULL) {
        if (field == 1) { // phone number is the second field
            strncpy(new_user.phone, token, sizeof(new_user.phone) - 1);
            new_user.phone[sizeof(new_user.phone) - 1] = '\0';
            char* trimmed_phone = trim(new_user.phone);
            strncpy(new_user.phone, trimmed_phone, sizeof(new_user.phone) - 1);
            token = new_user.phone; // replace the token with the trimmed phone number
        }
        fprintf(file, "%s,", token);
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
            if(field == 1 && strcmp(token, phone_numb) == 0){
                match= 1;
                
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


void update(char* tokens[4]){
    if(tokens[1]== NULL || tokens[2] == NULL || tokens[3] == NULL){
        printf("Invalid number of arguments\n");
        printf("Usage: update <phone number> <type> <change>\n");
        return;
    }

    char* phone_numb = tokens[1];
    char* type = tokens[2];
    char* change = tokens[3];
    FILE *file = fopen("users.csv", "r");
    if(file == NULL){
        printf("Could not open file for reading-update\n");
        return;
    }

    //taking the line and updating the fields
    char lines[MAX_USER_DATABASE][1024];
    int line_counter=0;

    char line[1024];

    //read each line from file

   
}

void types(){
    
     printf("name\nemail\nbirthday\nage\njob\ncity\npronouns\npreferred\navailability\n");

}
void delete(char* tokens[2]){
    //TODO


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
