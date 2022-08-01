#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <base64.h> // stole mine from https://nachtimwald.com/2017/11/18/base64-encode-and-decode-in-c/

int random_gen(int argc, char **argv);
int base64(int argc, char **argv);
int help_check(int argc, char **argv, char *help);
void print_help(char cmd[]);

int random_gen(int argc, char **argv) {
    const char *number_table = "0123456789";
    const char *lowercase_table = "abcdefghijklmnopqrstuvwxyz";
    const char *uppercase_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *symbol_table = "!@#$%^&*()-=_+[]{}\\|;:'\",.<>/?`~";

    FILE *outfile;
    char table[256];
    char random_char;
    char *options = "aHnlusohA:m:M:";
    char *output;
    char opt;
    size_t size;
    unsigned long int length;
    int len;
    int all, hex, num;
    int lower, upper, symbol;
    int acc;
    int c, i;
    int min, max;

    output = NULL;
    size = 0;
    length = 0;
    all = 0, hex = 0, num = 0;
    lower = 0, upper = 0, symbol = 0;
    min = 0, max = 9;

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
    optind = 1;

    strcpy(table, "");
    while ((opt = getopt(argc, argv, options)) != -1) {
        switch(opt) {
            case 'a':
                if (all == 1)
                    break;
                all = 1;
                for (i = min; i <= max; i++) {
                    len = strlen(table);
                    table[len] = number_table[i];
                    table[len+1] = '\0';
                    size = size + sizeof(number_table[i]);
                }
                strcat(table, lowercase_table);
                size = size + strlen(lowercase_table);
                strcat(table, uppercase_table);
                size = size + strlen(uppercase_table);
                strcat(table, symbol_table);
                size = size + strlen(symbol_table);
                break;
            case 'H':
                if (all == 1 || hex == 1)
                    break;
                hex = 1;
                for (i = min; i <= max; i++) {
                    len = strlen(table);
                    table[len] = number_table[i];
                    table[len+1] = '\0';
                    size = size + sizeof(number_table[i]);
                }
                strcat(table, uppercase_table);
                size = size + strlen(uppercase_table);
                break;
            case 'n':
                if (all == 1 || hex == 1 || num == 1)
                    break;
                num = 1;
                for (i = min; i <= max; i++) {
                    len = strlen(table);
                    table[len] = number_table[i];
                    table[len+1] = '\0';
                    size = size + sizeof(number_table[i]);
                }
                break;
            case 'l':
                if (all == 1 || lower == 1)
                    break;
                lower = 1;
                strcat(table, lowercase_table);
                size = size + strlen(lowercase_table);
                break;
            case 'u':
                if (all == 1 || hex == 1 || upper == 1)
                    break;
                upper = 1;
                strcat(table, uppercase_table);
                size = size + strlen(uppercase_table);
                break;
            case 's':
                if (all == 1 || symbol == 1)
                    break;
                symbol = 1;
                strcat(table, symbol_table);
                size = size + strlen(symbol_table);
                break;
            case 'A':
                acc = 1;
                break;
            case 'o':
                output = optarg;
                outfile = fopen(output, "w");
                break;
            case 'h':
                print_help("random");
                return 1;
                break;
        }
    }

    if (optind < argc) {
        while (optind < argc) {
            if (atoi(argv[optind]) > 0)
                length = atol(argv[optind++]);
            else if (strcmp(argv[optind++], "random") != 0) {
                print_help("random");
                return 1;
            }
        }
    } else {
        print_help("random");
        return 1;
    }

    for (c = 0; c < length; c++) {
        random_char = table[rand() % size];
        if (output == NULL)
            printf("%c", random_char);
        else {
            fprintf(outfile, "%c", random_char);
        }
        if (acc == 1)
            fflush(stdout);
    }
    if (output != NULL)
        fclose(outfile);
    return 0;
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
        if (output == NULL)
            printf("%s", data_out);
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

        if (output == NULL)
            printf("%s", data_out);
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
        puts("    random");
        puts("    b64");
    }
    else if (strcmp(cmd, "random") == 0) {
        puts("tool random <length> [-nlusaHA] [-m min_length] [-M max_length]");
        puts("  -n  numbers");
        puts("          enable numbers");
        puts("  -l  lowercase");
        puts("          enable lowercase letters");
        puts("  -u  uppercase");
        puts("          enable uppercase letters");
        puts("  -s  symbols");
        puts("          enable symbols");
        puts("  -a  all");
        puts("          enable all");
        puts("  -H  hexadecimal");
        puts("          enable hexadecimal characters");
        puts("  -A  accurate");
        puts("          use `fflush(stdout)`");
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
    if (strcmp(action, "random") == 0) {
        if (argc < 4) {
            print_help(action);
            return 1;
        }
        int err;

        srand(getpid());

        err = random_gen(argc, argv);
        if (err == 1)
            return 1;
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
