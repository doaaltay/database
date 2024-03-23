#include <stdio.h>
#include <string.h>
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

    
       while (1) {
        printf("Enter a command: ");
        fgets(command, sizeof(command), stdin);

       
        command[strcspn(command, "\n")] = 0; // remove newline character

        if (strcmp(command, "add") == 0) {
            add();
        } else if (strcmp(command, "users") == 0) {
            users();
        } else if (strcmp(command, "exit") == 0) {
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



