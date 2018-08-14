#include <stdlib.h>
#include "list.h"
#include "../util.h"

int 
popList(struct ListNode **head) {
	if (!head) die("popList: got a null arg\n");
	if (!*head) return 0;

	struct ListNode *tmp = (*head)->next;
	free(*head);
	*head = tmp;
	return 0;
}

int 
fpopList(struct ListNode **head) {
	if (!head) die("popList: got a null arg\n");
	if (!*head) return 0;

	struct ListNode *tmp = (*head)->next;
	free((*head)->data);
	free(*head);
	*head = tmp;
	return 0;
}

int
pushList(struct ListNode **head, void *data) {
	struct ListNode *new_node = malloc(sizeof(struct ListNode));
	if (!new_node || !head) die("pushList:");

	new_node->data = data;
	new_node->next = *head;
	*head = new_node;
	return 0;
}

