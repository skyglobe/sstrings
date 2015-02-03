/****************************************************************************
Copyright (c) 2015 Gianfranco Gallizia

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFLEN 4096

enum offsetformat {
    NONE,
    OCTAL,
    DECIMAL,
    HEXADECIMAL
};

static size_t n_chars = 4; /*Minimum number of printable characters in a string*/
static size_t saved_chars = 0;
static enum offsetformat oft = NONE; /* -t argument flag*/
static char out_buffer[BUFLEN] = {'\0'};
int print_header = 1;

static void printletters(unsigned int offset)
{
    switch(oft)
    {
        case OCTAL:
            printf("%o %s", offset, out_buffer);
            break;
        case DECIMAL:
            printf("%u %s", offset, out_buffer);
            break;
        case HEXADECIMAL:
            printf("%x %s", offset, out_buffer);
            break;
        case NONE:
            printf("%s", out_buffer);
            break;
    }

}

/*Read the file content and search for strings*/
static void searchletters(void)
{
    char buffer[BUFLEN];
    ssize_t i; /*Counter*/
    ssize_t read_val;
    off_t base = (off_t)0;

    while((read_val = read(STDIN_FILENO, buffer, BUFLEN)) > 0)
    {
        for (i = 0; i < read_val; i++)
        {
            if(isprint(buffer[i]))
            {
                if(saved_chars < n_chars)
                {
                    out_buffer[saved_chars] = buffer[i];
                    saved_chars++;
                }
                else
                {
                    if(print_header)
                    {
                        out_buffer[saved_chars] = buffer[i];
                        out_buffer[saved_chars + 1] = '\0';
                        printletters(base + i);
                        print_header = 0;
                    }
                    else
                    {
                        fputc(buffer[i], stdout);
                    }
                }
            }
            else
            {
                if(!print_header && saved_chars >= n_chars)
                    fputc('\n', stdout);

                saved_chars = 0;
                print_header = 1;
            }
        }
        base += read_val;
    }
}

int main(int argc, char *argv[])
{
    int ch; /*getopt return value*/

    /*Parse command line arguments*/
    while((ch = getopt(argc, argv, "an:t:")) != -1)
    {
        switch(ch)
        {
            case 'a':
                /*Ignore the -a flag*/
                break;
            case 'n':
                /*Parse the argument of -n and store it in n_chars*/
                {
                    unsigned long t = strtoul(optarg, NULL, 0);
                    if (t < 1 || t == ULONG_MAX)
                    {
                        fprintf(stderr, "Invalid argument for -n: '%s'\n", optarg);
                        exit(2);
                    }
                    n_chars = (size_t) t;
                }
                break;
            case 't':
                {
                    size_t length = strlen(optarg);
                    if (length != 1)
                    {
                        fprintf(stderr, "Invalid argument for -t: '%s'\n", optarg);
                        exit(3);
                    }

                    switch(optarg[0])
                    {
                        case 'o':
                            oft = OCTAL;
                            break;
                        case 'd':
                            oft = DECIMAL;
                            break;
                        case 'x':
                            oft = HEXADECIMAL;
                            break;
                        default:
                            fprintf(stderr, "Invalid argument for -t: '%s'\n", optarg);
                            exit(3);
                            break;
                    }
                }
                break;
            default:
                exit(1);
        }
    }

    /*Adjust argc and argv*/
    argc -= optind;
    argv += optind;

    /*Process input file(s)*/

    if(argc > 0)
    {
        int retval;
        while(argc)
        {
            /*Close stdin*/
            retval = close(STDIN_FILENO);
            if(retval) /*Unable to close the file*/
            {
                perror(NULL);
                exit(4);
            }

            errno = 0;
            retval = open(*argv, O_RDONLY);
            if(retval == -1)
            {
                perror(NULL);
                exit(5);
            }

            searchletters();

            argc--;
            argv++;
        }
    }
    else
    {
        /*Process stdin*/
        searchletters();
    }

    return 0;

}

