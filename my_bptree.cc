#include <iostream>
#include <strings.h>
#include <stdlib.h>
#include <cassert>
#include <math.h>
using namespace std;

#define N 4

typedef struct _data{
    int key;
    int val;
    struct _data *next;
} data;

typedef struct _node{
    bool is_leaf;
    struct _node *child[N];
    int key[N-1];
    int num_key;
    struct _node * parent;
} node;

typedef struct _temp{
    bool is_leaf;
    node *child[N+1];
    int key[N];
    int num_key;
} temp;

data head;
data *tail;
node *root = NULL;

node * alloc_node(node *parent, int i){
    node *nd;
    if(!(nd = (node *)calloc(1,sizeof(node)))){
        perror("ERROR");
        exit(EXIT_FAILURE);
    }
    if(i == 1){
        nd->is_leaf = true;
    }else{
        nd->is_leaf = false;
    }
    nd->parent = parent;
    nd->num_key = 0;
    return nd;
}

node *
alloc_root(node *left, int rs_key, node *right)
{
	node *nd;

	if (!(nd = (node *)calloc(1, sizeof(node)))){
        perror("ERROR");
        exit(EXIT_FAILURE);
    }
	nd->parent = NULL;
	nd->is_leaf = false;
	nd->key[0] = rs_key;
	nd->child[0] = left;
	nd->child[1] = right;
	nd->num_key = 1;

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
    return find_leaf(root->child[kid], key);
}

void copy_from_l2t(temp *tp, node * left){
    int i;
    bzero(tp, sizeof(tp));
    for(i = 0;i < (N - 1);i++){
        tp->child[i] = left->child[i];
        tp->key[i] = left->key[i];
    }tp->num_key = N - 1;
    tp->child[i] = left->child[i];
}

void copy_from_t2l(temp tp, node *left){
    for(int i = 0;i < (int)ceil(N / 2);i++){
        left->key[i] = tp.key[i];
        left->child[i] = tp.child[i];
        left->num_key++;
    }
}

void copy_from_t2r(temp tp, node *right){
    for(int i = (int)ceil(N/2); i < N; i++){
        right->key[i - (int)ceil(N/2)] = tp.key[i];
        right->child[i - (int)ceil(N/2)] = tp.child[i];
        right->num_key++;
    }
}

void
copy_from_t2lp(temp *tp, node *left)
{
	for (int i = 0; i < (int)ceil((N+1)/2); i++) {
		left->key[i] = tp->key[i];
		left->child[i] = tp->child[i];
		left->num_key++;
	}
	left->child[(int)ceil((N+1)/2)] = tp->child[(int)ceil((N+1)/2)];	
}

void
copy_from_t2rp(temp *tp, node *right)
{
	int id;

	for (id = ((int)ceil((N+1)/2) + 1); id < N; id++) {
		right->child[id - ((int)ceil((N+1)/2) + 1)] = tp->child[id];
		right->key[id - ((int)ceil((N+1)/2) + 1)] = tp->key[id];
		right->num_key++;
	}
	right->child[id - ((int)ceil((N+1)/2) + 1)] = tp->child[id];	

	for (int i = 0; i < right->num_key+1; i++) right->child[i]->parent = right;
}

node * insert_in_leaf(node *leaf, int key, data* dt){
    int i;
    if(key < leaf->key[0]){
        for(i = leaf->num_key; i > 0;i--){
            leaf->child[i] = leaf->child[i - 1];
            leaf->key[i] = leaf->key[i - 1];
        }
        leaf->key[0] = key;
        leaf->child[0] = (node *)dt;
    }else{
        for(i = 0;i < leaf->num_key;i++){
            if(key < leaf->key[i]) break;
        }
        for(int j = leaf->num_key;j > i;j--){
            leaf->child[j] = leaf->child[j - 1];
            leaf->key[j] = leaf->key[j - 1];
        }
        leaf->key[i] = key;
        leaf->child[i] = (node *)dt;
    }
    leaf->num_key++;

    return leaf;
}
void
insert_after_left_child(node *parent, node *left_child, int rs_key, node *right_child)
{
	int leftchild_id = 0;
	int rightchild_id = 0;
	int i;

	for (i = 0; i < parent->num_key+1; i++) {
		if (parent->child[i] == left_child) {
			leftchild_id = i;
			rightchild_id = leftchild_id+1; break; 
		}
	} 
	assert(i != parent->num_key+1);

	for (i = parent->num_key+1; i > rightchild_id; i--) parent->child[i] = parent->child[i-1];		
	for (i = parent->num_key; i > leftchild_id; i--) parent->key[i] = parent->key[i-1];

	parent->key[leftchild_id] = rs_key;
	parent->child[rightchild_id] = right_child;
	parent->num_key++;
}

