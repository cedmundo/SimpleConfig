// clang-format off
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
// clang-format on

#include <scf/table.h>

static void table_add(void** state) {
  (void)state;

  XYZ_SCFTable* table = XYZ_SCFTableCreate();
  assert_non_null(table);

  XYZ_SCFValue value = {.type = XYZ_SCF_VALUE_TYPE_I32, .as_i32 = 1234};
  XYZ_SCFPair* pair = XYZ_SCFPairCreate("key1", SDL_strlen("key1"), value);
  assert_non_null(pair);

  XYZ_SCFTableAdd(table, pair);
  assert_ptr_equal(table->head, pair);
  assert_ptr_equal(table->tail, pair);

  pair = XYZ_SCFPairCreate("key2", SDL_strlen("key2"), value);
  assert_non_null(pair);

  XYZ_SCFTableAdd(table, pair);
  assert_ptr_not_equal(table->head, pair);
  assert_ptr_equal(table->tail, pair);

  XYZ_SCFTableDestroy(table);
  SDL_free(table);
}

static void table_has(void** state) {
  (void)state;

  XYZ_SCFTable* table = XYZ_SCFTableCreate();
  assert_non_null(table);

  XYZ_SCFValue value = {.type = XYZ_SCF_VALUE_TYPE_I32, .as_i32 = 1234};
  XYZ_SCFPair* pair = XYZ_SCFPairCreate("key1", SDL_strlen("key1"), value);
  assert_non_null(pair);

  XYZ_SCFTableAdd(table, pair);
  assert_ptr_equal(table->head, pair);
  assert_ptr_equal(table->tail, pair);

  pair = XYZ_SCFPairCreate("key2", SDL_strlen("key2"), value);
  assert_non_null(pair);

  XYZ_SCFTableAdd(table, pair);
  assert_ptr_not_equal(table->head, pair);
  assert_ptr_equal(table->tail, pair);

  bool exists = false;
  exists = XYZ_SCFTableHas(table, "key1");
  assert_true(exists);

  exists = XYZ_SCFTableHas(table, "key2");
  assert_true(exists);

  exists = XYZ_SCFTableHas(table, "invalid");
  assert_false(exists);

  XYZ_SCFTableDestroy(table);
  SDL_free(table);
}

static void table_get(void** state) {
  (void)state;

  XYZ_SCFTable* table = XYZ_SCFTableCreate();
  assert_non_null(table);

  XYZ_SCFValue value = {.type = XYZ_SCF_VALUE_TYPE_I32, .as_i32 = 1234};
  XYZ_SCFPair* pair = XYZ_SCFPairCreate("key1", SDL_strlen("key1"), value);
  assert_non_null(pair);

  XYZ_SCFTableAdd(table, pair);
  assert_ptr_equal(table->head, pair);
  assert_ptr_equal(table->tail, pair);

  value = (XYZ_SCFValue){.type = XYZ_SCF_VALUE_TYPE_F32, .as_f32 = 69.0f};
  pair = XYZ_SCFPairCreate("key2", SDL_strlen("key2"), value);
  assert_non_null(pair);

  XYZ_SCFTableAdd(table, pair);
  assert_ptr_not_equal(table->head, pair);
  assert_ptr_equal(table->tail, pair);

  bool success = false;
  success = XYZ_SCFTableGet(table, "key1", &value);
  assert_true(success);
  assert_int_equal(value.type, XYZ_SCF_VALUE_TYPE_I32);
  assert_int_equal(value.as_i32, 1234);

  success = XYZ_SCFTableGet(table, "key2", &value);
  assert_true(success);
  assert_int_equal(value.type, XYZ_SCF_VALUE_TYPE_F32);
  assert_int_equal(value.as_f32, 69.0f);

  XYZ_SCFTableDestroy(table);
  SDL_free(table);
}

static void table_set(void** state) {
  (void)state;

  XYZ_SCFTable* table = XYZ_SCFTableCreate();
  assert_non_null(table);

  XYZ_SCFValue value = {.type = XYZ_SCF_VALUE_TYPE_I32, .as_i32 = 1234};
  XYZ_SCFPair* pair = XYZ_SCFPairCreate("key1", SDL_strlen("key1"), value);
  assert_non_null(pair);

  XYZ_SCFTableAdd(table, pair);
  assert_ptr_equal(table->head, pair);
  assert_ptr_equal(table->tail, pair);

  value = (XYZ_SCFValue){.type = XYZ_SCF_VALUE_TYPE_F32, .as_f32 = 69.0f};
  pair = XYZ_SCFPairCreate("key2", SDL_strlen("key2"), value);
  assert_non_null(pair);

  XYZ_SCFTableAdd(table, pair);
  assert_ptr_not_equal(table->head, pair);
  assert_ptr_equal(table->tail, pair);

  bool success = false;
  value = (XYZ_SCFValue){.type = XYZ_SCF_VALUE_TYPE_I32, .as_i32 = 4321};
  success = XYZ_SCFTableSet(table, "key1", value);
  assert_true(success);

  XYZ_SCFValue got = {0};
  assert_true(XYZ_SCFTableGet(table, "key1", &got));
  assert_int_equal(value.type, got.type);
  assert_int_equal(value.as_i32, got.as_i32);

  value = (XYZ_SCFValue){.type = XYZ_SCF_VALUE_TYPE_F32, .as_f32 = 420.0f};
  success = XYZ_SCFTableSet(table, "key3", value);
  assert_true(success);

  assert_true(XYZ_SCFTableGet(table, "key3", &got));
  assert_int_equal(value.type, got.type);
  assert_float_equal(value.as_f32, got.as_f32, 0.000001f);

  XYZ_SCFTableDestroy(table);
  SDL_free(table);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(table_add),  // add pair to table
      cmocka_unit_test(table_has),  // table has key
      cmocka_unit_test(table_get),  // get value using key
      cmocka_unit_test(table_set),  // set old and new value using key
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
