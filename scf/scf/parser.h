#ifndef XYZ_SCF_PARSER_H
#define XYZ_SCF_PARSER_H

#include "lexer.h"
#include "table.h"

#define XYZ_SCF_MAX_DIGITS 250

typedef struct {
  XYZ_SCFToken cur;
} XYZ_SCFParser;

/**
 * Set the parser state to the begin of given data
 */
void XYZ_SCFParserSetFile(XYZ_SCFParser* parser, const char* data, size_t len);

/**
 * Parse a file into a table
 */
bool XYZ_SCFParseTable(XYZ_SCFParser* parser, XYZ_SCFTable* table);

#endif /* XYZ_SCF_PARSER_H */
