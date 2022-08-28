#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <base64.h> // stole mine from https://nachtimwald.com/2017/11/18/base64-encode-and-decode-in-c/

int base64(int argc, char **argv);
int help_check(int argc, char **argv);
void print_help();

int base64(int argc, char **argv) {
    size_t data_out_len;
    char *data_in = malloc(1048576);
    char *data_out = malloc(1048576);
    char *input;
    char *output = NULL;
    char *codec = argv[1];
    char *options = "i:pf:o:h";
    char opt;
    char buf;

    while ((opt = getopt(argc, argv, options)) != -1) {
        switch (opt) {
            case 'i':
                strcat(data_in, optarg);
                data_in[strlen(data_in)] = ' ';
                while (optind < argc) {
                    strcat(data_in, argv[optind++]);
                    data_in[strlen(data_in)] = ' ';
                }
                data_in[strlen(data_in)-1] = '\0';
                break;
            case 'p':
                while ((buf = getchar()) != '\n')
                    strcat(data_in, &buf);
                break;
            case 'f':
                input = optarg;
                FILE *infile = fopen(input, "r");
                while ((buf = getc(infile)) != EOF)
                    strcat(data_in, &buf);
                fclose(infile);
                break;
            case 'o':
                output = optarg;
                break;
            case 'h':
                return 1;
            default:
                break;
        }
    }

    if (strcmp(codec, "e") == 0) {
        data_out = b64_encode((const unsigned char *)data_in, strlen(data_in));
        if (output == NULL) {
            printf("%s\n", data_out);
            fflush(stdout);
        } else {
            FILE *outfile = fopen(output, "w");
            fprintf(outfile, "%s", data_out);
            fclose(outfile);
        }
    } else if (strcmp(codec, "d") == 0) {
        data_out_len = b64_decoded_size(data_in);
        data_out = malloc(data_out_len);

        if (!b64_decode(data_in, (unsigned char *)data_out, data_out_len)) {
            puts("decoding failed: invalid base64 string");
            return -1;
        }
        data_out[data_out_len] = '\0';

        if (output == NULL) {
            printf("%s\n", data_out);
            fflush(stdout);
        } else {
            FILE *outfile = fopen(output, "w");
            fprintf(outfile, "%s", data_out);
            fclose(outfile);
        }
    }

    free(data_in);
    free(data_out);
    return 0;
}

int help_check(int argc, char **argv) {
    char opt;
    opt = getopt(argc, argv, "h");
    switch (opt) {
        case 'h':
            return 1;
            break;
        default:
            break;
    }
    return 0;
}

void print_help() {
    puts("b64 e|d [-p] [-i string] [-f infile] [-o outfile]");
    puts("  -i  input (default)");
    puts("          encode/decode the argument string (reads until the end of the command)");
    puts("  -p  prompt");
    puts("          prompts you for the string");
    puts("  -f  file");
    puts("          file to read the input string from");
    puts("  -o  outfile");
    puts("          file to write the output to");
    exit(1);
}

int main(int argc, char **argv) {
    if (argc < 3 || help_check(1, argv) == 1 || base64(argc, argv) == 1)
        print_help();
    return 0;
}
