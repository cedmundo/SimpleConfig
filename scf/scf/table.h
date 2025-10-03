#ifndef XYZ_SCF_TABLE_H
#define XYZ_SCF_TABLE_H

#include <SDL3/SDL_stdinc.h>

struct XYZ_SCFTable;
struct XYZ_SCFPair;

typedef enum {
  XYZ_SCF_VALUE_TYPE_NIL,
  XYZ_SCF_VALUE_TYPE_BOOL,
  XYZ_SCF_VALUE_TYPE_I32,
  XYZ_SCF_VALUE_TYPE_F32,
  XYZ_SCF_VALUE_TYPE_STRING,
  XYZ_SCF_VALUE_TYPE_TABLE,
} XYZ_SCFValueType;

typedef struct {
  union {
    bool as_bool;
    Sint32 as_i32;
    float as_f32;
    char* as_string;
    struct XYZ_SCFTable* as_table;
  };
  XYZ_SCFValueType type;
} XYZ_SCFValue;

typedef struct XYZ_SCFPair {
  struct XYZ_SCFPair* next;
  struct XYZ_SCFPair* prev;
  char* key;
  XYZ_SCFValue value;
} XYZ_SCFPair;

typedef struct XYZ_SCFTable {
  XYZ_SCFPair* head;
  XYZ_SCFPair* tail;
} XYZ_SCFTable;

XYZ_SCFTable* XYZ_SCFTableCreate();
void XYZ_SCFTableDestroy(XYZ_SCFTable* table);
XYZ_SCFPair* XYZ_SCFPairCreate(const char* key, XYZ_SCFValue value);
void XYZ_SCFPairDestroy(XYZ_SCFPair* pair);
bool XYZ_SCFTableHas(XYZ_SCFTable* table, const char* key);
void XYZ_SCFTableAdd(XYZ_SCFTable* table, XYZ_SCFPair* pair);
bool XYZ_SCFTableSet(XYZ_SCFTable* table, const char* key, XYZ_SCFValue value);
bool XYZ_SCFTableGet(XYZ_SCFTable* table, const char* key, XYZ_SCFValue* value);
bool XYZ_SCFTableGetBool(XYZ_SCFTable* table, const char* key, bool* value);
bool XYZ_SCFTableGetI32(XYZ_SCFTable* table, const char* key, Sint32* value);
bool XYZ_SCFTableGetF32(XYZ_SCFTable* table, const char* key, float* value);
bool XYZ_SCFTableGetString(XYZ_SCFTable* table, const char* key, char** value);
bool XYZ_SCFTableGetTable(XYZ_SCFTable* table,
                          const char* key,
                          XYZ_SCFTable** value);
#endif /* XYZ_SCF_TABLE_H */
