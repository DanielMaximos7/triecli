#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET_SIZE 26
#define MAX_WORD_LEN 100

typedef struct RadixNode RadixNode;

typedef struct {
    char* label;
    RadixNode* child;
} RadixEdge;

struct RadixNode {
    RadixEdge* edges;
    int num_edges;
    int cap_edges;
    int is_end_of_word;
    int frequency;
};

RadixNode* radix_create_node(){
    RadixNode* node = malloc(sizeof(RadixNode));
    if(!node) return NULL;

    node->edges = NULL;
    node->num_edges = 0;
    node->cap_edges = 0;
    node->is_end_of_word 0;
    node->frequency = 0;
    return node;
}

int common_prefix_len(const char* a, const char* b){
    int i = 0;
    while(a[i] && b[i] && a[i] == b[i]) i++;
    return i;
}

void radix_insert(RadixNode* node, const char* word){
    for(int = 0; i < node->num_edges; i++){
        RadixEdge* edge = &node->edges[i];
        int prefix_len = common_prefix_len(edge->label, word);

        if(prefix_len == 0) continue;
        
        if(prefix_len == strlen(edge->label)){
            
            radix_insert(edge->child, word + prefix_len);
            return;
        }

        RadixNode* split_node = radix_create_node();

        RadixEdge old_suffix = {
            .label = strdup(edge->label + prefix_len),
            .child = edge->child
        };

        RadixEdge = new_edge = {
            .label = strdup(word + prefix_len),
            .child = radix_create_node()
        };
        new_edge.child->is_end_of_word = 1;
        new_edge.child->frequency = 1;

        split_node->edges = malloc(2 * sizeof(RadixEdge));
        split_node->edges[0] = old_suffix;
        split_node->edges[1] = new_edge;
        split_node->num_edges = 2;
        split_node->cap_edges = 2;

        free(edge->label);
        edge->label = strndup(edge->label, prefix_len);
        edge->child = split_node;

        return;
    }
    
    //no match insert new edge
    if(node->num_edges == node->cap_edges){
        node->cap_edges = node->cap_edges ? node->cap_edges * 2 : 2;
        node->edges = realloc(node->edges, node->cap_edges * sizeof(RadixEdge));
    }

    RadixEdge new_edge = {
        .label = strdup(word),
        .child = radix_create_node()
    };
    new_edge.child->is_end_of_word = 1;
    new_edge.child->frequency = 1;
    
    node->edges[node->num_edges++] = new_edge;
}

bool radix_search(const RadixNode* node, const char* word){
    
    if(word[0] == '\0'){
        return node->is_end_of_word != 0;
    }
    
    for(int i = 0; i < node->num_edges; ++i){
        const RadixEdge* edge = &node->edges[i];
        
        int k = common_prefix_len(edge->label, word);

        if (k == 0) continue;
        
        if(k < (int)strlen(edge->label)){
            return false;
        }
            
        if(word[k] == '\0'){
            return edge->child->is_end_of_word != 0;
        }
        return radix_search(edge->child, word + k);
    }
    
    return false;
}

const RadixNode* radix_find_prefix(const RadixNode* node, const char* prefix){
    if(prefix[0] == '\0') return node;
    //for the current node, check all the edges the node contains
    for(int i =0; i < node->num_edges; ++i){
        const RadixEdge* edge = &node->edges[i]; //get their mem addrs
        //calculate the no. of common chars 
        int k = common_prefix_len(edge->label, prefix);
        
        if (k == 0) continue;
        //we wouldn't be hitting another node where we can look to more edges
        if(k < (int)strlen(edge->label)) return NULL; //i.e. we'd end in middle of an edg
        
        if(prefix[k] == '\0') return edge->child;

        return radix_find_prefix(edge->child, prefix + k);
    }

    return NULL;
}

static void push_string(char*** out, int* n, int* cap, const char* s){
    if(*n == *cap){
        *cap = (*cap == 0) ? 8 : (*cap * 2);
        *out = realloc(*out, (*cap) * size(char*));
    }
    (*out)[(*n)++] = strdup(s);
}





