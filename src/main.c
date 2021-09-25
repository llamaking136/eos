#include <assert.h>
#include <eval.h>
#include <log.h>
#include <parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tokenizer.h>
#include <errno.h>
#include <sys/stat.h>

int is_dir(const char *cc)
{
	struct stat buffer;
	stat(cc, &buffer);
	return S_ISDIR(buffer.st_mode);
}

int file_exists(const char *cc)
{
	struct stat buffer;
	return stat(cc, &buffer) == 0;
}

void error_read(const char *name)
{
	if (is_dir(name)) 
	{
		log_error("Cannot read file '%s'! Error: Is a Directory", name);
	} 
	else if (!file_exists(name))
	{
		log_error("Cannot read file '%s'! Error: File not Found", name);
	}
	else
	{
		log_error("Cannot read file '%s'! Errno: [%d]", name, errno);
	}
}

void usage(FILE *stream, char *invocation)
{
    fprintf(stream, "Usage: %s [OPTIONS] <file>\n", invocation);
    fprintf(stream, "OPTIONS: \n");
    fprintf(stream, "	-h, --help		Show this message\n");
}

/* utility function to go through command line arguments. */
char *shift(int *argc, char ***argv)
{
    assert(*argc > 0);

    char *result = **argv;
    *argv += 1;
    *argc -= 1;

    return result;
}

char *read_file(char *name)
{
    char *source = NULL;
    FILE *fp = fopen(name, "r");
    if (fp != NULL)
    {
        if (fseek(fp, 0L, SEEK_END) == 0)
        {
            long bufsize = ftell(fp);
            if (bufsize == -1)
            {
				error_read(name);
            }

            source = malloc(sizeof(char) * (bufsize + 1));

            if (fseek(fp, 0L, SEEK_SET) != 0)
            { /* Error */
            }

            size_t newLen = fread(source, sizeof(char), bufsize, fp);
            if (ferror(fp) != 0)
            {
                /* fputs("Error reading file", stderr); */
				error_read(name);
			}
            else
            {
                source[newLen++] = '\0'; /* Just to be safe. */
            }
        }
        fclose(fp);
    } else {
		error_read(name);
	}

    return source;
}

int main(int argc, char **argv)
{
    char *program = shift(&argc, &argv);

    char *input_file = NULL;

    while (argc > 0)
    {
        char *argument = shift(&argc, &argv);

        if (strcmp(argument, "-h") == 0 || strcmp(argument, "--help") == 0)
        {
            usage(stdout, program);
        }

        else
        {
            input_file = argument;
        }
    }

    if (!input_file)
    {
        /* fprintf(stderr, "No file name provided. Exiting\n"); */ /* This is my personal opinion, change it if you'd like. */
		usage(stderr, program);
        return 1;
    }

    char *string = read_file(input_file);
	if (string == NULL)
	{
		return 1;
	}
    vec_token_t tokens = tokenize(string);

    ParseValueList tree = parse(tokens);

    eval(tree);

    free(string);

    return 0;
}
