#include <stdio.h>
#include <string.h>
#include <ctype.h>
/*
TODO:

MVP
● Single data type
● CRUD operations
● no persistence
● basically just an array
● Command line UI

-- Error handling
-- Implement CRUD operations
-- Implement file operations
● Design your file format!
● Implement a module responsible for writing to the file
● Add persistence 

*/



//phone service provider

#define MAX_USER_DATABASE 100
#define NUMBER_NOT_FOUND "This phone number doesn't belong to any user"
#define NUMBER_FOUND "Number is found"
#define MAX_DATA 100

void add();
void users();
void search (char* tokens[], int tokens_length);
int select(char* phone_numb);
int select_by_name(char* name);
char *trim(char *str);
void update(char* tokens[4]);
void types();
void delete(char* tokens[2]);
void help();
void addhelp();
int num_users = 0;

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

//initializing the database ( array of users)
User userDatabase[MAX_USER_DATABASE];


void types() {
    printf("name\nemail\nbirthday\nage\njob\ncity\npronouns\npreferred\navailability\n");
}


int main() {
    // to improve have login system so certan commands can only be used by the user of that phone number
    // networking system
    printf("Welcome to the reservation system!\n");
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
    return 0;

}

void addhelp(){
    printf("Enter user information in the following format:\n\n");
    printf("name,phone,birthday,email,age,job,city,pronouns,preferred(communication method),availability\n\n");
    printf("To skip information, enter two consecutive commas\n");
    printf("Example: Alex,1234567890,01/01/2000,,20,Engineer,New York,he/him,phone,weekdays\n\n");

    printf("If you want to enter lastname enter it in this format: Alex-Smith\n");
}

void add(char* user_info) {
    if (num_users >= MAX_USER_DATABASE) {
        printf("User database is full.\n");
        return;
    }

 
    User new_user={0};
    char* token;
    char* rest= user_info;


    token = strsep(&rest, ",");
    if (token != NULL) {
        strncpy(new_user.name, token, sizeof(new_user.name) - 1);
        new_user.name[sizeof(new_user.name) - 1] = '\0';
    }

    token = strsep(&rest, ",");
    if (token != NULL) {
        strncpy(new_user.phone, token, sizeof(new_user.phone) - 1);
        new_user.phone[sizeof(new_user.phone) - 1] = '\0';
        char* trimmed_phone = trim(new_user.phone);
        strncpy(new_user.phone, trimmed_phone, sizeof(new_user.phone) - 1);  //added
    }
    

    token = strsep(&rest, ",");
    if (token != NULL) {
        strncpy(new_user.birthday, token, sizeof(new_user.birthday) - 1);
        new_user.birthday[sizeof(new_user.birthday) - 1] = '\0';
    }

    token = strsep(&rest, ",");
    if (token != NULL) {
        strncpy(new_user.email, token, sizeof(new_user.email) - 1);
        new_user.email[sizeof(new_user.email) - 1] = '\0';
    }

    token = strsep(&rest, ",");
    if (token != NULL) {
        strncpy(new_user.age, token, sizeof(new_user.age) - 1);
        new_user.age[sizeof(new_user.age) - 1] = '\0';
    }

    token = strsep(&rest, ",");
    if (token != NULL) {
        strncpy(new_user.job, token, sizeof(new_user.job) - 1);
        new_user.job[sizeof(new_user.job) - 1] = '\0';
    }
    token = strsep(&rest, ",");
    if (token != NULL) {
        strncpy(new_user.city, token, sizeof(new_user.city) - 1);
        new_user.city[sizeof(new_user.city) - 1] = '\0';
    }

    token = strsep(&rest, ",");
    if (token != NULL) {
        strncpy(new_user.pronouns, token, sizeof(new_user.pronouns) - 1);
        new_user.pronouns[sizeof(new_user.pronouns) - 1] = '\0';
    }

    token = strsep(&rest, ",");
    if (token != NULL) {
        strncpy(new_user.preferred, token, sizeof(new_user.preferred) - 1);
        new_user.preferred[sizeof(new_user.preferred) - 1] = '\0';
    }

    token = strsep(&rest, ",");
    if (token != NULL) {
        strncpy(new_user.availability, token, sizeof(new_user.availability) - 1);
        new_user.availability[sizeof(new_user.availability) - 1] = '\0';
    }

    userDatabase[num_users++] = new_user;
}

