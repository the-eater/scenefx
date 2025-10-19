#ifndef TYPES_LINKED_NODES_H
#define TYPES_LINKED_NODES_H
#include <wayland-util.h>

/**
 * A node in a link between two objects. Can be used to safely couple two
 * objects together.
 */
struct linked_node {
	struct link *link;
};

/**
 * A list of nodes linked between two objects. Can be used to safely couple 1 object to a list of objects
 *
 */
struct linked_node_list {
	struct wl_list list;
};

/**
 * The list entry for the linked_node_list
 */
struct linked_node_list_entry {
	struct wl_list link;
	struct linked_node node;
};

#define linked_node_init() \
	((struct linked_node) { \
		.link = NULL \
	})

void linked_node_init_link(struct linked_node *main_node,
		struct linked_node *reference_node);

struct linked_node *linked_nodes_get_sibling(struct linked_node *node);

void linked_node_unlink(struct linked_node *main_node,
		struct linked_node *reference_node);

void linked_node_destroy(struct linked_node *node);

void linked_node_list_init(struct linked_node_list *list);

bool linked_node_list_is_linked(struct linked_node_list *linked_list, struct linked_node* node);

void linked_node_list_init_link(struct linked_node_list *linked_list, struct linked_node* node);

void linked_node_list_unlink(struct linked_node_list *linked_list, struct linked_node* node);

void linked_node_list_destroy(struct linked_node_list *linked_list);

#endif
