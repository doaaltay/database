#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define EXT_SUCCESS 0
#define EXT_ERR_TOO_MANY_ARGUMENTS 1
#define EXT_ERR_SOME_ERROR 2

#define CONFIG_GLOBAL_PATH "/etc/tamidshell"
#define CONFIG_XDG_PATH "$XDG_CONFIG_HOME/tamidshell"
#define CONFIG_HOME_PATH "$HOME/tamidshell"

struct Config
{
    bool interactive;
    bool login;
    // I will add more to this section later
};

void config_init(struct Config *config)
{
    config->interactive = false;
    config->login = false;
}

int parse_config(struct Config *config, char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {

        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        // TODO
        line[strcspn(line, "\n")] = 0;
        if (strncmp(line, "interactive=", 13) == 0) // HERE
        {
            config->interactive = strcmp(line + 13, "true") == 0;
        }

        else if (strncmp(line, "login=", 6) == 0)
        {
            config->login = strcmp(line + 6, "true") == 0;
        }
    }
    fclose(file);
    return 1;
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

int run(char *line)
{
    pid_t pid = fork();
    if (pid == 0)
    {
        execlp(line, line, NULL);
        exit(0);
    }
    else
    {
        wait(NULL);
    }
    return 0;
}

int run_repl(struct Config *config)
{
    char line[1024];
    while (config->interactive)
    {
        printf("> ");
        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            break;
        }

        line[strcspn(line, "\n")] = 0;
        pid_t pid = fork();
        if (pid == 0)
        {
            execlp("/bin/sh", "/bin/sh", "-c", line, NULL);
            exit(EXIT_FAILURE);
        }
        else if (pid < 0)
        {
            printf("Fork failed\n");
        }
        else
        {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            {
                printf("Program exited with status %d\n", WEXITSTATUS(status));
            }
        }
    }

    return 0;
}

int main(int argc, char **argv, char **envp)
{
    if (argc != 1)
    {
        fprintf(stderr, "USAGE: tamidsh");
        return EXT_ERR_TOO_MANY_ARGUMENTS;
    }
    struct Config config;
    config_init(&config);
    parse_config(&config, CONFIG_GLOBAL_PATH);
    parse_config(&config, CONFIG_XDG_PATH);
    parse_config(&config, CONFIG_HOME_PATH);
    parse_args(&config, argc, argv);

    // Repl logic
    if (run_repl(&config) == 0)
    {
        return EXT_ERR_SOME_ERROR;
    }

    return EXT_SUCCESS;
}