#include "bptree.h"
#include <vector>
#include <sys/time.h>

void
print_temp(TEMP t)
{
	int i;

	for (i = 0; i < t.nkey; i++) {
		printf("[%p]", t.chi[i]);		
		printf("%d", t.key[i]);
	}
	printf("[%p]\n", t.chi[i]);		
}

void
print_tree_core(NODE *n)
{
	printf("["); 
	for (int i = 0; i < n->nkey; i++) {
		if (!n->isLeaf) print_tree_core(n->chi[i]); 
		printf("%d", n->key[i]); 
		if (i != n->nkey-1 && n->isLeaf) putchar(' ');
	}
	if (!n->isLeaf) print_tree_core(n->chi[n->nkey]);
	printf("]");
}

void
print_tree(NODE *node)
{
	print_tree_core(node);
	printf("\n"); fflush(stdout);
}

NODE *
alloc_leaf(NODE *parent)
{
	NODE *node;
	if (!(node = (NODE *)calloc(1, sizeof(NODE)))) ERR;
	node->isLeaf = true;
	node->parent = parent;
	node->nkey = 0;

	return node;
}

NODE *
alloc_internal(NODE *parent)
{
	NODE *node;
	if (!(node = (NODE *)calloc(1, sizeof(NODE)))) ERR;
	node->isLeaf = false;
	node->parent = parent;
	node->nkey = 0;

	return node;
}

NODE *
alloc_root(NODE *left, int rs_key, NODE *right)
{
	NODE *node;

	if (!(node = (NODE *)calloc(1, sizeof(NODE)))) ERR;
	node->parent = NULL;
	node->isLeaf = false;
	node->key[0] = rs_key;
	node->chi[0] = left;
	node->chi[1] = right;
	node->nkey = 1;

	return node;
}

NODE *
find_leaf(NODE *node, int key)
{
	int kid;

	if (node->isLeaf) return node;
	for (kid = 0; kid < node->nkey; kid++) {
		if (key < node->key[kid]) break;
	}

	return find_leaf(node->chi[kid], key);
}

NODE *
insert_in_leaf(NODE *leaf, int key, DATA *data)
{
	int i;
	if (key < leaf->key[0]) {
		for (i = leaf->nkey; i > 0; i--) {
			leaf->chi[i] = leaf->chi[i-1] ;
			leaf->key[i] = leaf->key[i-1] ;
		} 
		leaf->key[0] = key;
		leaf->chi[0] = (NODE *)data;
	}
	else {
		for (i = 0; i < leaf->nkey; i++) {
			if (key < leaf->key[i]) break;
		}
		for (int j = leaf->nkey; j > i; j--) {		
			leaf->chi[j] = leaf->chi[j-1] ;
			leaf->key[j] = leaf->key[j-1] ;
		} 
		leaf->key[i] = key;
		leaf->chi[i] = (NODE *)data;
	}
	leaf->nkey++;

	return leaf;
}

void
insert_in_temp(TEMP *temp, int key, void *ptr)
{
	int i;
	if (key < temp->key[0]) {
    // Write your code 
		for (i = temp->nkey; i > 0; i--) {
			temp->chi[i] = temp->chi[i-1] ;
			temp->key[i] = temp->key[i-1] ;
		} 
		temp->key[0] = key;
		temp->chi[0] = (NODE *)ptr;
	}
	else {
    // Write your code
		for (i = 0; i < temp->nkey; i++) {
			if (key < temp->key[i]) break;
		}
		for (int j = temp->nkey; j > i; j--) {		
			temp->chi[j] = temp->chi[j-1] ;
			temp->key[j] = temp->key[j-1] ;
		} 
		temp->key[i] = key;
		temp->chi[i] = (NODE *)ptr;
	}

	temp->nkey++;
}

void
erase_entries(NODE *node)
{
	for (int i = 0; i < N-1; i++) node->key[i] = 0;
	for (int i = 0; i < N; i++) node->chi[i] = NULL;
	node->nkey = 0;
}

void
copy_from_temp_to_left(TEMP temp, NODE *left)
{
	for (int i = 0; i < (int)ceil(N/2); i++) {
		left->key[i] = temp.key[i];
		left->chi[i] = temp.chi[i];
		left->nkey++;
	}
}

void
copy_from_temp_to_right(TEMP temp, NODE *right)
{
  // Write your code
	for(int i = (int)ceil(N/2) + 1;i < (int)ceil(N);i++){
		right->key[i] = temp.key[i];
		right->chi[i] = temp.chi[i];
		right->nkey++;
	}
}

void
copy_from_temp_to_left_parent(TEMP *temp, NODE *left)
{
	for (int i = 0; i < (int)ceil((N+1)/2); i++) {
		left->key[i] = temp->key[i];
		left->chi[i] = temp->chi[i];
		left->nkey++;
	}
	left->chi[(int)ceil((N+1)/2)] = temp->chi[(int)ceil((N+1)/2)];	
}

