# Stomach

A Simple library that provides a simple setup for a recursive descent parser. Now, you should be able to write a parser that can digest any string that you received. The library is aimed to be transparent of what it is doing but having a simple design with little abstraction. The memory layout of the whole parser is the definition of the `struct Stomach`.

## Usage

```c
#include "Stomach.h"
#include <fcntl.h>
#include <unistd.h>

struct Stomach_Lexer_Output  My_Lexer(Stomach_String input)
{
  struct Stomach_Lexer_Output output = {0};
  // user-defined lexer
  return output;
}

Stomach_Parser_Output My_Parser(struct Stomach_Parser* parser, struct Stomach_Lexer* lexer)
{
  Stomach_Parser_Output output= {0};
  struct Stomach_Token token = Stomach_lex(lexer, &My_Lexer); // If there is no enough string, lexer will try to read more bytes from the file
  // user-defined parser
  return output;
}

int main(int argc, char**argv)
{
  static struct Stomach stomach;
  if (argc != 2)
    return 1;
  int fd = open(argv[1], O_RDONLY, 0644);
  if (fd == -1)
    return 1;
  Stomach_init(&stomach);
  Stomach_Lexer_set_input_file(&stomach.lexer, fd);
  Stomach_Parser_Output output = My_Parser(&stomach.parser, &stomach.lexer);
  if (Stomach_Slice_is_valid(output))
  {
    struct Stomach_Parse_Tree_Node* parse_tree_root = output.data;
    // do something with the parse tree
  }
  close(fd);
  munmap(stomach, sizeof(struct Stomach));
  return 0;
}
```

