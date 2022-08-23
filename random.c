#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int random_gen(char settings[], unsigned long int length, FILE *outfile);
int help_check(int argc, char **argv);
void print_help();

int random_gen(char settings[], unsigned long int length, FILE *outfile) {
    const char *number_table = "0123456789";
    const char *lowercase_table = "abcdefghijklmnopqrstuvwxyz";
    const char *uppercase_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *symbol_table = "!@#$%^&*()-=_+[]{}\\|;:'\",.<>/?`~";

    char type_array[6][2048];
    unsigned char random_type;
    char random_char;
    char number_table_mm[10];
    char setting;
    int types;
    int all, hex, num;
    int lower, upper, symbol;
    int s, c, m;
    int min, max;
    int acc;

    types = 0;
    s = 0;
    acc = 0;
    all = 0, hex = 0, num = 0;
    lower = 0, upper = 0, symbol = 0;
    min = 0, max = 9;

    srand(getpid());

    while ((setting = settings[s++]) != '\0') {
        switch (setting) {
            case 'm':
                min = settings[s] - '0';
                break;
            case 'M':
                max = settings[s] - '0';
                break;
        }
    }
    if (min > max)
        return 1;
    s = 0;

    while ((setting = settings[s++]) != '\0') {
        switch(setting) {
            case 'a':
                if (all == 1)
                    break;
                all = 1;
                for (m = min; m <= max; m++)
                    number_table_mm[strlen(number_table_mm)] = number_table[m];
                strcpy(type_array[types++], number_table_mm);
                strcpy(type_array[types++], lowercase_table);
                strcpy(type_array[types++], uppercase_table);
                strcpy(type_array[types++], symbol_table);
                break;
            case 'H':
                if (all == 1 || hex == 1)
                    break;
                hex = 1;
                for (m = min; m <= max; m++)
                    number_table_mm[strlen(number_table_mm)] = number_table[m];
                strcpy(type_array[types++], number_table_mm);
                strcpy(type_array[types++], uppercase_table);
                break;
            case 'n':
                if (all == 1 || hex == 1 || num == 1)
                    break;
                num = 1;
                for (m = min; m <= max; m++)
                    number_table_mm[strlen(number_table_mm)] = number_table[m];
                strcpy(type_array[types++], number_table_mm);
                break;
            case 'l':
                if (all == 1 || lower == 1)
                    break;
                lower = 1;
                strcpy(type_array[types++], lowercase_table);
                break;
            case 'u':
                if (all == 1 || hex == 1 || upper == 1)
                    break;
                upper = 1;
                strcpy(type_array[types++], uppercase_table);
                break;
            case 's':
                if (all == 1 || symbol == 1)
                    break;
                symbol = 1;
                strcpy(type_array[types++], symbol_table);
                break;
            case 'A':
                acc = 1;
                break;
            case 'h':
                print_help("random");
                return 1;
                break;
        }
    }

    for (c = 0; c < length; c++) {
        random_type = rand() % types;
        random_char = type_array[random_type][rand() % strlen(type_array[random_type])-1];
        if (outfile == NULL) {
            printf("%c", random_char);
            if (acc == 1)
                fflush(stdout);
        } else
            fprintf(outfile, "%c", random_char);
    }
    printf("\n");
    fflush(stdout);
    return 0;
}

int help_check(int argc, char **argv) {
    char opt;
    opt = getopt(argc, argv, "h");
    switch (opt) {
        case 'h':
            print_help();
            return 1;
            break;
        default:
            break;
    }
    return 0;
}

void print_help() {
    puts("random <length> [-nlusaHA] [-m min_length] [-M max_length]");
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
    puts("          output each character when possible (only works when outputting to stdout)");
    puts("  -o  outfile");
    puts("          file to output the string to (overwrites the file)");
}

int main(int argc, char **argv) {
    if (argc == 1) {
        print_help();
        return 1;
    }
    if (help_check(1, argv) == 1)
        return 1;

    if (argc < 3) {
        print_help();
        return 1;
    }
    FILE *outfile = NULL;
    char *options = "aHnlushAo:m:M:";
    char settings[256];
    char opt;
    int success;
    int length, len;
    int min, max;

    length = 0;
    len = 0;
    min = 0, max = 9;
    strcpy(settings, "");

    while ((opt = getopt(argc, argv, options)) != -1) {
        switch(opt) {
            case 'a':
                len = strlen(settings);
                settings[len] = 'a';
                break;
            case 'H':
                len = strlen(settings);
                settings[len] = 'H';
                break;
            case 'n':
                len = strlen(settings);
                settings[len] = 'n';
                break;
            case 'l':
                len = strlen(settings);
                settings[len] = 'l';
                break;
            case 'u':
                len = strlen(settings);
                settings[len] = 'u';
                break;
            case 's':
                len = strlen(settings);
                settings[len] = 's';
                break;
            case 'A':
                len = strlen(settings);
                settings[len] = 'A';
                break;
            case 'm':
                min = optarg[0];
                len = strlen(settings);
                settings[len] = 'm';
                settings[len+1] = min;
                break;
            case 'M':
                max = optarg[0];
                len = strlen(settings);
                settings[len] = 'M';
                settings[len+1] = max;
                break;
            case 'o':
                outfile = fopen(optarg, "w");
                break;
            case 'h':
                print_help();
                return 1;
                break;
        }
    }

    if (optind < argc) {
        while (optind < argc) {
            if (atoi(argv[optind]) > 0)
                length = atol(argv[optind++]);
            else {
                print_help();
                return 1;
            }
        }
    } else {
        print_help();
        return 1;
    }

    success = random_gen(settings, length, outfile);
    if (success == 1) {
        print_help();
        return 1;
    }
    return 0;
}
