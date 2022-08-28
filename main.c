# include <stdlib.h>
# include <unistd.h>
# include <sys/wait.h>
# include <signal.h>
# include <string.h>

int check_arg(char *str)
{
    if (str == NULL)
        return (1);
    else if (*str == '~')
        return (2);

}

size_t  ee_strlen(const char *str)
{
    size_t  len = 0;
    
    while (str[len])
        ++len;
    return (len);
}

unsigned int    count_char(char *str, char c)
{
    unsigned int    count = 0;

    while (*str)
        if (*(str++) == c)
            ++count;
    return (count);
}

int ee_cd(char **str, int count)
{
    if (count > 1 || count_char(str[0], ' ') > 1)
    {
        write(2, "error: cd: bad arguments\n", ee_strlen("error: cd: bad arguments"));
        exit(0);
    }
    return (0);
}

void    skip_space(int argc, char **argv)
{
    size_t  count;

    while (argc--)
    {
        while (argv[0][0] == ' ')
            ++(argv[0]);
        count = 0;
        while (argv[0][count])
            ++count;
        if (count > 0)
            --count;
        while (count && argv[0][count] == ' ')
            argv[0][count--] = 0;
        ++argv;
    }
}

int main(int argc, char **argv, char **env)
{
    int pipe1;
    int pipe2;
    --argc;
    ++argv;
    skip_space(argc, argv);
    while (argc > 0)
    {
        pipe1 = 0;
        while (!(strcmp("|", argv[pipe1]) ^ strcmp(";", argv[pipe1])) && pipe1 < argc)
            ++pipe1;
        if (argc != pipe1)
        {
            pipe2 = pipe1 + 1;
            while (!(strcmp("|", argv[pipe2]) ^ strcmp(";", argv[pipe2])) && pipe2 < argc)
                ++pipe2;
        }
        else
            pipe2 = 0;
        if (strncmp("cd", *argv, 2) == 0)
        {
            ee_cd(argv, pipe1);
            argv += pipe1 + 1;
            argc -= pipe1 - 1;
        }
        else
        {
        }
    }
    return (0);
}