void
insert_temp_after_left_child(temp *tp, node *left_child, int rs_key, node *right_child)
{
	int leftchild_id = 0;
	int rightchild_id = 0;
	int i;

	for (i = 0; i < tp->num_key+1; i++) {
		if (tp->child[i] == left_child) {
			leftchild_id = i;
			rightchild_id = leftchild_id+1; break; 
		}
	} assert(i != tp->num_key+1);

	for (i = tp->num_key+1; i > rightchild_id; i--) tp->child[i] = tp->child[i-1];		
	for (i = tp->num_key; i > leftchild_id; i--) tp->key[i] = tp->key[i-1];

	tp->key[leftchild_id] = rs_key;
	tp->child[rightchild_id] = right_child;
	tp->num_key++;
}

void insert_in_parent(node *left_child, int rs_key, node *right_child){
    node *left_parent;
    node *right_parent;
    if(left_child == root){
        root = alloc_root(left_child, rs_key, right_child);
        left_child->parent = right_child->parent = root;
        return;
    }
    left_parent = left_child->parent;
	if (left_parent->num_key < N-1) {
		insert_after_left_child(left_parent, left_child, rs_key, right_child);
	}
	else {
		temp tp;
		copy_from_l2t(&tp, left_parent);
    insert_temp_after_left_child(&tp, left_child, rs_key, right_child);

		for (int i = 0; i < N-1; i++) left_parent->key[i] = 0;
	for (int i = 0; i < N-1; i++) left_parent->child[i] = NULL;
	left_parent->num_key = 0;
		right_parent = alloc_node(left_parent->parent, 0);
		copy_from_t2lp(&tp, left_parent);
		int rs_key_parent = tp.key[(int)ceil(N/2)]; 
		copy_from_t2rp(&tp, right_parent);
		insert_in_parent(left_parent, rs_key_parent, right_parent);
	}
}

void
insert_in_temp(temp *tp, int key, void *ptr)
{
	int i;
	if (key < tp->key[0]) {
		for (i = tp->num_key; i > 0; i--) {
			tp->child[i] = tp->child[i-1] ;
			tp->key[i] = tp->key[i-1] ;
		} 
		tp->key[0] = key;
		tp->child[0] = (node *)ptr;
	}
	else {
		for (i = 0; i < tp->num_key; i++) {
			if (key < tp->key[i]) break;
		}
		for (int j = tp->num_key; j > i; j--) {		
			tp->child[j] = tp->child[j-1] ;
			tp->key[j] = tp->key[j-1] ;
		} 
		tp->key[i] = key;
		tp->child[i] = (node *)ptr;
	}

	tp->num_key++;
}

void insert(int key, data *dt){
    node *leaf;
    if(root == NULL){
        leaf = alloc_node(root, 1);
        root = leaf;
    }else{
        leaf = find_leaf(root, key);
    }

    if(leaf->num_key < (N - 1)){
        insert_in_leaf(leaf, key, dt);
    }else{
        node * left = leaf;
        node * right = alloc_node(leaf->parent, 1);
        temp tp;

        copy_from_l2t(&tp, left);
        insert_in_temp(&tp, key, dt);

        right->child[N - 1] = left->child[N - 1];
        left->child[N - 1] = right;
        
        for(int i = 0;i < N - 1;i++){
            left->key[i] = 0;
        }
        for(int i = 0;i < N - 1;i++){
            left->child[i] = NULL;
            left->num_key = 0;
        }

        copy_from_t2l(tp, left);
        copy_from_t2r(tp, right);
        int rs_key = right->key[0];

        insert_in_parent(left, rs_key, right);
    }
}

void print_tree(node * root){
    if(root == NULL){
        printf("tree is empty now\n");
        return;
    }
    printf("[");
    for(int i = 0;i < root->num_key; i++){
        if(!root->is_leaf){
            print_tree(root->child[i]);
        }
        printf("%d", root->key[i]);
        if(i != root->num_key - 1 && root->is_leaf){
            putchar(' ');
        }
    }
    if(!root->is_leaf){
            print_tree(root->child[root->num_key]);
        }
    printf("]");
}

int main(int argc, char **argv){
    while(1){
        int key;
        cout << "Key: ";
        cin >> key;
        insert(key ,NULL);
        print_tree(root);
        cout << endl;
    }
    return 0; 
}