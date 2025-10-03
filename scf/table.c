#include "scf/table.h"

#include <SDL3/SDL_assert.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_stdinc.h>

XYZ_SCFTable* XYZ_SCFTableCreate() {
  XYZ_SCFTable* table = SDL_malloc(sizeof(XYZ_SCFTable));
  if (table == NULL) {
    return NULL;
  }
  SDL_memset(table, 0, sizeof(XYZ_SCFTable));
  return table;
}

void XYZ_SCFTableDestroy(XYZ_SCFTable* table) {
  SDL_assert(table != NULL && "XYZ_SCFTableDestroy: table cannot be NULL");
  XYZ_SCFPair* cur = table->head;
  XYZ_SCFPair* tmp = NULL;
  while (cur != NULL) {
    tmp = cur->next;
    XYZ_SCFPairDestroy(cur);
    cur = tmp;
  }
}

XYZ_SCFPair* XYZ_SCFPairCreate(const char* key, XYZ_SCFValue value) {
  SDL_assert(key != NULL && "XYZ_SCFPairCreate: key cannot be NULL");

  XYZ_SCFPair* pair = SDL_malloc(sizeof(XYZ_SCFPair));
  if (pair == NULL) {
    return NULL;
  }
  SDL_memset(pair, 0, sizeof(XYZ_SCFPair));

  size_t key_len = SDL_strlen(key);
  pair->key = SDL_malloc(key_len + 1);
  if (pair->key == NULL) {
    SDL_free(pair);
    return NULL;
  }
  SDL_memset(pair->key, 0, key_len + 1);
  SDL_memcpy((void*)pair->key, key, key_len);
  pair->value = value;
  return pair;
}

void XYZ_SCFPairDestroy(XYZ_SCFPair* pair) {
  SDL_assert(pair != NULL && "XYZ_SCFPairCreate: pair cannot be NULL");
  if (pair->key != NULL) {
    SDL_free(pair->key);
  }

  XYZ_SCFValue value = pair->value;
  if (value.type == XYZ_SCF_VALUE_TYPE_STRING) {
    SDL_free(value.as_string);
  } else if (value.type == XYZ_SCF_VALUE_TYPE_TABLE) {
    XYZ_SCFTableDestroy(value.as_table);
    SDL_free(value.as_table);
  }
}

bool XYZ_SCFTableHas(XYZ_SCFTable* table, const char* key) {
  SDL_assert(table != NULL && "XYZ_SCFTableHas: table cannot be NULL");
  SDL_assert(key != NULL && "XYZ_SCFTableHas: key cannot be NULL");
  XYZ_SCFPair* cur = table->head;
  while (cur != NULL) {
    if (SDL_strcmp(cur->key, key) == 0) {
      return true;
    }
    cur = cur->next;
  }

  return false;
}

void XYZ_SCFTableAdd(XYZ_SCFTable* table, XYZ_SCFPair* pair) {
  SDL_assert(table != NULL && "XYZ_SCFTableAdd: table cannot be NULL");
  SDL_assert(pair != NULL && "XYZ_SCFTableAdd: pair cannot be NULL");

  if (table->head == NULL) {
    table->head = pair;
  }

  if (table->tail == NULL) {
    table->tail = pair;
  } else {
    table->tail->next = pair;
    pair->prev = table->tail;
    table->tail = pair;
  }
}

bool XYZ_SCFTableSet(XYZ_SCFTable* table, const char* key, XYZ_SCFValue value) {
  SDL_assert(table != NULL && "XYZ_SCFTableSet: table cannot be NULL");
  SDL_assert(key != NULL && "XYZ_SCFTableSet: key cannot be NULL");
  XYZ_SCFPair* cur = table->head;
  while (cur != NULL) {
    if (SDL_strcmp(cur->key, key) == 0) {
      cur->value = value;
      return true;
    }
    cur = cur->next;
  }

  XYZ_SCFPair* pair = XYZ_SCFPairCreate(key, value);
  if (pair == NULL) {
    return false;
  }

  XYZ_SCFTableAdd(table, pair);
  return true;
}