void users() {
    for (int i = 0; i < num_users; i++) {
        printf("%s\n", userDatabase[i].name);
    }
}

int select( char* phone_numb){
   
    for (int i = 0; i < num_users; i++) {
        //printf("Comparing: %s and %s\n", userDatabase[i].phone, phone_numb);
        //phone_numb = trim(phone_numb);
        if(strcmp(userDatabase[i].phone, phone_numb) == 0){
            return i;
}
    }
    return -1; // -1 if not found
}

int select_by_name( char* name){
    //alternative select function to select by name
    for (int i = 0; i < num_users; i++) {
        if (strcmp(userDatabase[i].name, name) == 0) {
            return i;
        }
    }
    return -1; // -1 if not found

}

void search( char* tokens[], int tokens_length){
    if(tokens_length != 2){
        printf("Invalid number of arguments\n");
        printf("Usage: search <phone number>\n");
        return;
    }

    char* phone_number= tokens[1];
   
    int index = select(phone_number);
    if(index == -1){
       // printf("Searching for phone number: %s\n", phone_number);
        printf("%s\n", NUMBER_NOT_FOUND);
        return;
    
}
    printf("%s\n", NUMBER_FOUND);
    printf("Name: %s\n", userDatabase[index].name);
    printf("Phone: %s\n", userDatabase[index].phone);
    printf("Birthday: %s\n", userDatabase[index].birthday);
    printf("Email: %s\n", userDatabase[index].email);
    printf("Age: %s\n", userDatabase[index].age);
    printf("Job: %s\n", userDatabase[index].job);
    printf("City: %s\n", userDatabase[index].city);
    printf("Pronouns: %s\n", userDatabase[index].pronouns);
    printf("Preferred communication method: %s\n", userDatabase[index].preferred);
    printf("Availability: %s\n", userDatabase[index].availability);
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

void update (char* tokens[4]){
    if(tokens[1]== NULL || tokens[2] == NULL || tokens[3] == NULL){
        printf("Invalid number of arguments\n");
        printf("Usage: update <phone number> <type> <change>\n");
        return;
    }

    char* phone_number = tokens[1];
    //trim(phone_number);
    char* type = tokens[2];
    char* change = tokens[3];

    int index = select(phone_number);
    if(index == -1){
        printf("%s\n", NUMBER_NOT_FOUND);
        return;
    }

    if(strcmp(type, "name") == 0){
        strcpy(userDatabase[index].name, change);
    } else if(strcmp(type, "email") == 0){
        strcpy(userDatabase[index].email, change);
    } else if(strcmp(type, "birthday") == 0){
        strcpy(userDatabase[index].birthday, change);
    } else if (strcmp(type, "age") == 0){
        strcpy(userDatabase[index].age, change);
    } else if (strcmp(type, "job") == 0){
        strcpy(userDatabase[index].job, change);
    } else if (strcmp(type, "city") == 0){
        strcpy(userDatabase[index].city, change);
    } else if (strcmp(type, "pronouns") == 0){
        strcpy(userDatabase[index].pronouns, change);
    } else if (strcmp(type, "preferred") == 0){
        strcpy(userDatabase[index].preferred, change);
    } else if (strcmp(type, "availability") == 0){
        strcpy(userDatabase[index].availability, change);
    } else {
        printf("Invalid type\n");
   
        return;
    }
}

void delete(char* tokens[2]){
    if(tokens[1] == NULL){
        printf("Invalid number of arguments\n");
        printf("Usage: delete <phone number>\n");
        return;
    }

    char* phone_number = tokens[1];
    int index = select(phone_number);
    if(index == -1){
        printf("%s\n", NUMBER_NOT_FOUND);
        return;
    }
    for(int i = index; i < MAX_USER_DATABASE - 1; i++){
        userDatabase[i] = userDatabase[i + 1];
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