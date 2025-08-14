#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "RadixTree.h"

/* Helper function to free autocomplete results */
static void free_autocomplete_results(char** results, int count) {
    if (!results) return;
    
    for (int i = 0; i < count; i++) {
        free(results[i]);
    }
    free(results);
}

/* ---------- tests ---------- */
static void test_basic_insert_and_search(void){
    RadixNode* root = radix_create_node();
    const char* words[] = {"car","care","cater","cart","dog","dot","dove","cat"};
    for(size_t i=0;i<sizeof(words)/sizeof(words[0]);++i) radix_insert(root, words[i]);

    printf("Searching for 'car': %s\n", radix_search(root, "car") ? "found" : "not found");
    printf("Searching for 'care': %s\n", radix_search(root, "care") ? "found" : "not found");
    printf("Searching for 'cat': %s\n", radix_search(root, "cat") ? "found" : "not found");
    printf("Searching for 'dove': %s\n", radix_search(root, "dove") ? "found" : "not found");

    printf("Searching for 'ca': %s\n", radix_search(root, "ca") ? "found" : "not found");
    printf("Searching for 'cars': %s\n", radix_search(root, "cars") ? "found" : "not found");
    printf("Searching for 'do': %s\n", radix_search(root, "do") ? "found" : "not found");
    printf("Searching for 'doves': %s\n", radix_search(root, "doves") ? "found" : "not found");

    printf("Prefix 'ca' %s\n", radix_find_prefix(root, "ca") ? "found" : "not found");
    printf("Prefix 'do' %s\n", radix_find_prefix(root, "do") ? "found" : "not found");
    printf("Prefix 'zzz' %s\n", radix_find_prefix(root, "zzz") ? "found" : "not found");

    int n1=0; char** a1 = radix_autocomplete(root, "ca", &n1);
    printf("Autocomplete 'ca':\n");
    for(int i=0; i<n1; ++i) printf("  %s\n", a1[i]);
    free_autocomplete_results(a1, n1);

    int n2=0; char** a2 = radix_autocomplete(root, "do", &n2);
    printf("Autocomplete 'do':\n");
    for(int i=0; i<n2; ++i) printf("  %s\n", a2[i]);
    free_autocomplete_results(a2, n2);

    int n3=0; char** a3 = radix_autocomplete(root, "care", &n3);
    printf("Autocomplete 'care':\n");
    for(int i=0; i<n3; ++i) printf("  %s\n", a3[i]);
    free_autocomplete_results(a3, n3);

    int n4=0; char** a4 = radix_autocomplete(root, "x", &n4);
    printf("Autocomplete 'x':\n");
    for(int i=0; i<n4; ++i) printf("  %s\n", a4[i]);
    free_autocomplete_results(a4, n4);

    radix_free_node(root);
}

static void test_duplicates_and_empty(void){
    RadixNode* root = radix_create_node();
    radix_insert(root, "a");
    radix_insert(root, "a");
    printf("Searching for 'a': %s\n", radix_search(root, "a") ? "found" : "not found");

    int n=0; char** xs = radix_autocomplete(root, "", &n);
    printf("Autocomplete '':\n");
    for(int i=0; i<n; ++i) printf("  %s\n", xs[i]);
    free_autocomplete_results(xs, n);

    radix_free_node(root);
}

/* ---------- main ---------- */
int main(void){
    printf("RadixTree test: starting...\n");
    test_basic_insert_and_search();
    test_duplicates_and_empty();
    printf("RadixTree test: finished.\n");
    return 0;
}