#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 26
#define MAX_WORD_LEN 100

typedef struct TrieNode {
    struct TrieNode* children[ALPHABET_SIZE];
    int is_end_of_word;
} TrieNode;

TrieNode* create_node() {
    TrieNode* node = malloc(sizeof(TrieNode));
    if (node) {
        node->is_end_of_word = 0;
        for (int i = 0; i < ALPHABET_SIZE; i++)
            node->children[i] = NULL;
    }
    return node;
}

void insert(TrieNode* root, const char* word) {
    TrieNode* curr = root;
    for (int i = 0; word[i] != '\0'; i++) {
        int idx = word[i] - 'a';
        if (curr->children[idx] == NULL) {
            curr->children[idx] = create_node();
        }
        curr = curr->children[idx];
    }
    curr->is_end_of_word = 1;
}

int search(TrieNode* root, const char* word) {
    TrieNode* curr = root;
    for (int i = 0; word[i] != '\0'; i++) {
        int idx = word[i] - 'a';
        if (curr->children[idx] == NULL)
            return 0;
        curr = curr->children[idx];
    }
    return curr != NULL && curr->is_end_of_word;
}

TrieNode* find_prefix_node(TrieNode* root, const char* prefix) {
    TrieNode* curr = root;
    for (int i = 0; prefix[i] != '\0'; i++) {
        int idx = prefix[i] - 'a';
        if (curr->children[idx] == NULL)
            return NULL;
        curr = curr->children[idx];
    }
    return curr;
}

void dfs_collect(TrieNode* node, char* buffer, int depth) {
    if (node->is_end_of_word) {
        buffer[depth] = '\0';
        printf("- %s\n", buffer);
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            buffer[depth] = 'a' + i;
            dfs_collect(node->children[i], buffer, depth + 1);
        }
    }
}

void autocomplete(TrieNode* root, const char* prefix) {
    TrieNode* start = find_prefix_node(root, prefix);
    if (!start) {
        printf("No suggestions for '%s'\n", prefix);
        return;
    }

    char buffer[MAX_WORD_LEN];
    strcpy(buffer, prefix);
    dfs_collect(start, buffer, strlen(prefix));
}

void destroy_trie(TrieNode* node) {
    if (!node) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        destroy_trie(node->children[i]);
    }
    free(node);
}

