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

#define CONFIG_GLOBAL_PATH "/etc/tamidsh"          //????
#define CONFIG_XDG_PATH "$XDG_CONFIG_HOME/tamidsh" //????
#define CONFIG_HOME_PATH "$HOME/tamidsh"           //????
#define MAX_INPUT 1000
#define STDOUT_FILENO 1
#define DEFAULT_PROMPT "> "

#include <fcntl.h>
#include <ctype.h>

#define ERR_PIPE 1
#define ERR_FORK 2

#define BUFSIZE 1024
/*
Note: ManPages: shell manpage: man_shell.1 - written here
shell config manpage: configfile.5 - written through commands
*/
void process_child(int p_read, int p_write);
void process_parent(int p_read, int p_write);
void execute_pipe(char *command1, char *command2);
FILE *logfile;
struct Config
{
    bool interactive;
    bool login;
    // I will add more to this section later
};

struct command
{
    const char **argv;
};

int run_repl(struct Config *config)
{
    char input[MAX_INPUT];
    char *command;
    char *args[256];
    while (1)
    {
        printf("> ");
        if (fgets(input, MAX_INPUT, stdin) == NULL)
        {
            break;
        }
        input[strcspn(input, "\n")] = 0; // to remove the newline
        command = strtok(input, " ");
        int i = 0;
        while (command != NULL)
        {
            args[i] = command;
            command = strtok(NULL, " ");
            i++;
        }
        args[i] = NULL;

        if (strcmp(args[0], "exit") == 0)
        {
            break;
        }

        else if (strcmp(args[0], "cd") == 0)
        {
            if (args[1] == NULL)
            {
                fprintf(stderr, "cd: missing argument\n");
            }
            else
            {
                if (chdir(args[1]) != 0)
                {
                    perror("cd");
                }
                else
                {
                    if (logfile != NULL)
                    {
                        fprintf(logfile, "cd command executed successfully\n");
                    }
                }
            }
        }
        else if (strcmp(args[0], "showlogs") == 0)
        {
            FILE *file = fopen("tamid_sh.log", "r");
            if (file != NULL)
            {
                char line[256];
                while (fgets(line, sizeof(line), file))
                {
                    printf("%s", line);
                }
                fclose(file);
                if (logfile != NULL)
                {
                    fprintf(logfile, "showlogs command executed successfully\n");
                }
            }
            else
            {
                fprintf(stderr, "Error opening log file\n");
            }
        }
        /*
                else
                {
                    int pid = fork();
                    if (pid == 0)
                    {
                        execvp(args[0], args);
                        perror("exec");
                        exit(EXIT_FAILURE);
                    }
                    else if (pid < 0)
                    {
                        perror("fork");
                    }
                    else
                    {
                        int status;
                        wait(&status);
                    }
                }*/
        else
        {
            // finish
            //  fork and exec
            int pipefd[2];
            pipe(pipefd);
            int pid = fork();
            if (pid == 0)
            {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                execvp(args[0], args);
                perror("exec");
                syslog(LOG_ERR, "exec error: %s", strerror(errno));
                if (logfile != NULL)
                {
                    fprintf(logfile, "exec error: %s", strerror(errno));
                }

                exit(EXIT_FAILURE);
            }
            else if (pid < 0)
            {
                perror("fork");
                syslog(LOG_ERR, "fork error: %s", strerror(errno));
                if (logfile != NULL)
                {
                    fprintf(logfile, "fork error: %s", strerror(errno));
                }
            }
            else
            {

                syslog(LOG_INFO, "Command executed successfully %s", " ");

                // close(pipefd[1]);
                char buffer[4096];
                ssize_t bytesRead;
                bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1);
                if (bytesRead == -1)
                {
                    perror("read");
                    return 1;
                }
                buffer[bytesRead] = '\0';
                printf("%s\n", buffer);
                syslog(LOG_INFO, "Command executed successfully %s", " ");

                if (logfile != NULL)
                {
                    fprintf(logfile, "Command executed successfully %s", " ");
                }

                int status;
                wait(&status);
                close(pipefd[1]);
            }
        }

        printf("> ");
        return 1;
    }
}

