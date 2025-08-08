//trie.h
#ifndef TRIENODE_H
#define TRIENODE_H

typedef struct TrieNode TrieNode;

TrieNode* create_node();
void insert(TrieNode* root, const char* word);
int search(TrieNode* root, const char* word);
TrieNode* find_prefix_node(TrieNode* root, const char* prefix);
char* complete_first_match(TrieNode* root, const char* prefix);
void normalize(char* word);
void autocomplete(TrieNode* root, const char* prefix);
void destroy_trie(TrieNode* root);


#endif
