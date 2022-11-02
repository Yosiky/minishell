#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

char    **g_env;

void    putstr(char *str)
{
    size_t  i = 0;

    while (str[i])
        ++i;
    write(STDERR, str, i);
}

void    fatal(void)
{
    putstr((char *)"error: fatal\n");
    exit(1);
}

void    builtin_execve(char **argv)
{
    int frk = fork();
    
    if (frk == -1)
        fatal();
    if (frk == 0)
    {
        if (execve(*argv, argv, g_env) == -1)
        {
            putstr((char *)"error: cannot execute ");
            putstr(*argv);
            putstr("\n");
            exit(-1);
        }
    }
    else
    {
        int status;
        
        waitpid(frk, &status, 0);
        if (status == -1)
            exit(0);
    }
}

void    builtin_cd(char **argv)
{
    if (*argv == NULL || argv[1] != NULL)
        putstr((char *)"error: cd: bad arguments\n");
    else if (chdir(*argv))
    {
        putstr((char *)"error: cd: cannot access to ");
        putstr(*argv);
        putstr((char *)"\n");
    }
}

void    microshell(char **argv, int stop)
{
    if (!strcmp(*argv, "cd"))
        builtin_cd(argv + 1);
    else
        builtin_execve(argv);
    if (stop)
        exit(0);
}

int do_pipe(void)
{
    int pipes[2];
    pipe(pipes);
    int frk = fork();

    if (frk == -1)
        fatal();
    if (frk == 0)
    {
        close(pipes[0]);
        close(STDOUT);
        dup(pipes[1]);
        close(pipes[1]);
    }
    else
    {
        close(pipes[1]);
        close(STDIN);
        dup(pipes[0]);
        close(pipes[0]);
    }
    return (frk);
}

char    **working(char **argv)
{
    char    **argv_start = argv;

    if (!strcmp(*argv, ";"))
        return (argv + 1);
    while (1)
    {
        if (*argv == NULL || !strcmp(*argv, ";") || !strcmp(*argv, "|"))
        {
           int  stop = 0;
           int  frk = 0;

           if (*argv == NULL || !strcmp(*argv, "|"))
               stop = 1;
           if (*argv != NULL && !strcmp(*argv, "|"))
               frk = do_pipe();
           *argv = NULL;
           if (frk == 0)
               microshell(argv_start, stop);
           else
               return (working(argv + 1));
           return (argv + 1);
        }
        ++argv;
    }
}

int main(int argc, char **argv, char **env)
{
    g_env = env;
    (void)argc;
    ++argv;
    while (*argv)
    {
        int backup[2] = {dup(STDIN), dup(STDOUT)};
        argv = working(argv);
        dup2(backup[0], STDIN);
        dup2(backup[1], STDOUT);
        close(backup[0]);
        close(backup[1]);
    }
    return (0);
}
