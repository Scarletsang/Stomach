#include "Stomach.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>

enum Token_Type
{
  kPlus,
  kMinus,
  kTimes,
  kDivide,
  kLeftParentheses,
  kRightParentheses,
  kInteger,
  kFloat
};

// Tokens: '+', '-', '*', '/', '(', ')', INTEGER, FLOAT
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

Stomach_b32 is_white_space(char c)
{
  return ((c == ' ') || (c == '\t'));
}

Stomach_b32 is_digit(char c)
{
  return ((c >= '0') && (c <= '9'));
}
struct Stomach_Lexer_Output  my_lexer(Stomach_String input, Stomach_b32 is_eof)
{
  struct Stomach_Lexer_Output output = {0};
  // remove white space
  for (Stomach_u64 i = 0; (i < input.length) && is_white_space(input.string[i]); i++)
  {
    output.length++;
  }
  Stomach_String_consume(&input, output.length);
  char first_char = Stomach_String_peek(input);
  if (first_char == '+')
  {
    output.data.type = kPlus;
    output.data.content = (Stomach_String) {.string = input.string, .length = 1};
    output.length += 1;
  }
  else if (first_char == '-')
  {
    output.data.type = kMinus;
    output.data.content = (Stomach_String) {.string = input.string, .length = 1};
    output.length += 1;
  }
  else if (first_char == '*')
  {
    output.data.type = kTimes;
    output.data.content = (Stomach_String) {.string = input.string, .length = 1};
    output.length += 1;
  }
  else if (first_char == '/')
  {
    output.data.type = kDivide;
    output.data.content = (Stomach_String) {.string = input.string, .length = 1};
    output.length += 1;
  }
  else if (first_char == '(')
  {
    output.data.type = kLeftParentheses;
    output.data.content = (Stomach_String) {.string = input.string, .length = 1};
    output.length += 1;
  }
  else if (first_char == ')')
  {
    output.data.type = kRightParentheses;
    output.data.content = (Stomach_String) {.string = input.string, .length = 1};
    output.length += 1;
  }
  else
  {
    Stomach_u64 i = 0;
    for (; (i < input.length) && is_digit(input.string[i]); i++);
    int type = kInteger;
    if ((i != 0) && (input.string[i] == '.'))
    {
      i++;
      type = kFloat;
      for (; (i < input.length) && is_digit(input.string[i]); i++);
    }
    output.length += i;
    if ((i != 0) && (is_eof || (i < input.length)))
    {
      output.data.type = type;
      output.data.content = (Stomach_String) {.string = input.string, .length = i};
    }
    else
    {
      output.trigger_read = TRUE;
    }
  }
  return output;
}

Stomach_Parser_Output my_parser(struct Stomach_Parser* parser, struct Stomach_Lexer* lexer)
{
  Stomach_Parser_Output output= {0};
  (void) parser;
  struct Stomach_Token token = Stomach_lex(lexer, &my_lexer);
  while (Stomach_Slice_is_valid(token.content))
  {
    printf("type: %d [%.*s]\n", token.type, (int) token.content.length, token.content.string);
    token = Stomach_lex(lexer, &my_lexer);
  }
  return output;
}

int main(int argc, char**argv)
{
  static struct Stomach stomach;
  if (argc != 2)
    return 1;
  Stomach_init(&stomach);
  Stomach_Lexer_push_input_string(&stomach.lexer, (Stomach_String){.string = argv[1], .length = strlen(argv[1])});
  Stomach_Parser_Output output = my_parser(&stomach.parser, &stomach.lexer);
  if (Stomach_Slice_is_valid(output))
  {
    // struct Stomach_Parse_Tree_Node* parse_tree_root = output.data;
    // do something with the parse tree
  }
  return 0;
}

