#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATA_SIZE 1024
#define TOKEN_LINES 8
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
    "aaa",   "aad",    "aam",   "aas",    "adc",   "add",    "and",    "arpl",
    "bound", "bsf",    "bsr",   "bswap",  "bt",    "btc",    "btr",    "bts",
    "call",  "cbw",    "clc",   "cld",    "cli",   "clts",   "cmc",    "cmp",
    "cmpsb", "cmpsw",  "cmpsd", "cmpsq",  "cwd",   "cwde",   "cdq",    "cdqe",
    "daa",   "das",    "dec",   "div",    "enter", "esc",    "hlt",    "idiv",
    "imul",  "in",     "inc",   "insb",   "insw",  "insd",   "insq",   "int",
    "into",  "iret",   "ja",    "jae",    "jb",    "jbe",    "jc",     "jcxz",
    "je",    "jg",     "jge",   "jl",     "jle",   "jmp",    "jna",    "jnae",
    "jnb",   "jnbe",   "jnc",   "jne",    "jng",   "jnge",   "jnl",    "jnle",
    "jno",   "jnp",    "jns",   "jnz",    "jo",    "jp",     "jpe",    "jpo",
    "js",    "jz",     "lahf",  "lds",    "lea",   "les",    "lock",   "lodsb",
    "lodsw", "lodsd",  "lodsq", "loop",   "loope", "loopne", "loopnz", "loopz",
    "mov",   "movsb",  "movsw", "movsd",  "movsq", "movsx",  "movzx",  "mul",
    "neg",   "nop",    "not",   "or",     "out",   "outsb",  "outsw",  "outsd",
    "outsq", "pop",    "popa",  "popf",   "push",  "pusha",  "pushf",  "rcl",
    "rcr",   "rep",    "repe",  "repne",  "repnz", "repz",   "ret",    "rol",
    "ror",   "sal",    "sar",   "sbb",    "scasb", "scasw",  "scasd",  "scasq",
    "seta",  "setae",  "setb",  "setbe",  "setc",  "sete",   "setg",   "setge",
    "setl",  "setle",  "setna", "setnae", "setnb", "setnbe", "setnc",  "setne",
    "setng", "setnge", "setnl", "setnle", "setno", "setnp",  "setns",  "setnz",
    "seto",  "setp",   "setpe", "setpo",  "sets",  "setz",   "shl",    "shr",
    "shld",  "shrd",   "sidt",  "sgdt",   "shldt", "sldt",   "smsw",   "stc",
    "std",   "sti",    "stosb", "stosw",  "stosd", "stosq",  "sub",    "test",
    "ud2",   "xchg",   "xlat",  "xor",    "xadd",  "xlatb",  "[bits",  "dd",
    "db",    "dq"};

struct TokenLine {
    char instruction[TOKEN_LENGTH];
    char comment[TOKEN_LENGTH];
};

void strncpy_trim(char *dest, const char *src, const unsigned int n) {
    assert(src != NULL);
    assert(strnlen(src, 2) != 0);
    assert(strlen(src) <= n);

    int j = 0;
    for (int i = 0; src[i] != 0 || i < n - 1; ++i) {
        if (src[i] == '\n' || src[i] == '\0' || src[i] == '\r')
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
    assert(strnlen(str, 2) != 0);

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
}

int get_word_len(char *str) {
    int word_len = 0;
    while (str[word_len] != ' ' && str[word_len] != 0 &&
           str[word_len] != '\n' && str[word_len] != '\r') {
        word_len++;
    }
    return ++word_len;
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

        int word_len = get_word_len(tokens[i].instruction);
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
}

int main(int argc, char *argv[]) {
    char buffer[MAX_DATA_SIZE];

    struct TokenLine tokens[TOKEN_LINES] = {0};
    int line_count = 0;
    bool oversized = false;

    while (fgets(buffer, MAX_DATA_SIZE, stdin) != NULL) {

        // an oversized module is usually the long comment at the bottom
        if (line_count >= TOKEN_LINES) {
            oversized = true;
            for (int i = 0; i < line_count; i++) {
                printf("; %s\n", tokens[i].comment);
            }
            line_count = 0;
        }

        if (oversized) {
            strncpy_trim(tokens[0].comment, buffer, TOKEN_LENGTH);
            printf("%s\n", tokens[0].comment);
            continue;
        }

        if (strnlen(buffer, 3) <= 2/*  ||
            line_count >= 1 &&
                tokens[line_count - 1].instruction[get_word_len(
                    tokens[line_count - 1].instruction) + 1] == '\0' */) {

            module_print(tokens, line_count);
            printf("\n");

            line_count = 0;
            continue;
        }

        tokenline_add(&tokens[line_count], buffer);
        line_count++;
    }
    module_print(tokens, line_count);

    return EXIT_SUCCESS;
}
