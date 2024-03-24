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


int num_users = 0;

typedef struct{
    char name[MAX_DATA];
    char phone[MAX_DATA];
    char email[MAX_DATA];
    char birthday[MAX_DATA];
} User;

//initializing the database ( array of users)
User userDatabase[MAX_USER_DATABASE];



int main() {
    // to improve have login system so certan commands can only be used by the user of that phone number
    // networking system
    printf("Welcome to the reservation system!\n");
    char *cmds = 
    "\nUsage:\n"
    "  execute <command> [options]\n\n"
    "Commands:\n"
    "  add                                   Add a new user.\n"
    "  search <phone number>                 Search their name by entering the phone number.\n"
    "  update <phone number> <type> <change> Update name email or birthday by phone number.\n"
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
            add();
        } else if (strcmp(cmd, "users") == 0) {
            users();
        }else if(strcmp(cmd, "search")== 0){
            search(tokens, tokens_length);
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


void add() {
    if (num_users >= MAX_USER_DATABASE) {
        printf("User database is full.\n");
        return;
    }

    User new_user;

    printf("Enter full name (string: Any): ");
    fgets(new_user.name, sizeof(new_user.name), stdin);

    printf("Enter phone number (string: xxx-xxx-xxxx): ");
    fgets(new_user.phone, sizeof(new_user.phone), stdin);
    trim(new_user.phone);

    printf("Enter birthday yyyy-mm-dd (string: yyyy-mm-dd): ");
    fgets(new_user.birthday, sizeof(new_user.birthday), stdin);

    printf("Enter email address (string: Any): ");
    fgets(new_user.email, sizeof(new_user.email), stdin);


    userDatabase[num_users++] = new_user;
}

void users() {
    for (int i = 0; i < num_users; i++) {
        printf("%s\n", userDatabase[i].name);
    }
}

int select( char* phone_numb){
    if(userDatabase[0].phone != NULL){
        printf("%s\n", userDatabase[0].phone);
    }
  

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
}

char *trim(char *str) {
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}