#include "Stomach.h"
#include <string.h>
#include <unistd.h>
#include <assert.h>

void* Stomach_Arena_fill(struct Stomach_Arena* arena, Stomach_u64 size)
{
  uint8_t*  fill_pointer = arena->fill_pointer;
  uintptr_t alignment = 2 * sizeof(void*);
  uintptr_t modulo = (uintptr_t) fill_pointer & (alignment - 1);
  if (modulo != 0)
  {
    fill_pointer += alignment - modulo;
  }
  uint8_t*  fill_pointer_new = fill_pointer + size;
  if (fill_pointer_new > ((uint8_t*)arena->fill_pointer + arena->capacity))
  {
    assert("Arena overflow");
    fill_pointer = NULL;
  }
  else
  {
    arena->fill_pointer = fill_pointer_new;
  }
  return fill_pointer;
}

void  Stomach_Arena_split(struct Stomach_Arena* src, struct Stomach_Arena* dest, Stomach_u64 size)
{
  void* data = Stomach_Arena_fill(src, size);
  dest->data = data;
  dest->fill_pointer = data;
  dest->capacity = size;
}

Stomach_Arena_Snapshot  Stomach_Arena_snapshot(struct Stomach_Arena* arena)
{
  return arena->fill_pointer;
}

void  Stomach_Arena_rollback(struct Stomach_Arena* arena, Stomach_Arena_Snapshot snapshot)
{
  arena->fill_pointer = snapshot;
}

void  Stomach_Arena_clear(struct Stomach_Arena* arena)
{
  arena->fill_pointer = arena->data;
}

void* Stomach_Array_push(Stomach_Array* array, Stomach_u64 item_size)
{
  uint8_t*  fill_pointer = array->fill_pointer;
  uint8_t*  fill_pointer_new = fill_pointer + item_size;
  if (fill_pointer_new > ((uint8_t*)array->data + array->capacity))
  {
    assert("Arena overflow");
    fill_pointer = NULL;
  }
  else
  {
    array->fill_pointer = fill_pointer_new;
  }
  return fill_pointer;
}

Stomach_u64  Stomach_Array_size(Stomach_Array* array, Stomach_u64 item_size)
{
  return ((uint8_t*)array->fill_pointer - (uint8_t*)array->data) / item_size;
}

Stomach_u64   Stomach_Array_avaliable(Stomach_Array* array)
{
  return array->capacity - ((uint8_t*)array->fill_pointer - (uint8_t*)array->data);
}

Stomach_b32 Stomach_Slice_is_valid(struct Stomach_Slice slice)
{
  return (slice.data != NULL);
}

char  Stomach_String_peek(Stomach_String string)
{
  return string.string[0];
}

void  Stomach_String_consume(Stomach_String* string, Stomach_u64 amount)
{
  string->string += amount;
  string->length -= amount;
}

void  Stomach_init(struct Stomach* stomach)
{
  stomach->arena_temporary.data = stomach->memory_temporary;
  stomach->arena_temporary.fill_pointer = stomach->memory_temporary;
  stomach->arena_temporary.capacity = STOMACH_TEMPORARY_SIZE;
  stomach->lexer = (struct Stomach_Lexer) {0};
  stomach->lexer.input_string.data = stomach->input_string;
  stomach->lexer.input_string.fill_pointer = stomach->input_string;
  stomach->lexer.input_string.capacity = STOMACH_INPUT_STRING_SIZE;
  stomach->lexer.input.data = stomach->input_string;
  stomach->lexer.fd = -1;
  stomach->lexer.token_stack.data = stomach->token_stack;
  stomach->lexer.token_stack.fill_pointer = stomach->token_stack;
  stomach->lexer.token_stack.capacity = STOMACH_TOKEN_STACK_SIZE;
  stomach->lexer.state = kLexerEof;
  stomach->parser.arena_parse_tree.data = stomach->memory_parse_tree;
  stomach->parser.arena_parse_tree.fill_pointer = stomach->memory_parse_tree;
  stomach->parser.arena_parse_tree.capacity = STOMACH_PARSE_TREE_SIZE;
}

void  Stomach_reset(struct Stomach* stomach)
{
  Stomach_Arena_clear(&stomach->arena_temporary);
  Stomach_Arena_clear(&stomach->lexer.input_string);
  Stomach_Arena_clear(&stomach->lexer.token_stack);
  stomach->lexer.fd = -1;
  Stomach_Arena_clear(&stomach->parser.arena_parse_tree);
}

void  Stomach_Lexer_set_input_file(struct Stomach_Lexer* lexer, int fd)
{
  lexer->fd = fd;
  lexer->state = 0;
}

void  Stomach_Lexer_push_input_string(struct Stomach_Lexer* lexer, Stomach_String input_string)
{
  char* input_string_dest = Stomach_Array_push(&lexer->input_string, sizeof(char)* input_string.length);
  memcpy(input_string_dest, input_string.data, input_string.length); 
  lexer->input.length += input_string.length;
}

struct Stomach_Token  Stomach_lex(struct Stomach_Lexer* lexer, Stomach_Lexer_Func lexer_func)
{
  struct Stomach_Token token = {0};
  // if there is tokens left on the stack, pop it
  if (lexer->token_stack_end != lexer->token_stack_bottom)
  {
    struct Stomach_Token* token_ptr = lexer->token_stack_end - 1;
    token = *token_ptr;
    lexer->token_stack_end = token_ptr;
  }
  else
  {
    struct Stomach_Lexer_Output output = {0};
    if ((lexer->fd != -1) && (lexer->state == kLexerOk))
    {
      if (lexer->input.length != 0)
      {
        output = lexer_func(lexer->input, lexer->state == kLexerEof);
      }
      while (((lexer->input.length == 0) || output.trigger_read) && (lexer->state == kLexerOk))
      {
        Stomach_i64 size = read(lexer->fd, lexer->read_string + lexer->read_string_leftover_size, STOMACH_LEXER_READ_SIZE - lexer->read_string_leftover_size);
        lexer->read_string_leftover_size = 0;
        if (size == -1)
        {
          lexer->state = kLexerReadError;
          break;
        }
        else if (size == 0)
        {
          lexer->state = kLexerEof;
        }
        else if ((Stomach_u64) size > Stomach_Array_avaliable(&lexer->input_string))
        {
          Stomach_u64 avaliable_size = Stomach_Array_avaliable(&lexer->input_string);
          Stomach_Lexer_push_input_string(lexer, (Stomach_String) {.string = lexer->read_string, .length = avaliable_size});
          lexer->read_string_leftover_size = size - avaliable_size;
          memmove(lexer->read_string, lexer->read_string + avaliable_size, lexer->read_string_leftover_size);
        }
        else
        {
          Stomach_Lexer_push_input_string(lexer, (Stomach_String) {.string = lexer->read_string, .length = size});
        }
        output = lexer_func(lexer->input, lexer->state == kLexerEof);
      }
    }
    else if (lexer->input.length > 0)
    {
      output = lexer_func(lexer->input, lexer->state == kLexerEof);
    }
    if (Stomach_Slice_is_valid(output.data.content))
    {
      token = output.data;
      lexer->input.string += output.length;
      lexer->input.length -= output.length;
    }
  }
  return token;
}

void  Stomach_lex_revert(struct Stomach_Lexer* lexer, struct Stomach_Token token)
{
  struct Stomach_Token* token_on_stack = Stomach_Array_push(&lexer->token_stack, sizeof(struct Stomach_Token));
  *token_on_stack = token;
}

