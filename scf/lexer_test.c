// clang-format off
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
// clang-format on

#include <scf/lexer.h>

static void lex_int(void** state) {
  (void)state;

  const char* src_data = "0 -1 123";
  size_t src_size = SDL_strlen(src_data);

  XYZ_SCFToken token = {0};
  XYZ_SCFStartToken(&token, src_data, src_size);
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_START);
  assert_int_equal(token.val_start, token.buf_start);
  assert_int_equal(token.val_len, 0);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_INTEGER);
  assert_int_equal(token.val_len, 1);
  assert_memory_equal(token.val_start, "0", 1);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_INTEGER);
  assert_int_equal(token.val_len, 2);
  assert_memory_equal(token.val_start, "-1", 2);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_INTEGER);
  assert_int_equal(token.val_len, 3);
  assert_memory_equal(token.val_start, "123", 3);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_EOF);
}

static void lex_float(void** state) {
  (void)state;

  const char* src_data = "0.0 -1.0 123.456";
  size_t src_size = SDL_strlen(src_data);

  XYZ_SCFToken token = {0};
  XYZ_SCFStartToken(&token, src_data, src_size);
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_START);
  assert_int_equal(token.val_start, token.buf_start);
  assert_int_equal(token.val_len, 0);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_FLOAT);
  assert_int_equal(token.val_len, 3);
  assert_memory_equal(token.val_start, "0.0", 3);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_FLOAT);
  assert_int_equal(token.val_len, 4);
  assert_memory_equal(token.val_start, "-1.0", 4);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_FLOAT);
  assert_int_equal(token.val_len, 7);
  assert_memory_equal(token.val_start, "123.456", 7);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_EOF);
}

static void lex_string(void** state) {
  (void)state;

  const char* src_data = "\"hello\" \"\"";
  size_t src_size = SDL_strlen(src_data);

  XYZ_SCFToken token = {0};
  XYZ_SCFStartToken(&token, src_data, src_size);
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_START);
  assert_int_equal(token.val_start, token.buf_start);
  assert_int_equal(token.val_len, 0);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_STRING);
  assert_int_equal(token.val_len, 7);
  assert_memory_equal(token.val_start, "\"hello\"", 7);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_STRING);
  assert_int_equal(token.val_len, 2);
  assert_memory_equal(token.val_start, "\"\"", 2);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_EOF);
}

static void lex_punct(void** state) {
  (void)state;

  const char* src_data = "{} =";
  size_t src_size = SDL_strlen(src_data);

  XYZ_SCFToken token = {0};
  XYZ_SCFStartToken(&token, src_data, src_size);
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_START);
  assert_int_equal(token.val_start, token.buf_start);
  assert_int_equal(token.val_len, 0);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_PUNCT);
  assert_int_equal(token.val_len, 1);
  assert_memory_equal(token.val_start, "{", 1);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_PUNCT);
  assert_int_equal(token.val_len, 1);
  assert_memory_equal(token.val_start, "}", 1);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_PUNCT);
  assert_int_equal(token.val_len, 1);
  assert_memory_equal(token.val_start, "=", 1);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_EOF);
}

static void lex_word(void** state) {
  (void)state;

  const char* src_data = "hello world1 _false true";
  size_t src_size = SDL_strlen(src_data);

  XYZ_SCFToken token = {0};
  XYZ_SCFStartToken(&token, src_data, src_size);
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_START);
  assert_int_equal(token.val_start, token.buf_start);
  assert_int_equal(token.val_len, 0);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_WORD);
  assert_int_equal(token.val_len, 5);
  assert_memory_equal(token.val_start, "hello", 5);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_WORD);
  assert_int_equal(token.val_len, 6);
  assert_memory_equal(token.val_start, "world1", 6);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_WORD);
  assert_int_equal(token.val_len, 6);
  assert_memory_equal(token.val_start, "_false", 6);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_WORD);
  assert_int_equal(token.val_len, 4);
  assert_memory_equal(token.val_start, "true", 4);

  assert_true(XYZ_SCFNextToken(&token));
  assert_int_equal(token.type, XYZ_SCF_TOKEN_TYPE_EOF);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(lex_int),    cmocka_unit_test(lex_float),
      cmocka_unit_test(lex_string), cmocka_unit_test(lex_punct),
      cmocka_unit_test(lex_word),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