void execute_command(char *cmd)
{
    char *args[BUFSIZE];
    char *token;
    int i = 0;

    token = strtok(cmd, " ");
    while (token != NULL)
    {
        args[i] = token;
        i++;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    if (logfile != NULL)
    {
        fprintf(logfile, "[%s] Command executed: %s\n", timestamp, cmd);
    }
    if (execvp(args[0], args) == -1)
    {
        if (logfile != NULL)
        {
            fprintf(stderr, "Execvp error: %s\n", strerror(errno));
            fprintf(logfile, "Execvp error: %s\n", strerror(errno));
        }

        exit(EXIT_FAILURE);
    }
}
void read_config_file(char *filename, struct Config *config)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error opening the config file");
        return;
    }
    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == '#' || line[0] == '\n')
        {
            continue;
        }
        // printf("%s", line);
        char key[256], value[256];
        // key,value syntax
        if (sscanf(line, "%255[^=]=%255s", key, value) != 2)
        {
            fprintf(stderr, "Syntax error in config file: %s", line);
            continue;
        }

        // updating config
        if (strcmp(key, "interactive") == 0)
        {
            config->interactive = strcmp(value, "true") == 0;
        }
        else if (strcmp(key, "login") == 0)
        {
            config->login = strcmp(value, "true") == 0;
        }
    }

    // fgets(prompt, BUFSIZE, file);
    fclose(file);
}
void create_config_file1(char *prompt)

// gets a custom prompt from the config file
{
    FILE *file = fopen("config1.txt", "r");
    if (file == NULL)
    {
        perror("Error creating the config file 1");
        if (logfile != NULL)
        {
            fprintf(logfile, "Error creating the config file");
        }
        return;
    }

    if (fgets(prompt, BUFSIZE, file) == NULL)
    {
        fprintf(stderr, "Error reading the config file");
        if (logfile != NULL)
        {
            fprintf(logfile, "Error reading the config file");
        }
    }
    if (file != NULL)
    {
        if (logfile != NULL)
        {
            fprintf(logfile, "Config file for custom prompt is found\n");
        }

    }
    // fprintf(file, "interactive=true\n");
    ;
    // fprintf(file, "prompt=[custom prompt here]\n");

    fclose(file);

    return;
}

void create_config_file2()
{
    FILE *file = fopen("man_shell.1", "w");
    if (file == NULL)
    {
        perror("Error creating the man file");
        if (logfile != NULL)
        {
            fprintf(logfile, "Error creating the man file");
        }

        return;
    }
    fprintf(file, ".TH BASIC SHELL MAN PAGE\n");
    fprintf(file, ".SH NAME\n");
    fprintf(file, "basic_shell , a simple shell\n");
    fprintf(file, ".SH SYNOPSIS\n");
    // add
    fprintf(file, ".B basic_shell\n");
    // fprintf(file, "[\fB\-n\fR \fIBITS\fR]\n");
    // fprintf(file, "[\fB\-\-bits\fR \fIBITS\fR]\n");

    fprintf(file, ".SH DESCRIPTION\n");
    fprintf(file, "is a simple shell that can execute commands and pipe them together.\n");
    fprintf(file, ".SH OPTIONS\n");
    fprintf(file, ".TP\n");
    // fprintf(file, ".BR \-n ", " \-\-bits =\fIBITS\fR\n");
    fprintf(file, "Make a custom prompt.\n");
    fprintf(file, "Default is > \n");

    fclose(file);
}
void create_config_file3()
{
    // sets environment variables
    char line[256];
    FILE *file = fopen("config3.txt", "a");
    if (file == NULL)
    {
        perror("Error creating the config file");
        if (logfile != NULL)
        {
            fprintf(logfile, "Error creating the config file");
        }
        return;
    }
    while (fgets(line, sizeof(line), file))
    {
        if (line[0] == '#' || line[0] == '\n')
        {
            continue;
        }
        printf("%s", line);
    }

    fclose(file);
}

