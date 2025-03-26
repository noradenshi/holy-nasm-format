#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATA_SIZE 256
#define MAX_TOKEN_LENGTH 64
#define MAX_TOKEN_SIZE 1024

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

bool token_in_array(char *token, char **arr, int size) {
    for (int i = 0; i < size; i++) {
        if (strcmp(token, arr[i]) == 0)
            return true;
    }
    return false;
}

const char *commands[] = {
    "mov",   "add",   "sub",    "mul",   "div",    "inc",    "dec",
    "cmp",   "test",  "jmp",    "je",    "jne",    "jg",     "jge",
    "jl",    "jle",   "jz",     "jnz",   "jo",     "jno",    "js",
    "jns",   "call",  "ret",    "push",  "pop",    "lea",    "nop",
    "int",   "iret",  "lcall",  "lret",  "xchg",   "shl",    "shr",
    "sal",   "sar",   "rol",    "ror",   "clc",    "stc",    "cli",
    "sti",   "cld",   "std",    "wait",  "pause",  "fild",   "fist",
    "fistp", "fld",   "fst",    "fstp",  "fcomp",  "fadd",   "fsub",
    "fmul",  "fdiv",  "fchs",   "finit", "fsqrt",  "fptan",  "frndint",
    "fsubr", "faddp", "fmulp",  "fdivp", "fcompp", "fwait",  "fxch",
    "fsin",  "fcos",  "fscale", "fucom", "fucomp", "fisttp", "fucompp",
    "movsb", "movsw", "movsd",  "stosb", "stosw",  "stosd",  "lodsb",
    "lodsw", "lodsd", "scasb",  "scasw", "scasd",  "cmpsb",  "cmpsw",
    "cmpsd", "rep",   "repe",   "repne", "repnz",  "repz",   "lock",
    "[bits"};

int main(int argc, char *argv[]) {
    char buffer[MAX_DATA_SIZE];
    bool was_prev_line_empty = false;

    char tokens[10][MAX_TOKEN_LENGTH];
    int token_size = 0;
    int token_start = -1;
    int token_end = -1;

    while (fgets(buffer, MAX_DATA_SIZE, stdin) != NULL) {

        int token_size = 0;
        int token_start = -1;
        int token_end = -1;

        if (strlen(buffer) <= 1) {
            if (was_prev_line_empty)
                continue;

            was_prev_line_empty = true;
        } else {
            was_prev_line_empty = false;
        }

        const int buffer_len = strlen(buffer);
        for (int i = 0; i < buffer_len; i++) {
            switch (buffer[i]) {
            case ' ':
            case '\r':
            case '\n':
            case '\t':
            case ',':
                if (token_start == -1) {
                    continue;
                }

                token_end = i;
                strncpy(tokens[token_size++], buffer + token_start,
                        token_end - token_start);
                tokens[token_size - 1][token_end - token_start] = 0;
                token_start = -1;
                break;

            default:
                if (token_start == -1) {
                    token_start = i;
                }
            }
        }

        if (token_size == 0) {
            printf("\n");
            continue;
        }

        if (strncmp(tokens[0], ";", 1) == 0) {
            printf("%-10s", ";");
            continue;
        }

        if (token_in_array(tokens[0], (char **)commands, ARRAY_SIZE(commands)))
            printf("%10c", ' ');

        for (int i = 0; i < token_size; i++) {
            printf("%s ", tokens[i]);
        }
        printf("\n");
    }

    return EXIT_SUCCESS;
}
