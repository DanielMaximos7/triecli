#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct RadixNode RadixNode;

typedef struct {
    char       *label;
    uint16_t    len;
    uint8_t     first;
    RadixNode  *child;
} RadixEdge;

struct RadixNode {
    RadixEdge  *edges;
    uint16_t    num_edges;
    uint16_t    cap_edges;  
    uint16_t    frequency;
    uint16_t    is_end_of_word : 1;
};

/* HELPER FUNCTIONS  */

static inline void edge_set_label(RadixEdge *e, char *owned_cstr) {
    e->label = owned_cstr;
    e->len = (uint16_t)strlen(owned_cstr); //cache length of str 
    e->first = e->len ? (uint8_t)owned_cstr[0] : 0; //cache first byte of str
}

static inline void edge_replace_label(RadixEdge *e, char *owned_ctsr) {
    char *old = e->label;
    edge_set_label(e, owned_ctsr);
    free(old);
}

/* Walks both strings up to a_len or until a mismatch/null in b, returns the match len */
int common_prefix_len(const char *a, int a_len, const char *b) {
    int i;
    
    for (i = 0; i < a_len; ++i) {
        char cb = b[i];
        if (cb == '\0' || a[i] != cb) 
            break;
    }
    
    return i;
}

/* Binary search for the first edge whose first byte is >= key  */
static int lower_bound_first(const RadixEdge *a, int n, uint8_t key) {
    int lo = 0, hi = n;
    
    while (lo < hi) {
        int mid = (lo + hi) >> 1;
        
        if (a[mid].first < key) 
            lo = mid + 1;
        else 
            hi = mid;
    }

    return lo;
}

/* Binary search for the first edge whose first byte is > key */
static int upper_bound_first(const RadixEdge *a, int n, uint8_t key) {
    int lo = 0, hi = n;
    
    while (lo < hi) {
        int mid = (lo + hi) >> 1;
        
        if (a[mid].first <= key) 
            lo = mid + 1;
        else 
            hi = mid;
    }

    return lo;
}

/* Ensures node->edges has capacity for at least min_cap entries, growing if needed */
static bool grow_edges(RadixNode *node, int min_cap) {
    if(node->edges && node->cap_edges >= min_cap)
        return true;

    int cap = node->cap_edges ? node->cap_edges : 2;
    while (cap < min_cap) 
        cap <<= 1;
    
    if((size_t)cap > SIZE_MAX / sizeof *node->edges)
        return false;

    RadixEdge *p = realloc(node->edges, (size_t)cap * sizeof *p);
    if (!p) 
        return false;

    node->edges = p; 
    node->cap_edges = cap;
    return true;
}

/* Insert edge e into node->edges, maintaining ascending order by first byte */
static bool node_insert_edge_sorted(RadixNode *node, RadixEdge e) {
    if (node->num_edges == node->cap_edges)
        if (!grow_edges(node, node->num_edges + 1))
                return false;
    
    int pos = upper_bound_first(node->edges, node->num_edges, e.first);
    
    /* Move existing edges at and after pos one slot to the right */
    memmove(&node->edges[pos + 1], &node->edges[pos], (size_t)(node->num_edges - pos) * sizeof(RadixEdge));

    node->edges[pos] = e;
    node->num_edges++;
    return true;
}

/* CORE API  */

/* */
RadixNode* radix_create_node(void) {
    RadixNode *node = malloc(sizeof(RadixNode));
    if(!node) 
        return NULL;

    node->edges = NULL;
    node->num_edges = 0;
    node->cap_edges = 0;
    node->is_end_of_word = 0;
    node->frequency = 0;
    
    return node;
}

