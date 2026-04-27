#include "parser.h"

int main() {
    const char *source =
        "var A = 2\n"
        "var B = 3\n"
        "var C = 4\n"
        "var D A + B";

    Parser parser;
    parser_init(&parser, source);
    parser_parse(&parser);

    return 0;
}