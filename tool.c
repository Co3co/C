#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <base64.h> // stole mine from https://nachtimwald.com/2017/11/18/base64-encode-and-decode-in-c/

int random_int(int min, int max);
char random_hexadecimal();
int base64(int argc, char **argv);
int help_check(int argc, char **argv, char *help);
void print_help(char cmd[]);

int random_int(int min, int max) {
    return min + rand() % (max + 1 - min);
}

char random_hexadecimal() {
    char hexList[] = {
                '0', '1', '2', '3', '4', '5',
                '6', '7', '8', '9', 'A', 'B',
                'C', 'D', 'E', 'F' };

    return hexList[rand() % 16];
}

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
    int output_bool = 0;

    while ((opt = getopt(argc, argv, options)) != -1) {
        switch (opt) {
            case 'i':
                memcpy(data_in, optarg, sizeof(optarg));
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
                output_bool = 1;
                break;
            case 'h':
                return 1;
            default:
                break;
        }
    }

    if (strcmp(codec, "e") == 0) {
        data_out = b64_encode((const unsigned char *)data_in, strlen(data_in));
        if (output_bool == 0) {
            printf("%s", data_out);
        } else {
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

        if (output_bool == 0) {
            printf("%s", data_out);
        } else {
            FILE *outfile = fopen(output, "w");
            fprintf(outfile, "%s", data_out);
            fclose(outfile);
        }
    }
    free(data_in);
    free(data_out);
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
}

void print_help(char cmd[]) {
    if (strcmp(cmd, "tool") == 0) {
        puts("tool commands:");
        puts("    rand-int");
        puts("    rand-hex");
        puts("    b64");
    else if (strcmp(cmd, "rand-int") == 0)
        puts("tool rand-int <length> [-m min] [-M max]");
    else if (strcmp(cmd, "rand-hex") == 0)
        puts("tool rand-hex <length> [-m min_length] [-M max_length]");
    else if (strcmp(cmd, "b64") == 0)
        puts("tool b64 [e|d] [-p] [-i string] [-f infile] [-o outfile]");
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
    if (strcmp(action, "rand-int") == 0) {
        if (help_check(argc, argv, action) == 1)
            return 1;
        if (argc < 2) {
            print_help(action);
            return 1;
        }

        char *options = "m:M:";
        char opt;
        int min, max, l, len;

        min = 0;
        max = 9;
        len = (argc > 2) ? atoi(argv[2]) : 1;
        while ((opt = getopt(argc, argv, options)) != -1) {
            switch (opt) {
                case 'm':
                    min = atoi(optarg);
                    break;
                case 'M':
                    max = atoi(optarg);
                    break;
            }
        }

        srand(getpid());
        for (l = 0; l < len; l++) {
            printf("%d", random_int(min, max));
            fflush(stdout);
        }
    }
    else if (strcmp(action, "rand-hex") == 0) {
        if (help_check(argc, argv, action) == 1)
            return 1;
        char opt;
        char *options = "m:M:";
        int min, max, mm, c, len;

        mm = (argc > 2) ? atoi(argv[2]) : 16;
        while ((opt = getopt(argc, argv, options)) != -1) {
            switch (opt) {
                case 'm':
                    min = atoi(optarg);
                    mm = -1;
                    break;
                case 'M':
                    max = atoi(optarg);
                    mm = -1;
                    break;
            }
        }

        srand(getpid());
        if (mm == -1)
            len = min + rand() % (max+1 - min);
        else
            len = mm;
        for (c = 0; c < len; c++) {
            printf("%c", random_hexadecimal());
            fflush(stdout);
        }
    }
    else if (strcmp(action, "b64") == 0) {
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
