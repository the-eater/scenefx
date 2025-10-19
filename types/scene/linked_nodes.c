#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <wlr/util/log.h>

#include "types/linked_nodes.h"

struct link {
	struct linked_node *node_1;
	struct linked_node *node_2;
};

static struct link *link_init(struct linked_node *node_1,
		struct linked_node *node_2) {
	struct link *link = calloc(1, sizeof(*link));
	if (!link) {
		wlr_log(WLR_ERROR, "Failed to allocate link!");
		return NULL;
	}

	link->node_1 = node_1;
	link->node_2 = node_2;

	return link;
}

//
// Linked Node
//

static bool linked_nodes_are_linked(struct linked_node *node_1,
		struct linked_node *node_2) {
	return node_1->link != NULL && node_2->link != NULL
		&& node_1->link == node_2->link;
}

void linked_node_init_link(struct linked_node *node_1,
		struct linked_node *node_2) {
	if (node_1->link || node_2->link) {
		assert(linked_nodes_are_linked(node_1, node_2));
		linked_node_unlink(node_1, node_2);
	}

	struct link *link = link_init(node_1, node_2);
	node_1->link = link;
	node_2->link = link;
}

struct linked_node *linked_nodes_get_sibling(struct linked_node *node) {
	if (node->link == NULL) {
		return NULL;
	}
	assert(node == node->link->node_1 || node == node->link->node_2);

	struct link *link = node->link;
	if (link->node_1 == node) {
		return link->node_2;
	}
	return link->node_1;
}

void linked_node_unlink(struct linked_node *node_1,
		struct linked_node *node_2) {
	assert(linked_nodes_are_linked(node_1, node_2));

	struct link *link = node_1->link;
	node_1->link = NULL;
	node_2->link = NULL;
	free(link);
}

void linked_node_destroy(struct linked_node *node) {
	if (node->link == NULL) {
		return;
	}
	assert(node == node->link->node_1 || node == node->link->node_2);
	linked_node_unlink(node->link->node_1, node->link->node_2);
}

// Linked node list

void linked_node_list_init(struct linked_node_list *list) {
	wl_list_init(&list->list);
}

static struct linked_node_list_entry *linked_node_list_get_entry(struct linked_node_list *linked_list, struct linked_node* node)
{
	struct linked_node_list_entry *entry;
	wl_list_for_each(entry, &linked_list->list, link) {
		if (linked_nodes_are_linked(&entry->node, node)) {
			return entry;
		}
	}

	return NULL;
}

bool linked_node_list_is_linked(struct linked_node_list *linked_list, struct linked_node* node) {
	return NULL == linked_node_list_get_entry(linked_list, node);
}

void linked_node_list_init_link(struct linked_node_list *linked_list, struct linked_node* node) {
	if (linked_node_list_is_linked(linked_list, node)) {
		return;
	}

	assert(node->link == NULL);

	struct linked_node_list_entry *entry = calloc(1, sizeof(struct linked_node_list_entry));
	entry->node = linked_node_init();
	wl_list_insert(&linked_list->list, &entry->link);
	linked_node_init_link(&entry->node, node);
}

void linked_node_list_unlink(struct linked_node_list *linked_list, struct linked_node* node) {
	struct linked_node_list_entry *entry = linked_node_list_get_entry(linked_list, node);
	if (entry == NULL) {
		return;
	}

	linked_node_unlink(&entry->node, node);
	wl_list_remove(&entry->link);
	free(entry);
}

void linked_node_list_destroy(struct linked_node_list *linked_list) {
	struct linked_node_list_entry *entry;
	struct linked_node_list_entry *tmp;
	wl_list_for_each_safe(entry, tmp, &linked_list->list, link) {
		linked_node_destroy(&entry->node);
		wl_list_remove(&entry->link);
		free(entry);
	}

	free(linked_list);
}