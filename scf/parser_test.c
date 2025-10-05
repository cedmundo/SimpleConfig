// clang-format off
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
// clang-format on

#include <scf/parser.h>
#include "scf/table.h"

static void parse_single_entry(void** state) {
  (void)state;

  XYZ_SCFParser parser = {0};
  const char* src = "key = 123";
  size_t src_len = SDL_strlen(src);
  XYZ_SCFParserSetFile(&parser, src, src_len);

  XYZ_SCFTable* table = XYZ_SCFTableCreate();
  assert_true(XYZ_SCFParseTable(&parser, table));

  XYZ_SCFValue value = {0};
  assert_true(XYZ_SCFTableGet(table, "key", &value));
  assert_int_equal(value.type, XYZ_SCF_VALUE_TYPE_I32);
  assert_int_equal(value.as_i32, 123);

  XYZ_SCFTableDestroy(table);
  SDL_free(table);
}

static void parse_multiple_entries(void** state) {
  (void)state;

  XYZ_SCFParser parser = {0};
  const char* src = "key1 = nil key2 = true key3 = 123";
  size_t src_len = SDL_strlen(src);
  XYZ_SCFParserSetFile(&parser, src, src_len);

  XYZ_SCFTable* table = XYZ_SCFTableCreate();
  assert_true(XYZ_SCFParseTable(&parser, table));

  XYZ_SCFValue value = {0};
  assert_true(XYZ_SCFTableGet(table, "key1", &value));
  assert_int_equal(value.type, XYZ_SCF_VALUE_TYPE_NIL);

  assert_true(XYZ_SCFTableGet(table, "key2", &value));
  assert_int_equal(value.type, XYZ_SCF_VALUE_TYPE_BOOL);
  assert_int_equal(value.as_bool, true);

  assert_true(XYZ_SCFTableGet(table, "key3", &value));
  assert_int_equal(value.type, XYZ_SCF_VALUE_TYPE_I32);
  assert_int_equal(value.as_i32, 123);

  XYZ_SCFTableDestroy(table);
  SDL_free(table);
}

static void parse_subtables(void** state) {
  (void)state;

  XYZ_SCFParser parser = {0};
  const char* src = "sub { key = 123 }";
  size_t src_len = SDL_strlen(src);
  XYZ_SCFParserSetFile(&parser, src, src_len);

  XYZ_SCFTable* table = XYZ_SCFTableCreate();
  assert_true(XYZ_SCFParseTable(&parser, table));

  XYZ_SCFValue value1 = {0};
  XYZ_SCFValue value2 = {0};
  assert_true(XYZ_SCFTableGet(table, "sub", &value1));
  assert_int_equal(value1.type, XYZ_SCF_VALUE_TYPE_TABLE);

  assert_true(XYZ_SCFTableGet(value1.as_table, "key", &value2));
  assert_int_equal(value2.type, XYZ_SCF_VALUE_TYPE_I32);
  assert_int_equal(value2.as_i32, 123);

  XYZ_SCFTableDestroy(table);
  SDL_free(table);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(parse_single_entry),
      cmocka_unit_test(parse_multiple_entries),
      cmocka_unit_test(parse_subtables),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
