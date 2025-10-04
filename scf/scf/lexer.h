#ifndef XYZ_SCF_LEXER_H
#define XYZ_SCF_LEXER_H

#include <SDL3/SDL_stdinc.h>

typedef enum {
  XYZ_SCF_TOKEN_TYPE_START,
  XYZ_SCF_TOKEN_TYPE_EOF,
  XYZ_SCF_TOKEN_TYPE_INTEGER,
  XYZ_SCF_TOKEN_TYPE_FLOAT,
  XYZ_SCF_TOKEN_TYPE_STRING,
  XYZ_SCF_TOKEN_TYPE_PUNCT,
  XYZ_SCF_TOKEN_TYPE_WORD,
} XYZ_SCFTokenType;

typedef struct {
  const char* buf_start;
  const char* val_start;
  size_t buf_len;
  size_t val_len;
  XYZ_SCFTokenType type;
} XYZ_SCFToken;

/**
 * Initializes a token at the start of src buffer.
 */
void XYZ_SCFStartToken(XYZ_SCFToken* token, const char* src, size_t len);

/**
 * Moves the val_start and val_len to the next token available in src.
 */
bool XYZ_SCFNextToken(XYZ_SCFToken* token);

#endif /* XYZ_SCF_LEXER_H */