void radix_insert(RadixNode* node, const char* word){
    for(int i = 0; i < node->num_edges; i++){
        RadixEdge* edge = &node->edges[i];
        int prefix_len = common_prefix_len(edge->label, edge->len ,word);

        if(prefix_len == 0) continue;
        
        if(prefix_len == edge->len){
            
            radix_insert(edge->child, word + prefix_len);
            return;
        }

        RadixNode* split_node = radix_create_node();
        
        RadixEdge old_suffix = {0};
        edge_set_label(&old_suffix, xstrdup(edge->label + prefix_len));
        old_suffix.child = edge->child;

        RadixEdge new_edge = {0};
        edge_set_label(&new_edge, xstrdup(word + prefix_len));
        new_edge.child = radix_create_node();
        new_edge.child->is_end_of_word = 1;
        new_edge.child->frequency = 1;

        if(!node_insert_edge_sorted(split_node, old_suffix)){
            return;
        }

        if(!node_insert_edge_sorted(split_node, new_edge)){
            return;
        }
      
        edge->child = split_node;
        edge_replace_label(edge, xstrndup(edge->label, (size_t)prefix_len));
        
        return;
    }
    
    //no match insert new edge //TODO: Realloc clobber risk...
    if(node->num_edges == node->cap_edges){
        node->cap_edges = node->cap_edges ? node->cap_edges * 2 : 2;
        node->edges = realloc(node->edges, node->cap_edges * sizeof(RadixEdge));
    }

    
    RadixEdge e = {0};

    edge_set_label(&e, xstrdup(word));
    e.child = radix_create_node();

    e.child->is_end_of_word = 1;
    e.child->frequency = 1;
    //inserts the new node in the right pos
    if(!node_insert_edge_sorted(node, e)){
        return;
    }
}

bool radix_search(const RadixNode* node, const char* word){
    
    if(word[0] == '\0'){
        return node->is_end_of_word != 0;
    }
    
    for(int i = 0; i < node->num_edges; ++i){
        const RadixEdge* edge = &node->edges[i];
        
        int k = common_prefix_len(edge->label, edge->len,word);

        if (k == 0) continue;
        
        if(k < (int)edge->len){
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
 
    uint8_t ch = (uint8_t)prefix[0];

    int hi = upper_bound_first(node->edges, node->num_edges, ch);
    int lo = lower_bound_first(node->edges, node->num_edges, ch);

    for(int i = lo; i < hi; ++i){
        const RadixEdge* edge = &node->edges[i]; //get their mem addrs
        //calculate the no. of common chars 
        int k = common_prefix_len(edge->label, edge->len,prefix);
        
        if (k == 0) continue;
        //we wouldn't be hitting another node where we can look to more edges
        if(k < (int)edge->len) return NULL; //i.e. we'd end in middle of an edg
        
        if(prefix[k] == '\0') return edge->child;

        return radix_find_prefix(edge->child, prefix + k);
    }

    return NULL;
}

static void push_string(char*** out, int* n, int* cap, const char* s){
    if(*n == *cap){
        *cap = (*cap == 0) ? 8 : (*cap * 2);
        *out = realloc(*out, (*cap) * sizeof(char*));
    }
    (*out)[(*n)++] = strdup(s);
}

static void dfs_collect(const RadixNode* node, char** ptr_ buffer, int buf_len, int* ptr_bcap, char*** out, int* out_n, int* out_cap){
    
    //ptr_ to callers buffer scratch and cap
    char* buffer = *ptr_buffer;
    int buf_cap = *ptr_bcap;

    if(node->is_end_of_word){
        //buffer scratch string building
        buffer[buf_len] = '\0';
        push_string(out, out_n, out_cap, buffer);
    }
    
    for(int i = 0; i < node->num_edges; ++i){
        const RadixEdge* e = &node->edges[i];
        //lab_len -> the bytes that are going to be appended this step
        int lab_len = (int)e->len;
        //buf_len (how many bytes are meaningful) - buf_cap (buffer space)
        //checking to see if we can add the previous word plus the bytes that we're going to append this step + 1 ('\0')
        if(buf_len + lab_len + 1 > buf_cap){
            
            int new_cap = buf_cap ? buf_cap : 32;
            
            while(buf_len + lab_len + 1 > new_cap)  new_cap *= 2;
           
            char* grown = realloc(buffer, new_cap);

            if(!grown) return;
  
            buffer = grown; buf_cap = new_cap; //keep this frame correct
            *ptr_buffer = buffer; *ptr_bcap = buf_cap; //keep caller's frame correct     
        }

        memcpy(buffer + buf_len, e->label, lab_len);

        dfs_collect(e->child, ptr_buffer, buf_len + lab_len, ptr_bcap, out, out_n, out_cap);
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

    dfs_collect(at, &buffer, buf_len, &buf_cap, &results, &n, &cap);

    free(buffer);

    *out_count = n;

    return results;
}

void radix_free_node(RadixNode* node){
    if(!node) return;

    for(int i = 0; i < node->num_edges; ++i){
        RadixEdge* e = &node->edges[i];

        radix_free_node(e->child);

        free(e->label);
    }

    free(node->edges);

    free(node);
}
