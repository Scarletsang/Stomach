#include "Stomach.h"
#include <unistd.h>
#include <string.h>

// Tokens: '+', '-', '*', '/', '(', ')', '.', NUM
// expression
//     ::= term (('+' | '-' ) term)*
// term
//     ::= factor (('/' | '*' ) factor)*
//
// factor
//     : primary
//     | '-' factor
//     | '+' factor
//     ;
//
// primary
//     : IDENTIFIER
//     | INTEGER
//     | FLOATING_POINT_LITERAL
//     | '(' expression ')'
//     ;

struct Stomach_Lexer_Output  My_Lexer(Stomach_String input)
{
  struct Stomach_Lexer_Output output = {0};
  return output;
}

Stomach_Parser_Output My_Parser(struct Stomach_Parser* parser, struct Stomach_Lexer* lexer)
{
  Stomach_Parser_Output output= {0};
  struct Stomach_Token token = Stomach_lex(lexer, &My_Lexer);
  return output;
}

int main(int argc, char**argv)
{
  static struct Stomach stomach;
  if (argc != 2)
    return 1;
  Stomach_init(&stomach);
  Stomach_Lexer_push_input_string(&stomach.lexer, (Stomach_String) {argv[1], strlen(argv[1])});
  Stomach_Parser_Output output = My_Parser(&stomach.parser, &stomach.lexer);
  if (Stomach_Slice_is_valid(output))
  {
    struct Stomach_Parse_Tree_Node* parse_tree_root = output.data;
    // do something with the parse tree
  }
  return 0;
}

