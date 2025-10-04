#include "scf/lexer.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_stdinc.h>

typedef enum {
  l_state_any,
  l_state_int,
  l_state_float,
  l_state_string,
  l_state_word,
} l_state;

typedef struct {
  l_state set_state;
  bool error;
  bool accept;
  bool emit;
  XYZ_SCFTokenType emit_type;
} l_action;

// check whats the next action to take after the given UCP
l_action ucp_action(l_state cur_state, Sint32 ucp);

void XYZ_SCFStartToken(XYZ_SCFToken* token, const char* src, size_t len) {
  SDL_assert(token != NULL && "XYZ_SCFStartToken: token cannot be NULL");
  SDL_assert(src != NULL && "XYZ_SCFStartToken: src cannot be NULL");
  SDL_assert(len > 0 && "XYZ_SCFStartToken: len cannot be zero");
  *token = (XYZ_SCFToken){
      .type = XYZ_SCF_TOKEN_TYPE_START,
      .buf_start = src,
      .buf_len = len,
      .val_start = src,
      .val_len = 0,
  };
}

bool XYZ_SCFNextToken(XYZ_SCFToken* token) {
  SDL_assert(token != NULL && "XYZ_SCFNextToken: token cannot be NULL");
  l_state cur_state = l_state_any;
  const char* buf_start = token->buf_start;
  const char* buf_end = token->buf_start + token->buf_len;
  const char* token_start = token->val_start + token->val_len;
  Uint32 ucp = 0;

  if (token_start >= buf_end) {
    token->val_start = token_start;
    token->val_len = 0;
    token->type = XYZ_SCF_TOKEN_TYPE_EOF;
    return true;
  }

  while (token_start <= buf_end) {
    ucp = SDL_StepUTF8(&token_start, NULL);
    if (ucp == 0) {
      token->val_start = token_start;
      token->val_len = 0;
      token->type = XYZ_SCF_TOKEN_TYPE_EOF;
      return true;
    }

    if (ucp != ' ' && ucp != '\t' && ucp != '\n') {
      SDL_StepBackUTF8(buf_start, &token_start);
      break;
    }
  }

  const char* token_end = token_start;
  const char* token_tmp = token_start;
  while (token_tmp <= buf_end) {
    ucp = SDL_StepUTF8(&token_tmp, NULL);
    l_action action = ucp_action(cur_state, ucp);
    cur_state = action.set_state;
    if (action.error) {
      return false;
    }

    if (action.accept) {
      token_end = token_tmp;
    }

    if (action.emit) {
      token->val_start = token_start;
      token->val_len = token_end - token_start;
      token->type = action.emit_type;
      return true;
    }
  }

  return false;
}

l_action ucp_action(l_state cur_state, Sint32 ucp) {
  l_action action = {
      .set_state = cur_state,
      .error = false,
      .accept = false,
      .emit = false,
  };

  switch (cur_state) {
    case l_state_any:
      if (ucp == '-' || (ucp >= '0' && ucp <= '9')) {
        action.set_state = l_state_int;
        action.accept = true;
      } else if (ucp == '"') {
        action.set_state = l_state_string;
        action.accept = true;
      } else if (ucp == '{' || ucp == '}' || ucp == '=') {
        action.set_state = l_state_any;
        action.accept = true;
        action.emit_type = XYZ_SCF_TOKEN_TYPE_PUNCT;
        action.emit = true;
      } else if (ucp == '_' || (ucp >= 'a' && ucp <= 'z') ||
                 (ucp >= 'A' && ucp <= 'Z')) {
        action.set_state = l_state_word;
        action.accept = true;
      } else {
        action.set_state = l_state_any;
        action.accept = true;

        char c[8] = {0};
        SDL_UCS4ToUTF8(ucp, (char*)c);
        SDL_SetError("unknown character: '%s'\n", c);
      }
      break;
    case l_state_int:
      if (ucp >= '0' && ucp <= '9') {
        action.set_state = l_state_int;
        action.accept = true;
      } else if (ucp == '.') {
        action.set_state = l_state_float;
        action.accept = true;
      } else {
        action.set_state = l_state_any;
        action.emit_type = XYZ_SCF_TOKEN_TYPE_INTEGER;
        action.emit = true;
      }
      break;
    case l_state_float:
      if (ucp >= '0' && ucp <= '9') {
        action.set_state = l_state_float;
        action.accept = true;
      } else {
        action.set_state = l_state_any;
        action.emit_type = XYZ_SCF_TOKEN_TYPE_FLOAT;
        action.emit = true;
      }
      break;
    case l_state_string:
      if (ucp == '"') {
        action.set_state = l_state_any;
        action.emit_type = XYZ_SCF_TOKEN_TYPE_STRING;
        action.accept = true;
        action.emit = true;
      } else if (ucp == '\n' || ucp == 0) {
        SDL_SetError("unterminated string");
        action.error = true;
      } else {
        action.set_state = l_state_string;
        action.accept = true;
      }
      break;
    case l_state_word:
      if (ucp == '_' || (ucp >= 'a' && ucp <= 'z') ||
          (ucp >= 'A' && ucp <= 'Z') || (ucp >= '0' && ucp <= '9')) {
        action.set_state = l_state_word;
        action.accept = true;
      } else {
        action.set_state = l_state_any;
        action.emit_type = XYZ_SCF_TOKEN_TYPE_WORD;
        action.emit = true;
      }
      break;
    default:
      SDL_SetError("unknown state: %d", cur_state);
      action.error = true;
  }

  return action;
}
