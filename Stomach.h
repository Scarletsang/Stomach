#ifndef STOMACH_H
#define STOMACH_H

#include <stdint.h>

#define KB  * (1024)
#define MB  * (1048576)
#define GB  * (uint64_t)(1073741824)

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

struct Stomach_Slice
{
  union
  {
    void*       data;
    char*       string;
  };
  Stomach_u64   length;
};
typedef struct Stomach_Slice Stomach_String;

struct Stomach_Parse_Node;
struct Stomach_Parser_Output
{
  struct Stomach_Parse_Node*  data;
  Stomach_u64                 length;
};

Stomach_b32 Stomach_Slice_is_valid(struct Stomach_Slice slice);

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

struct Stomach_Lexer
{
  Stomach_Array             array_input_string;
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

extern struct Stomach_Lexer_Output  Stomach_Lexer(Stomach_String input);
extern struct Stomach_Parser_Output Stomach_Parser(struct Stomach_Parser* parser, struct Stomach_Lexer* lexer);

void  Stomach_init(struct Stomach* stomach);
void  Stomach_reset(struct Stomach* stomach);
void  Stomach_Lexer_set_input_file(struct Stomach_Lexer* lexer, int fd);
void  Stomach_Lexer_set_input_string(struct Stomach_Lexer* lexer, struct Stomach_String input_string);
struct Stomach_Token  Stomach_lex(struct Stomach_Lexer* lexer);
void                  Stomach_lex_revert(struct Stomach_Lexer* lexer, struct Stomach_Token token);

#endif
