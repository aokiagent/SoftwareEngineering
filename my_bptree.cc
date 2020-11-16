#include <iostream>
#include <string>
using namespacce std;

typedef struct data{
    int key;
    int val;
    struct data *next;
}data;

typedef struct node{
    int key[N-1];
    int num_key;
    bool is_leaf;
    struct node *parent;
    struct node *chi[N];
}node;

typedef struct temp{
    bool is_leaf;
    node *chi[N+1];
    int key[N];
    int num_key;
}temp;

data head;
data *tail;
node *root = NULL;

node * alloc_leaf(node *parent){
    node *nd;
    if(!(nd = (node *)calloc(1,sizeof(node)))) ERR;
    nd->is_leaf = true;
    nd->parent = parent;
    nd->num_key = 0;
    return nd;
}

node * find_leaf(node *root, int key){
    int kid;
    if(root->is_leaf){
        return root;
    }
    for(kid = 0;kid < root->num_key; kid++){
        if(key < root->key[kid])break;
    }
    return find_leaf(root->chi[kid], key);
}

node * insert_in_leaf(node *leaf, int key, data* dt){
    int i;
    if(key < leaf->key[0]){
        for(i = leaf->num_key; i > 0;i--){
            leaf->chi[i] = leaf->chi[i - 1];
            leaf->num_key[i] = leaf->key[i - 1];
        }
        leaf->key[0] = key;
        leaf->chi[0] = (node *)dt;
    }else{
        for(i = 0;i < leaf->num_key;i++){
            if(key < leaf->key[i]) break;
        }
    }
}

void insert(int key, data *dt){
    node *leaf;
    if(root == NULL){
        leaf = alloc_leaf(root);
        root = leaf;
    }else{
        leaf = find_leaf(root, key);
    }

    if(leaf->num_key < (N - 1)){
        insert_in_leaf(leaf, key, dt);
    }
}

void print_tree(node * root){
    if(root == NULL){
        printf("tree is empty now\n");
        return;
    }
    printf('[');
    for(int i = 0;i < root->num_key; i++){
        if(!root->is_leaf){
            print_tree(root->chi[i]);
        }
        printf("%d", root->key[i]);
        if(i != root->num_key - 1 && root->is_leaf){
            putchar(' ');
        }
    }
    if(!root->is_leaf){
            print_tree(root->chi[root->num_key]);
        }
    printf(']');
}

void init(){
    root = NULL;
}

int main(int argc, char **argv){
    init();
    while(1){
        insert();
        print_tree(root);
    }
    return 0; 
}