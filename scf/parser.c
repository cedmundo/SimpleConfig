#include "scf/parser.h"
#include "scf/lexer.h"
#include "scf/table.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_error.h>

bool expect_word(XYZ_SCFParser* parser, const char* word, XYZ_SCFToken* token);
bool expect_punct(XYZ_SCFParser* parser,
                  const char* punct,
                  XYZ_SCFToken* token);
bool expect_type(XYZ_SCFParser* parser,
                 XYZ_SCFTokenType type,
                 XYZ_SCFToken* token);
bool parse_block(XYZ_SCFParser* parser, XYZ_SCFValue* value);
bool parse_value(XYZ_SCFParser* parser, XYZ_SCFValue* value);
XYZ_SCFPair* parse_entry(XYZ_SCFParser* parser);

void XYZ_SCFParserSetFile(XYZ_SCFParser* parser, const char* data, size_t len) {
  SDL_assert(parser != NULL && "XYZ_SCFParserSetFile: parser cannot be NULL");
  SDL_assert(data != NULL && "XYZ_SCFParserSetFile: data cannot be NULL");
  SDL_assert(len > 0 && "XYZ_SCFParserSetFile: len cannot be zero");
  XYZ_SCFStartToken(&parser->cur, data, len);
}

bool XYZ_SCFParseTable(XYZ_SCFParser* parser, XYZ_SCFTable* out_table) {
  SDL_assert(parser != NULL && "XYZ_SCFParseTable: parser cannot be NULL");
  SDL_assert(out_table != NULL &&
             "XYZ_SCFParseTable: out_table cannot be NULL");

  XYZ_SCFToken eof = {0};
  if (!expect_type(parser, XYZ_SCF_TOKEN_TYPE_START, NULL)) {
    SDL_SetError("was expecting start of file but found: '%.*s'",
                 (Sint32)parser->cur.val_len, parser->cur.val_start);
    return false;
  }

  if (expect_type(parser, XYZ_SCF_TOKEN_TYPE_EOF, NULL)) {
    return true;
  }

  while (true) {
    if (expect_type(parser, XYZ_SCF_TOKEN_TYPE_EOF, &eof)) {
      break;
    }

    XYZ_SCFPair* entry = parse_entry(parser);
    if (entry == NULL) {
      return false;
    }

    XYZ_SCFTableAdd(out_table, entry);
  }

  if (eof.type != XYZ_SCF_TOKEN_TYPE_EOF) {
    SDL_SetError("was expecting end of file but found: '%.*s'",
                 (Sint32)parser->cur.val_len, parser->cur.val_start);
    return false;
  }

  return true;
}

bool expect_word(XYZ_SCFParser* parser, const char* word, XYZ_SCFToken* token) {
  if (parser->cur.type != XYZ_SCF_TOKEN_TYPE_WORD) {
    return false;
  }

  size_t word_len = SDL_strlen(word);
  if (parser->cur.val_len != word_len) {
    return false;
  }

  if (SDL_strncmp(parser->cur.val_start, word, word_len) != 0) {
    return false;
  }

  if (token != NULL) {
    *token = parser->cur;
  }
  return XYZ_SCFNextToken(&parser->cur);
}

bool expect_punct(XYZ_SCFParser* parser,
                  const char* punct,
                  XYZ_SCFToken* token) {
  if (parser->cur.type != XYZ_SCF_TOKEN_TYPE_PUNCT) {
    return false;
  }

  size_t word_len = SDL_strlen(punct);
  if (parser->cur.val_len != word_len) {
    return false;
  }

  if (SDL_strncmp(parser->cur.val_start, punct, word_len) != 0) {
    return false;
  }

  if (token != NULL) {
    *token = parser->cur;
  }
  return XYZ_SCFNextToken(&parser->cur);
}

bool expect_type(XYZ_SCFParser* parser,
                 XYZ_SCFTokenType type,
                 XYZ_SCFToken* token) {
  if (parser->cur.type != type) {
    return false;
  }

  if (token != NULL) {
    *token = parser->cur;
  }
  return XYZ_SCFNextToken(&parser->cur);
}

