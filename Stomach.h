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

#ifndef STOMACH_TOKENS_SIZE
#define STOMACH_TOKENS_SIZE (4 KB)
#endif

#ifndef STOMACH_TOKENS_AMOUNT
#define STOMACH_TOKENS_AMOUNT 256
#endif

#ifndef STOMACH_PARSE_TREE_SIZE
#define STOMACH_PARSE_TREE_SIZE (128 KB)
#endif

#define STOMACH_SIZE  (sizeof(struct Stomach))

#ifndef NULL
#define NULL (0)
#endif

typedef uint64_t  Stomach_u64;
typedef void*     Stomach_Arena_Snapshot;
struct Stomach_Arena
{
  void*       fill_pointer;
  Stomach_u64 capacity;
  void*       data;
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
  void*       data;
  Stomach_u64 length;
};

struct Stomach_String
{
  char*       data;
  Stomach_u64 length;
};

struct Stomach_Token;
struct Stomach_Lexer_Output
{
  struct Stomach_Token* data;
  Stomach_u64           length;
};

struct Stomach_Parse_Node;
struct Stomach_Parser_Output
{
  struct Stomach_Parse_Node*  data;
  Stomach_u64                 length;
};

int Stomach_Slice_is_valid(struct Stomach_Slice slice);

struct Stomach_Token
{
  int                   type;
  struct Stomach_String content;
};

struct Stomach_Lexer
{
  Stomach_Array         array_input_string;
  struct Stomach_Arena  arena_tokens;
  Stomach_Array         tokens; // data is Stomach_Token**
  int                   start_of_array; // to make the tokens array a circular one
  int                   fd;
  struct Stomach_String input;
};

struct Stomach_Parse_Tree_Node
{
  int                   type;
  struct Stomach_String content;

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

  uint8_t memory_temporary[STOMACH_TEMPORARY_SIZE];
  char input_string[STOMACH_INPUT_STRING_SIZE];
  union
  {
    struct Stomach_Token*  tokens[STOMACH_TOKENS_AMOUNT];
    uint8_t memory_tokens[STOMACH_TOKENS_SIZE];
  };
  uint8_t memory_parse_tree[STOMACH_PARSE_TREE_SIZE];
};

extern struct Stomach_Lexer_Output  Stomach_Lexer(struct Stomach_String input, struct Stomach_Arena* arena);
extern struct Stomach_Parser_Output Stomach_Parser(struct Stomach_Parser* parser, struct Stomach_Lexeri* lexer);

void  Stomach_init(struct Stomach* stomach);
void  Stomach_reset(struct Stomach* stomach);
void  Stomach_Lexer_set_input_file(struct Stomach_Lexer* lexer, int fd);
void  Stomach_Lexer_set_input_string(struct Stomach_Lexer* lexer, struct Stomach_String input_string);
int   Stomach_lex(struct Stomach_Lexer* lexer, uint32_t amount_of_token);

#endif