int main()
{

    const char *ls[] = {"ls", "-l", 0};
    const char *awk[] = {"awk", "{print $1}", 0};
    const char *sort[] = {"sort", 0};
    const char *uniq[] = {"uniq", 0};

    struct command cmd[] = {{ls}, {awk}, {sort}, {uniq}};
    char prompt[BUFSIZE] = DEFAULT_PROMPT;
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

    // openlog("tamidsh", LOG_PID | LOG_CONS, LOG_USER);
    // syslog(LOG_INFO, "tamidsh started");

    create_config_file1(prompt);
    struct Config config;
    read_config_file("config.txt", &config);
    create_config_file2();
    create_config_file3();
    int pipe_fd1[2];
    int pipe_fd2[2];
    if (pipe(pipe_fd1) == -1)
    {
        perror("pipe");
        exit(ERR_PIPE);
    }
    if (pipe(pipe_fd2) == -1)
    {
        perror("pipe");
        exit(ERR_PIPE);
    }

    int p_read1 = pipe_fd1[0];
    int p_write1 = pipe_fd1[1];

    int p_read2 = pipe_fd2[0];
    int p_write2 = pipe_fd2[1];

    int pid = fork();
    if (pid == 0)
    {
        process_child(p_read1, p_write2);
        exit(0);
    }

    else
    {
        fprintf(stderr, "Error creating pipe\n");
        syslog(LOG_ERR, "An error occurred: %s", strerror(errno));
        if (logfile != NULL)
        {
            fprintf(logfile, "Error creating pipe");
        }

        exit(ERR_FORK);
    }
    char command1[] = "ls";
    char command2[] = "wc -l";
    execute_pipe(command1, command2);
    syslog(LOG_INFO, "Command executed successfully %s", " ");

    char command[BUFSIZE];
    printf("%s", prompt);
    while (fgets(command, sizeof(command), stdin) != NULL)
    {

        command[strcspn(command, "\n")] = 0;
        // char args3[] = {command, NULL};
        execute_command(command);

        printf("%s", prompt);
    }

    fclose(logfile);
    return run_repl(&config);
}
void execute_pipe(char *cmd1, char *cmd2)
{
    int fd[2];
    pid_t pid;
    if (pipe(fd) == -1)
    {
        perror("pipe");
        if (logfile != NULL)
        {
            fprintf(logfile, "Error creating pipe");
        }

        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid == -1)
    {
        perror("fork");
        if (logfile != NULL)
        {
            fprintf(logfile, "Error creating fork");
        }

        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    { // child
        close(fd[0]);
        if (dup2(fd[1], STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(fd[1]);
        execute_command(cmd1);
        if (logfile != NULL)
        {
            fprintf(logfile, "Command executed successfully %s", cmd1);
        }
        exit(EXIT_SUCCESS);
    }
    else
    { // parent
        close(fd[1]);
        if (dup2(fd[0], STDIN_FILENO) == -1)
        {
            perror("dup2");
            if (logfile != NULL)
            {
                fprintf(logfile, "Error creating pipe");
            }

            exit(EXIT_FAILURE);
        }
        close(fd[0]);
        execute_command(cmd2);
        wait(NULL);
    }
}
void process_child(int p_read, int p_write)
{
    char buf[BUFSIZE];
    int n = read(p_read, buf, BUFSIZE - 1);
    buf[n] = 0;
    char *p = buf;
    while (*p)
    {
        *p = toupper(*p);
        p++;
    }
    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    write(p_write, buf, n);
    if (logfile != NULL)
    {
        fprintf(logfile, "[%s] Command executed: %s\n", timestamp, buf);
    }
}

int parse_config(struct Config *config, char *path)
{
    FILE *file = fopen("~/.tamidshrc", "r");
    if (file != NULL)
    {
        char line[256];
        while (fgets(line, sizeof(line), file))
        {
            line[strcspn(line, "\n")] = 0;
            if (strncmp(line, "interactive=", 13) == 0)
            {
                config->interactive = strcmp(line + 13, "true") == 0;
            }
            else if (strncmp(line, "login=", 6) == 0)
            {
                config->login = strcmp(line + 6, "true") == 0;
            }
        }
        fclose(file);
    }
    else
    {
        return -1;
    }
}

int parse_args(struct Config *config, int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--interactive") == 0)
        {
            config->interactive = 1;
        }
        else if (strcmp(argv[i], "--login") == 0)
        {
            config->login = 1;
        }
    }
    return 1;
}

// 2nd part
void change_directory(char *path)
{
    if (chdir(path) != 0)
    {
        perror("chdir");
    }
}