XYZ_SCFPair* parse_entry(XYZ_SCFParser* parser) {
  XYZ_SCFToken key_token = {0};
  if (!expect_type(parser, XYZ_SCF_TOKEN_TYPE_WORD, &key_token)) {
    SDL_SetError("was expecting identifier but found: '%.*s'",
                 (Sint32)parser->cur.val_len, parser->cur.val_start);
    return NULL;
  }

  XYZ_SCFValue value = {0};
  if (expect_punct(parser, "{", NULL)) {
    if (!parse_block(parser, &value)) {
      return NULL;
    }

    return XYZ_SCFPairCreate(key_token.val_start, key_token.val_len, value);
  } else if (expect_punct(parser, "=", NULL)) {
    if (!parse_value(parser, &value)) {
      return NULL;
    }

    return XYZ_SCFPairCreate(key_token.val_start, key_token.val_len, value);
  }

  SDL_SetError("was expecting assign '=' or block '{' but found: '%.*s'",
               (Sint32)parser->cur.val_len, parser->cur.val_start);
  return NULL;
}

bool parse_block(XYZ_SCFParser* parser, XYZ_SCFValue* value) {
  XYZ_SCFToken eob = {0};
  XYZ_SCFTable* table = XYZ_SCFTableCreate();
  value->type = XYZ_SCF_VALUE_TYPE_TABLE;
  value->as_table = table;

  // closing right after opening: "table {}"
  while (true) {
    if (expect_punct(parser, "}", &eob)) {
      break;
    }

    XYZ_SCFPair* entry = parse_entry(parser);
    if (entry == NULL) {
      return false;
    }

    XYZ_SCFTableAdd(table, entry);
  }

  if (eob.type != XYZ_SCF_TOKEN_TYPE_PUNCT) {
    SDL_SetError("was expecting end of block '}' but found: '%.*s'",
                 (Sint32)parser->cur.val_len, parser->cur.val_start);
    return false;
  }

  return true;
}

bool parse_value(XYZ_SCFParser* parser, XYZ_SCFValue* value) {
  char digits[XYZ_SCF_MAX_DIGITS] = {0};
  XYZ_SCFToken token = {0};
  if (expect_word(parser, "nil", NULL)) {
    value->type = XYZ_SCF_VALUE_TYPE_NIL;
  } else if (expect_word(parser, "true", NULL)) {
    value->type = XYZ_SCF_VALUE_TYPE_BOOL;
    value->as_bool = true;
  } else if (expect_word(parser, "false", NULL)) {
    value->type = XYZ_SCF_VALUE_TYPE_BOOL;
    value->as_bool = false;
  } else if (expect_type(parser, XYZ_SCF_TOKEN_TYPE_INTEGER, &token)) {
    SDL_memset(digits, 0, XYZ_SCF_MAX_DIGITS);
    SDL_memcpy(digits, token.val_start,
               SDL_min(token.val_len, XYZ_SCF_MAX_DIGITS));
    value->type = XYZ_SCF_VALUE_TYPE_I32;
    value->as_i32 = SDL_atoi(digits);
  } else if (expect_type(parser, XYZ_SCF_TOKEN_TYPE_FLOAT, &token)) {
    SDL_memset(digits, 0, XYZ_SCF_MAX_DIGITS);
    SDL_memcpy(digits, token.val_start,
               SDL_min(token.val_len, XYZ_SCF_MAX_DIGITS));
    value->type = XYZ_SCF_VALUE_TYPE_F32;
    value->as_f32 = SDL_atof(digits);
  } else if (expect_type(parser, XYZ_SCF_TOKEN_TYPE_STRING, &token)) {
    size_t token_len = token.val_len;
    value->type = XYZ_SCF_VALUE_TYPE_STRING;
    value->as_string = SDL_malloc(token_len);
    if (value->as_string == NULL) {
      return false;
    }

    SDL_memset(value->as_string, 0, token_len);
    SDL_memcpy(value->as_string, token.val_start + 1,
               token_len - 2);  // unquote
  } else {
    SDL_SetError("was expecting a value but found '%.*s'",
                 (Sint32)token.val_len, token.val_start);
    return false;
  }

  return true;
}
