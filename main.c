// Simple (educational purposes only) AVL Tree implementation in C.
// Copyright (C) 2013 Tadeusz Magura-Witkowski

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct node {
	int key;
	struct node *l;
	struct node *r;

	int bf;
};

static int node_height(struct node *node) {
	if(node == NULL)
		return 0;

	if(node->bf > 0) {
		return 1 + node_height(node->r);
	} else {
		return 1 + node_height(node->l);
	}
}

static void recalculate_bf(struct node *node) {
	assert(node != NULL);

	node->bf = node_height(node->r) - node_height(node->l);
}

static struct node * new_node(int key) {
	struct node *nnode = malloc(sizeof(struct node));

	assert(nnode != NULL);

	nnode->key = key;

	nnode->l = nnode->r = NULL;
	nnode->bf = 0;

	return nnode;
}

static void del_node(struct node *node) {
	assert(node != NULL);

	if(node->l)
		del_node(node->l);

	if(node->r)
		del_node(node->r);

	free(node);
}

static struct node * rotate_rr(struct node *node);
static struct node * rotate_ll(struct node *node);

static struct node * rotate_rr(struct node *node) { // Right Right Case
	assert(node != NULL);
	assert(node->r != NULL);

	struct node *nnode = node->r;

	node->r = nnode->l;
	nnode->l = node;

	recalculate_bf(node);
	recalculate_bf(nnode);

	return nnode;
}

static struct node * rotate_ll(struct node *node) { // Left Left Case
	assert(node != NULL);
	assert(node->l != NULL);

	struct node *nnode = node->l;

	node->l = nnode->r;
	nnode->r = node;

	recalculate_bf(node);
	recalculate_bf(nnode);

	return nnode;
}

static struct node * rotate_rl(struct node *node) { // Right Left Case
	assert(node != NULL);

	node->r = rotate_ll(node->r);

	return rotate_rr(node);
}

static struct node * rotate_lr(struct node *node) { // Left Right Cast
	assert(node != NULL);

	node->l = rotate_rr(node->l);

	return rotate_ll(node);
}

static struct node * add_node(struct node *root, struct node *node) {
	assert(node != NULL);

	if(root == NULL) {
		return node;
	}

	if(node->key > root->key) {
		if(root->r) {
			root->r = add_node(root->r, node);			
		} else {
			root->r = node;
		}

		recalculate_bf(root);

		if(root->bf >= 2) {
			if(node->key > root->r->key)
				root = rotate_rr(root);
			else
				root = rotate_rl(root);
		}
	} else if(node->key < root->key) {
		if(root->l) {
			root->l = add_node(root->l, node);
		} else {
			root->l = node;
		}

		recalculate_bf(root);

		if(root->bf <= -2) {
			if(node->key < root->l->key)
				root = rotate_ll(root);
			else
				root = rotate_lr(root);
		}
	} else {
		del_node(node);
	}
	
	assert(root->bf < 2 && root->bf > -2);

	return root;
}

static void print_spaces(struct node *node, int spaced) {
	int i = 0;

	for(; i<spaced-4; i++) putchar(' ');

	if(node)
		printf("%4d", node->key);
	else
		printf("    ");

	i = 0;

	for(; i<spaced; i++) putchar(' ');
}

// kod ponizej chyba byl najciekawsza rozkmina z tego wszystkiego :)
// czyt. rekurencyjne "splaszczanie" drzewa (do arraya), coby go wygodnie wyswietlic
static void flatten_tree(struct node *node, int level, struct node **node_array, int *node_array_pos) {
	node_array[ node_array_pos[level]++ ] = node;

	if(node == NULL)
		return;

	flatten_tree(node->l, level+1, node_array, node_array_pos);
	flatten_tree(node->r, level+1, node_array, node_array_pos);
}

static void print_tree(struct node *root) {
	assert(root != NULL);

	int tree_height = node_height(root)+1;
	int num_nodes = ((1<<(tree_height))-1);

	struct node **node_array = malloc(sizeof(struct node *) * num_nodes);
	int *node_array_pos = malloc(sizeof(int) * tree_height);

	if(!node_array || !node_array_pos)
		return;

	int i = 0;

	for(; i<tree_height; i++) {
		node_array_pos[i] = (1<<i) - 1;
	}

	// we won't fill in all the positions [children of null nodes] with flatten_tree()
	memset(node_array, 0, sizeof(struct node *) * num_nodes);

	flatten_tree(root, 0, node_array, node_array_pos);

	tree_height--;

	int max_height = (((1<<(tree_height)))*4) /2;
	struct node **node_array_ptr = node_array;

	for(i=0; i<tree_height; i++) {
		int on_this_level = (1<<i); // nodes

		while(on_this_level--) {
			print_spaces(*(node_array_ptr), max_height);

			node_array_ptr++;
		}

		max_height = max_height/2;

		printf("\n");
	}

	free(node_array);
	free(node_array_pos);
}

int main(int argc, char const *argv[]) {
	struct node *root = NULL;

	int entered_number;
	for(;;) {
		printf("> ");

		if(!scanf("%d", &entered_number))
			break;

		root = add_node(root, new_node(entered_number));

		print_tree(root);
	}

	del_node(root);

	return 0;
}
