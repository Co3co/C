#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <base64.h> // stole mine from https://nachtimwald.com/2017/11/18/base64-encode-and-decode-in-c/

int base64(int argc, char **argv);
int help_check(int argc, char **argv, char *help);
void print_help(char cmd[]);

int base64(int argc, char **argv) {
    char *data_in = malloc(1048576);
    char *data_out = malloc(1048576);
    size_t data_out_len;
    char *input;
    char *output;
    char *codec = argv[2];
    char *options = "i:pf:o:h";
    char opt;
    char buf;
    output = NULL;

    while ((opt = getopt(argc, argv, options)) != -1) {
        switch (opt) {
            case 'i':
                memcpy(data_in, optarg, strlen(optarg));
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
        }
        else {
            FILE *outfile = fopen(output, "w");
            fprintf(outfile, "%s", data_out);
            fclose(outfile);
        }
    } else if (strcmp(codec, "d") == 0) {
        data_out_len = b64_decoded_size(data_in);
        data_out = malloc(data_out_len);

        if (!b64_decode(data_in, (unsigned char *)data_out, data_out_len)) {
            printf("Decoding failed.");
            return 1;
        }
        data_out[data_out_len] = '\0';

        if (output == NULL) {
            printf("%s\n", data_out);
            fflush(stdout);
        }
        else {
            FILE *outfile = fopen(output, "w");
            fprintf(outfile, "%s", data_out);
            fclose(outfile);
        }
    }
    free(data_in);
    free(data_out);
    return 0;
}

int help_check(int argc, char **argv, char *help) {
    char opt;
    opt = getopt(argc, argv, "h");
    switch (opt) {
        case 'h':
            print_help(help);
            return 1;
            break;
        default:
            break;
    }
    return 0;
}

void print_help(char cmd[]) {
    if (strcmp(cmd, "tool") == 0) {
        puts("tool commands:");
        puts("    b64");
    }
    else if (strcmp(cmd, "b64") == 0)
        puts("tool b64 e|d [-p] [-i string] [-f infile] [-o outfile]");
    else if (strcmp(cmd, "") == 0)
        puts("[]");
    else if (strcmp(cmd, "") == 0)
        puts("[]");
    else {
    }
    return;
        puts("[]");
}

int main(int argc, char **argv) {
    if (argc == 1) {
        print_help("tool");
        return 1;
    }
    if (help_check(1, argv, "tool") == 1)
        return 1;

    char action[32];
    strcpy(action, argv[1]);
    if (strcmp(action, "b64") == 0) {
        if (argc < 4) {
            print_help(action);
            return 1;
        }
        if (base64(argc, argv) == 1) {
            print_help(action);
            return 1;
        }
    }
    else {
        print_help("tool");
        return 1;
    }

    return 0;
}
