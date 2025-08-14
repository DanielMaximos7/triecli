#ifndef RADIX_TREE_H
#define RADIX_TREE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opaque node type — callers don't need the definition.
typedef struct RadixNode RadixNode;

// Core API extracted from RadixTree.c
RadixNode* radix_create_node(void);
void       radix_insert(RadixNode *node, const char *word);
bool       radix_search(const RadixNode *node, const char *word);
const RadixNode* radix_find_prefix(const RadixNode *node, const char *prefix);
char**     radix_autocomplete(const RadixNode* root, const char* prefix, int* out_count);
void       radix_free_node(RadixNode* node);

#ifdef __cplusplus
}
#endif

#endif // RADIX_TREE_H
