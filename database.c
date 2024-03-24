#include <stdio.h>
#include <string.h> 

int num_users = 0;
FILE *users_file;


int main() {

    users_file = fopen("users.csv", "r");
    if (users_file == NULL) {
        printf("Error opening file\n");
        return -1;
    }

    char line[1024];

    while (fgets(line, sizeof(line), users_file)) {
        
        char *token;
        token= strtok(line,","); //split line into tokens separated by comma 
        while(token != NULL){
            printf("%s\n", token);
            token = strtok(NULL, ",");
        }
        
        
        num_users++;
    }



    fclose(users_file);
    printf("Number of users: %d\n", num_users);
    return 0;
}