bool XYZ_SCFTableGet(XYZ_SCFTable* table,
                     const char* key,
                     XYZ_SCFValue* value) {
  SDL_assert(table != NULL && "XYZ_SCFTableGet: table cannot be NULL");
  SDL_assert(key != NULL && "XYZ_SCFTableGet: key cannot be NULL");
  SDL_assert(value != NULL && "XYZ_SCFTableGet: value cannot be NULL");
  XYZ_SCFPair* cur = table->head;
  while (cur != NULL) {
    if (SDL_strcmp(cur->key, key) == 0) {
      *value = cur->value;
      return true;
    }
    cur = cur->next;
  }

  return false;
}

bool XYZ_SCFTableGetBool(XYZ_SCFTable* table, const char* key, bool* value) {
  SDL_assert(table != NULL && "XYZ_SCFTableGetBool: table cannot be NULL");
  SDL_assert(key != NULL && "XYZ_SCFTableGetBool: key cannot be NULL");
  SDL_assert(value != NULL && "XYZ_SCFTableGetBool: value cannot be NULL");

  XYZ_SCFValue any = {0};
  if (!XYZ_SCFTableGet(table, key, &any)) {
    return false;
  }

  if (any.type != XYZ_SCF_VALUE_TYPE_BOOL) {
    SDL_SetError("incompatible type for key: %s", key);
    return false;
  }

  *value = any.as_bool;
  return true;
}

bool XYZ_SCFTableGetI32(XYZ_SCFTable* table, const char* key, Sint32* value) {
  SDL_assert(table != NULL && "XYZ_SCFTableGetI32: table cannot be NULL");
  SDL_assert(key != NULL && "XYZ_SCFTableGetI32: key cannot be NULL");
  SDL_assert(value != NULL && "XYZ_SCFTableGetI32: value cannot be NULL");

  XYZ_SCFValue any = {0};
  if (!XYZ_SCFTableGet(table, key, &any)) {
    return false;
  }

  if (any.type != XYZ_SCF_VALUE_TYPE_I32) {
    SDL_SetError("incompatible type for key: %s", key);
    return false;
  }

  *value = any.as_i32;
  return true;
}

bool XYZ_SCFTableGetF32(XYZ_SCFTable* table, const char* key, float* value) {
  SDL_assert(table != NULL && "XYZ_SCFTableGetF32: table cannot be NULL");
  SDL_assert(key != NULL && "XYZ_SCFTableGetF32: key cannot be NULL");
  SDL_assert(value != NULL && "XYZ_SCFTableGetF32: value cannot be NULL");

  XYZ_SCFValue any = {0};
  if (!XYZ_SCFTableGet(table, key, &any)) {
    return false;
  }

  if (any.type != XYZ_SCF_VALUE_TYPE_F32) {
    SDL_SetError("incompatible type for key: %s", key);
    return false;
  }

  *value = any.as_f32;
  return true;
}

bool XYZ_SCFTableGetString(XYZ_SCFTable* table, const char* key, char** value) {
  SDL_assert(table != NULL && "XYZ_SCFTableGetString: table cannot be NULL");
  SDL_assert(key != NULL && "XYZ_SCFTableGetString: key cannot be NULL");
  SDL_assert(value != NULL && "XYZ_SCFTableGetString: value cannot be NULL");

  XYZ_SCFValue any = {0};
  if (!XYZ_SCFTableGet(table, key, &any)) {
    return false;
  }

  if (any.type != XYZ_SCF_VALUE_TYPE_STRING) {
    SDL_SetError("incompatible type for key: %s", key);
    return false;
  }

  *value = any.as_string;
  return true;
}

bool XYZ_SCFTableGetTable(XYZ_SCFTable* table,
                          const char* key,
                          XYZ_SCFTable** value) {
  SDL_assert(table != NULL && "XYZ_SCFTableGetTable: table cannot be NULL");
  SDL_assert(key != NULL && "XYZ_SCFTableGetTable: key cannot be NULL");
  SDL_assert(value != NULL && "XYZ_SCFTableGetTable: value cannot be NULL");

  XYZ_SCFValue any = {0};
  if (!XYZ_SCFTableGet(table, key, &any)) {
    return false;
  }

  if (any.type != XYZ_SCF_VALUE_TYPE_TABLE) {
    SDL_SetError("incompatible type for key: %s", key);
    return false;
  }

  *value = any.as_table;
  return true;
}
