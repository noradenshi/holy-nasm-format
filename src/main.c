#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATA_SIZE 1024
#define TOKEN_LINES 24
#define TOKEN_LENGTH 512

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))

bool token_in_array(const char *token, char **arr, const int size) {
    for (int i = 0; i < size; i++) {
        if (strcmp(token, arr[i]) == 0)
            return true;
    }
    return false;
}

const char *instructions[] = {
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
    "[bits", "dd",    "db"};

struct TokenLine {
    char instruction[TOKEN_LENGTH];
    char comment[TOKEN_LENGTH];
};

void strncpy_trim(char *dest, const char *src, const unsigned int n) {
    assert(src != NULL);
    assert(strlen(src) != 0);
    assert(strlen(src) <= n);
    // printf("len: %lu", strlen(src));

    int j = 0;
    for (int i = 0; src[i] != 0 || i < n - 1; ++i) {
        if (src[i] == '\n' || src[i] == '\0')
            break;

        if (src[i] != ' ' || j != 0 && src[i + 1] != ' ' && src[i + 1] != 0) {
            dest[j++] = src[i];
        }
    }
    dest[j] = 0;
}

void tokenline_add(struct TokenLine *tokenline, const char *line) {
    assert(strlen(line) != 0);

    char *str = strdup(line);
    assert(str != NULL);
    assert(strlen(str) != 0);

    char *split = strstr(str, ";");
    if (split != NULL) {
        split[0] = '\0';
        split++;
    }

    tokenline->instruction[0] = 0;
    tokenline->comment[0] = 0;

    if (str[0] != 0 && str != split) {
        strncpy_trim(tokenline->instruction, str, (split - str));
    }
    if (split != NULL) {
        assert(strlen(split) != 0);
        strncpy_trim(tokenline->comment, split, strlen(split));
    }

    free(str);
    str = NULL;
    split = NULL;

    // for (int i = 0; i < strlen(tokenline->instruction); i++) {
    //     if (tokenline->instruction[i] == '\n')
    //         tokenline->instruction[i] = 0;
    // }
    // for (int i = 0; i < strlen(tokenline->comment); i++) {
    //     if (tokenline->comment[i] == '\n')
    //         tokenline->comment[i] = 0;
    // }
}

void module_print(struct TokenLine *tokens, const unsigned int line_count) {
    if (line_count == 0)
        return;

    int alignment = 0;
    int line_align = 0;

    for (int i = 0; i < line_count; i++) {
        line_align = strlen(tokens[i].instruction);

        if (line_align > alignment)
            alignment = line_align;
    }

    for (int i = 0; i < line_count; i++) {
        if (tokens[i].instruction[0] == 0) {
            printf("%-9s%s\n", ";", tokens[i].comment);
            continue;
        }

        int word_len = 0;
        while (tokens[i].instruction[word_len] != ' ' &&
               tokens[i].instruction[word_len] != 0 &&
               tokens[i].instruction[word_len] != '\n') {
            word_len++;
        }
        word_len++;

        char word[word_len] = {};
        strncpy(word, tokens[i].instruction, word_len);
        word[word_len - 1] = 0;

        if (token_in_array(word, (char **)instructions,
                           ARRAY_SIZE(instructions)))
            printf("%9c%s", ' ', tokens[i].instruction);
        else {
            printf("%s", word);

            if (tokens[i].instruction[word_len - 1] != 0)
                printf("%*c%s", (9 - word_len), ' ',
                       tokens[i].instruction + word_len - 1);
        }

        if (tokens[i].comment[0] != 0)
            printf("  %*s %s",
                   alignment - (int)strlen(tokens[i].instruction) + 1, ";",
                   tokens[i].comment);
        printf("\n");
    }

    if (line_count > 0)
        printf("\n");
}

int main(int argc, char *argv[]) {
    char buffer[MAX_DATA_SIZE];

    struct TokenLine tokens[TOKEN_LINES] = {0};
    int line_count = 0;

    while (fgets(buffer, MAX_DATA_SIZE, stdin) != NULL) {

        // empty line = module finished
        // printf("buffer_len = %lu\n", strlen(buffer));
        if (strlen(buffer) <= 2) {
            module_print(tokens, line_count);
            line_count = 0;
            continue;
        }

        if (line_count >= TOKEN_LINES) {
            fprintf(stderr,
                    "Module exceeded maximum line capacity, stopping...\n");
            module_print(tokens, line_count);
            return EXIT_FAILURE;
        }

        tokenline_add(&tokens[line_count], buffer);
        line_count++;
        // printf("line_count = %d\n", line_count);
    }
    module_print(tokens, line_count);

    return EXIT_SUCCESS;
}
