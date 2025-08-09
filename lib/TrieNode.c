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

static void dfs_collect(const RadixNode* node, char* buffer, int buf_len, int buf_cap, char*** out, int* out_n, int* out_cap){
    if(node->is_end_of_word){
        //buffer scratch string building
        buffer[buf_len] = '\0';
        push_string(out, out_n, out_cap, buffer);
    }
    
    for(int i = 0; i < node->num_edges; ++i){
        const RadixEdge* e = &node->edges[i];
        //lab_len -> the bytes that are going to be appended this step
        int lab_len = (int)strlen(e->label);
        //buf_len (how many bytes are meaningful) - buf_cap (buffer space)
        //checking to see if we can add the previous word plus the bytes that we're going to append this step + 1 ('\0')
        if(buf_len + lab_len + 1 > buf_cap){
            
            int new_cap = buf_cap;
            while(buf_len + lab_len + 1 > new_cap) new_cap = (new_cap == 0) ? 32 : (new_cap *2);
            char* grown = malloc(new_cap);
            //copy whats already in buffer (current no. of chars buf_len) into grown
            memcpy(grown, buffer, buf_len);\
            //set the buffer as grown (which now includes old buffer + new mem
            buffer = grown;
            //increase the cap so next time we run out (dynamic malloc)
            buf_cap = new_cap;
        }

        memcpy(buffer + buf_len, e->label, lab_len);

        dfs_collect(e->child, buffer, buf_len + lab_len, buf_cap, out, out_n, out_cap);
    }
}

char** radix_autocomplete(const RadixNode* root, const char* prefix, int* out_count){
    *out_count = 0;

    const RadixNode* at = radix_find_prefix(root, prefix);
    if(!at) return NULL;

    int buf_cap = (int)strlen(prefix) + 16;
    char* buffer = malloc(buf_cap);
    int buf_len = (int)strlen(prefix);
    memcpy(buffer, prefix, buf_len);

    char** results = NULL;
    int n = 0, cap = 0;

    dfs_collect(at, buffer, buf_len, buf_cap, &results, &n, &cap);

    free(buffer);

    *out_count = n;

    return results;
}

