#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <syslog.h>
#include <errno.h>
#include <time.h>
#define EXT_SUCCESS 0
#define EXT_ERR_TOO_MANY_ARGUMENTS 1
#define EXT_ERR_SOME_ERROR 2
#include <fcntl.h>
#include <ctype.h>

#define ERR_PIPE 1
#define ERR_FORK 2

#define BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
#define LSH_RL_BUFSIZE 1024

#define LOGIN_CONFIG "/etc/shell_login.conf" // or /etc/shell_login.conf?
#define max_login_info_size 100              // add to man config file
#define INTERACTIVE_CONFIG "/etc/shell_interactive.conf"
#define CUSTOMPROMPT_CONFIG "/etc/shell_customprompt.conf"
#define max_prompt_size 50
char COMMANDPROMPT[max_prompt_size] = "> ";
FILE *logfile;
char *read_line(void);

char **split_line(char *line);
int execute_command(char **args);
int launch(char **args);
void command_loop(void);
int cd(char **args);
int help_command(char **args);
int exit_command(char **args);
int get_builtins_number();
char *builtin_commands[];

int (*builtin_func[])(char **);
struct Config
{
  char username[max_login_info_size];
  char password[max_login_info_size];
  char customprompt[max_prompt_size];

  bool interactive;
  bool login;
  // I will add more to this section later
};
void prompt_config(struct Config *config);
void interactive_config(struct Config *config);

struct Config login_config()
{

  struct Config config;
  FILE *login_config = fopen(LOGIN_CONFIG, "r");
  if (login_config == NULL)
  {
    perror("Error opening the login config file");
    fprintf(logfile, "Error opening the login config file");
    exit(EXIT_FAILURE);
  }
  char line[BUFSIZE];
  while (fgets(line, sizeof(line), login_config))
  {
    if (line[0] == '#')
    {
      continue;
    }
  }
  // fscanf(login_config, "enter username:%s\n", config.username);
  // fscanf(login_config, "enter password:%s\n", config.password);

  if (strncmp(line, "enter username:", 15) == 0)
  {
    strncpy(config.username, line + 15, sizeof(config.username) - 1);
    config.username[sizeof(config.username) - 1] = '\0'; // Ensure null-termination
  }
  else if (strncmp(line, "enter password:", 15) == 0)
  {
    strncpy(config.password, line + 15, sizeof(config.password) - 1);
    config.password[sizeof(config.password) - 1] = '\0'; // Ensure null-termination
  }
  fclose(login_config);
  prompt_config(&config);
  interactive_config(&config);
  return config;
}
void prompt_config(struct Config *config)
{
  FILE *prompt_config = fopen(CUSTOMPROMPT_CONFIG, "r");
  if (prompt_config == NULL)
  {
    perror("Error opening the prompt config file");
    fprintf(logfile, "Error opening the prompt config file");
    exit(EXIT_FAILURE);
  }
  fscanf(prompt_config, "%s", COMMANDPROMPT);
  fclose(prompt_config);
}
void interactive_config(struct Config *config)
{
  FILE *interactive_config = fopen(INTERACTIVE_CONFIG, "r");
  if (interactive_config == NULL)
  {
    perror("Error opening the interactive config file");
    fprintf(logfile, "Error opening the interactive config file");
    exit(EXIT_FAILURE);
  }
  int interactive;
  fscanf(interactive_config, "%d", &interactive);
  config->interactive = interactive != 0;
  fclose(interactive_config);
}

int main(int argc, char **argv)
{
  char *home_dir = getenv("HOME");
  if (home_dir == NULL)
  {
    perror("Error getting the home directory");
    return 1;
  }
  char logfile_path[BUFSIZE];
  snprintf(logfile_path, sizeof(logfile_path), "%s/%s", home_dir, "tamid_sh.log");
  FILE *logfile = fopen(logfile_path, "a");
  if (logfile == NULL)
  {
    perror("Error opening the log file");
    return 1;
  }
  fprintf(logfile, ".LOG\n");

  // ADD CONFIG HERE
  struct Config config = login_config(); // other configs are called in login_config

  command_loop();

  return EXIT_SUCCESS;
}
int execute_command(char **args) // execute command
{
  int i;

  if (args[0] == NULL) // empty command --> then return 1
  {

    return 1;
  }

  for (i = 0; i < get_builtins_number(); i++) // looping over the commands,  execute the one that matches
  {
    if (strcmp(args[0], builtin_commands[i]) == 0)
    {
      return (*builtin_func[i])(args);
    }
  }

  // launch the command as a program if commands doesnt match
  return launch(args);
}
void command_loop(void)
{
  /**
   * main loop of shell
   */
  int status;
  char *line;
  char **args;

  while (status)
  {
    printf("%s", COMMANDPROMPT);    // add config file for a custom command
    line = read_line();             // read line
    args = split_line(line);        // split line into tokens
    status = execute_command(args); // execute command and get sstatus

    free(line); // free the memory for line
    free(args); // free the memory for args
  }             // while status is true keep loopins
}

char *read_line(void)
{
  /**
   * reads a line from the command line
   */
  char *line = NULL;   // pointer
  ssize_t bufsize = 0; // variable to hold size of input buffer

  if (getline(&line, &bufsize, stdin) == -1) // reading a line from the command line stdin
  {
    if (feof(stdin))
    {
      // if we reach the end exit
      exit(EXIT_SUCCESS);
    }
    else
    {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }

  return line;
}

char **split_line(char *line)
{
  /**
   * splits the line into tokens
   */
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char *)); // memory for tokens
  char *token;

  if (!tokens) // if memory allocation fails
  {
    fprintf(logfile, "Memory allocation error\n");
    fprintf(stderr, "Memory allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM); // get first token

  // loop the line and get tokens
  while (token != NULL)
  {
    tokens[position] = token;
    position++;

    if (position >= bufsize) // if we exceed the buffer size reallocate more memory
    {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char *));
      if (!tokens)
      {
        fprintf(stderr, "Memory allocation error\n");
        fprintf(logfile, "Memory allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

int launch(char **args)
{
  pid_t pid, wpid; // process id
  int status;

  // create child process
  pid = fork();
  if (pid == 0)
  {
    // Child process
    if (execvp(args[0], args) == -1)
    {
      perror("exec");
    }
    exit(EXIT_FAILURE);
  }
  else if (pid < 0) // fork failed if pid is <0
  {

    perror("fork");
  }
  else
  {
    // parent
    // wait child to change status
    do
    {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1; // prompt input again
}

char *builtin_commands[] = {
    "cd",
    "help",
    "exit"};

int exit_command(char **args);
int get_builtins_number()
{
  return sizeof(builtin_commands) / sizeof(char *);
}

int (*builtin_func[])(char **) = {
    &cd,
    &help_command,
    &exit_command};

int cd(char **args)
{
  /**
   * change the working directory
   */
  if (args[1] == NULL)
  {
    fprintf(stderr, "Please specify the directory");
  }
  else
  {
    if (chdir(args[1]) != 0) // change the directory to the requested one
    {
      fprintf(logfile, "cd failed");
      perror("cd failed");
    }
  }
  return 1;
}

int help_command(char **args)
{
  /**
   * help command
   */
  int i;

  printf("This is a simple shere program. Enter commands\n");
  printf("These are the supported commands:\n");

  for (i = 0; i < get_builtins_number(); i++)
  {
    printf("  %s\n", builtin_commands[i]);
  }

  return 1;
}

int exit_command(char **args)
{ // exit
  return 0;
}