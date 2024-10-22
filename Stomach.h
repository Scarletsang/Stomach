#ifndef STOMACH_H
#define STOMACH_H

#include <stdint.h>

#define KB  * (1024)
#define MB  * (1048576)
#define GB  * (uint64_t)(1073741824)

#ifndef STOMACH_LEXER_READ_SIZE
#define STOMACH_LEXER_READ_SIZE (1 KB)
#endif

#ifndef STOMACH_TEMPORARY_SIZE
#define STOMACH_TEMPORARY_SIZE  (1 KB)
#endif

#ifndef STOMACH_INPUT_STRING_SIZE
#define STOMACH_INPUT_STRING_SIZE (4 KB)
#endif

#ifndef STOMACH_TOKEN_STACK_SIZE
#define STOMACH_TOKEN_STACK_SIZE (512)
#endif

#define STOMACH_TOKEN_STACK_AMOUNT (STOMACH_TOKEN_STACK_SIZE / sizeof(struct Stomach_Token))

#ifndef STOMACH_PARSE_TREE_SIZE
#define STOMACH_PARSE_TREE_SIZE (128 KB)
#endif

#define STOMACH_SIZE  (sizeof(struct Stomach))

#ifndef NULL
#define NULL (0)
#endif

#define TRUE  1
#define FALSE 0

typedef int32_t   Stomach_b32;
typedef int64_t   Stomach_i64;
typedef uint64_t  Stomach_u64;
typedef void*     Stomach_Arena_Snapshot;
struct Stomach_Arena
{
  void*       fill_pointer;
  void*       data;
  Stomach_u64 capacity; // in bytes
};

void* Stomach_Arena_fill(struct Stomach_Arena* arena, Stomach_u64 size); // will align the item
void  Stomach_Arena_split(struct Stomach_Arena* src, struct Stomach_Arena* dest, Stomach_u64 size);
Stomach_Arena_Snapshot  Stomach_Arena_snapshot(struct Stomach_Arena* arena);
void  Stomach_Arena_rollback(struct Stomach_Arena* arena, Stomach_Arena_Snapshot snapshot);
void  Stomach_Arena_clear(struct Stomach_Arena* arena);

typedef struct Stomach_Arena  Stomach_Array;

void*         Stomach_Array_push(Stomach_Array* array, Stomach_u64 item_size); // should not need to align the item
Stomach_u64   Stomach_Array_size(Stomach_Array* array, Stomach_u64 item_size);
Stomach_u64   Stomach_Array_avaliable(Stomach_Array* array);

struct Stomach_Parse_Node;
struct Stomach_Slice
{
  union
  {
    void*                       data;
    char*                       string;
    struct Stomach_Parse_Node*  parse_node;
  };
  Stomach_u64   length;
};
typedef struct Stomach_Slice Stomach_String;
typedef struct Stomach_Slice Stomach_Parser_Output;

Stomach_b32 Stomach_Slice_is_valid(struct Stomach_Slice slice);
char        Stomach_String_peek(Stomach_String string);
void        Stomach_String_consume(Stomach_String* string, Stomach_u64 amount);

struct Stomach_Token
{
  int             type;
  Stomach_String  content;
};

struct Stomach_Lexer_Output
{
  struct Stomach_Token  data;
  Stomach_u64           length;
  Stomach_b32           trigger_read;
};

enum Stomach_Lexer_State
{
  kLexerOk = 0,
  kLexerReadError,
  kLexerEof
};

struct Stomach_Lexer
{
  Stomach_Array             input_string;
  char                      read_string[STOMACH_LEXER_READ_SIZE];
  Stomach_u64               read_string_leftover_size;
  enum Stomach_Lexer_State  state;
  union
  {
    struct
    {
      struct Stomach_Token* token_stack_end; // minus one then it is the top of the stack
      struct Stomach_Token* token_stack_bottom;
    };
    Stomach_Array           token_stack; // data is Stomach_Token*
  };
  int                       fd;
  Stomach_String            input;
};

struct Stomach_Parse_Tree_Node
{
  int             type;
  Stomach_String  content;

  struct Stomach_Parse_Tree_Node* parent;
  struct Stomach_Parse_Tree_Node* next;
  struct Stomach_Parse_Tree_Node* prev;
  struct Stomach_Parse_Tree_Node* first;
  struct Stomach_Parse_Tree_Node* last;
};

struct Stomach_Parser
{
  struct Stomach_Arena            arena_parse_tree;
  struct Stomach_Parse_Tree_Node* root;
};

// This is the memory layout of the whole parser
struct Stomach
{
  struct Stomach_Arena  arena_temporary;
  struct Stomach_Lexer  lexer;
  struct Stomach_Parser parser;

  uint8_t               memory_temporary[STOMACH_TEMPORARY_SIZE];
  char                  input_string[STOMACH_INPUT_STRING_SIZE];
  struct Stomach_Token  token_stack[STOMACH_TOKEN_STACK_AMOUNT];
  uint8_t               memory_parse_tree[STOMACH_PARSE_TREE_SIZE];
};

typedef struct Stomach_Lexer_Output  (*Stomach_Lexer_Func)(Stomach_String input, Stomach_b32 is_eof);
typedef Stomach_Parser_Output        (*Stomach_Parser_Func)(struct Stomach_Parser* parser, struct Stomach_Lexer* lexer);

void  Stomach_init(struct Stomach* stomach);
void  Stomach_reset(struct Stomach* stomach);
void  Stomach_Lexer_set_input_file(struct Stomach_Lexer* lexer, int fd);
void  Stomach_Lexer_push_input_string(struct Stomach_Lexer* lexer, Stomach_String input_string);
struct Stomach_Token  Stomach_lex(struct Stomach_Lexer* lexer, Stomach_Lexer_Func func);
void                  Stomach_lex_revert(struct Stomach_Lexer* lexer, struct Stomach_Token token);

#endif