void
copy_from_temp_to_right_parent(TEMP *temp, NODE *right)
{
	int id;

	for (id = ((int)ceil((N+1)/2) + 1); id < N; id++) {
		right->chi[id - ((int)ceil((N+1)/2) + 1)] = temp->chi[id];
		right->key[id - ((int)ceil((N+1)/2) + 1)] = temp->key[id];
		right->nkey++;
	}
	right->chi[id - ((int)ceil((N+1)/2) + 1)] = temp->chi[id];	

	for (int i = 0; i < right->nkey+1; i++) right->chi[i]->parent = right;
}

void
copy_from_left_to_temp(TEMP *temp, NODE *left)
{
	int i;
	bzero(temp, sizeof(TEMP));
	for (i = 0; i < (N-1); i++) {
		temp->chi[i] = left->chi[i];
		temp->key[i] = left->key[i];
	} temp->nkey = N-1;
	temp->chi[i] = left->chi[i];	
}

void
insert_after_left_child(NODE *parent, NODE *left_child, int rs_key, NODE *right_child)
{
  // Write your code
	int lcid = 0; // left_child_id
	int rcid = 0; // right_child_id
	int rkid = 0; // right_key_id
	int i;

  // 1. Where is the place to insert?
  // 2. Making spaces for new key and new ptr
  // 3. Insert rs_key and right_child
  // 4. Increment parent->nkey
	for (i = 0; i < parent->nkey+1; i++) {
			if (parent->chi[i] == left_child) {
				lcid = i;
				rcid = lcid+1; break; 
			}
		} assert(i != parent->nkey+1);

	for (i = parent->nkey+1; i > rcid; i--) parent->chi[i] = parent->chi[i-1];		
	for (i = parent->nkey; i > lcid; i--) parent->key[i] = parent->key[i-1];

	parent->key[lcid] = rs_key;
	parent->chi[rcid] = right_child;
	parent->nkey++;
	
}

void
insert_temp_after_left_child(TEMP *temp, NODE *left_child, int rs_key, NODE *right_child)
{
	int lcid = 0;
	int rcid = 0; // right_child_id
	int i;

	for (i = 0; i < temp->nkey+1; i++) {
		if (temp->chi[i] == left_child) {
			lcid = i; // left_child_id
			rcid = lcid+1; break; 
		}
	} assert(i != temp->nkey+1);

	for (i = temp->nkey+1; i > rcid; i--) temp->chi[i] = temp->chi[i-1];		
	for (i = temp->nkey; i > lcid; i--) temp->key[i] = temp->key[i-1];

	temp->key[lcid] = rs_key;
	temp->chi[rcid] = right_child;
	temp->nkey++;
}

void
insert_in_parent(NODE *left_child, int rs_key, NODE *right_child)
{
	NODE *left_parent;
	NODE *right_parent;

	if (left_child == Root) {
		Root = alloc_root(left_child, rs_key, right_child);
		left_child->parent = right_child->parent = Root;
		return;
	}
	left_parent = left_child->parent;
	if (left_parent->nkey < N-1) {
		insert_after_left_child(left_parent, left_child, rs_key, right_child);
	}
	else {// split
    printf("Well done, but not for today. Bye.\n");
    exit(0);
	}
}

void 
insert(int key, DATA *data)
{
	NODE *leaf;

	if (Root == NULL) {
		leaf = alloc_leaf(NULL);
		Root = leaf;
	}
	else {
		leaf = find_leaf(Root, key);
	}

	if (leaf->nkey < (N-1)) {
		insert_in_leaf(leaf, key, data);
	}
	else { // split
		NODE *left = leaf;
		NODE *right = alloc_leaf(leaf->parent);
		TEMP temp;

		copy_from_left_to_temp(&temp, left);
		insert_in_temp(&temp, key, data);
    print_temp(temp);// here
    
    exit(0);

		right->chi[N-1] = left->chi[N-1];	
		left->chi[N-1] = right;
		erase_entries(left);
		copy_from_temp_to_left(temp, left);
		copy_from_temp_to_right(temp, right);
		int rs_key = right->key[0]; // right smallest key
		insert_in_parent(left, rs_key, right);
	}
}

void
init_root(void)
{
	Root = NULL;
}

void
search_core(const int key)
{
  NODE *n = find_leaf(Root, key);
	for (int i = 0; i < n->nkey+1; i++) {
		if (n->key[i] == key) return;
	}
  cout << "Key not found: " << key << endl;
	ERR;
}

int 
interactive()
{
  int key;

  std::cout << "Key: ";
  std::cin >> key;

  return key;
}

int
main(int argc, char *argv[])
{
	init_root();
  while (true) {
		insert(interactive(), NULL);
    print_tree(Root);
  }

	return 0;
}
