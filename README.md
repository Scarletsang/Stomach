# Stomach

A Simple library that provides a simple setup for a recursive descent parser. Now, you should be able to write a parser that can digest any string that you received. The library is aimed to be transparent of what it is doing but having a simple design with little abstraction. The memory layout of the whole parser is the definition of the `struct Stomach`.

## Usage

```c
#include "Stomach.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

struct Stomach_Lexer_Output  My_Lexer(struct Stomach_String input)
{
    // user-defined lexer
}

struct Stomach_Parser_Output My_Parser(struct Stomach_Parser* parser, struct Stomach_Lexer* lexer)
{
    struct Stomach_Lexer_Output output = Stomach_lex(lexer, &My_Lexer); // If there is no enough string, lexer will try to read more bytes from the file
    struct Stomach_Token token = output.data;
    // user-defined parser
}

int main(int argc, char**argv)
{
    if (argc != 2)
        return 1;
    int fd = open(argv[1], O_RDONLY, 0644);
    if (fd == -1)
        return 1;
    struct  Stomach* stomach  = mmap(NULL,
                                      sizeof(struct Stomach),
                                      PROT_READ | PROT_WRITE,
                                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    Stomach_init(stomach);
    Stomach_Lexer_set_input_file(&stomach->lexer, fd);
    struct Stomach_Parser_Output output = Stomach_Parser(&stomach->parser, &stomach->lexer);
    if (Stomach_Slice_is_valid((struct Stomach_Slice) output))
    {
        Stomach_Parse_Tree_Node* parse_tree_root = output.data;
        // do something with the parse tree
    }
    close(fd);
    munmap(stomach, sizeof(struct Stomach));
    return 0;
}

```

