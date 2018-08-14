#ifndef list_h_
#define list_h_

struct ListNode {
	void *data;
	struct ListNode *next;
};

int popList(struct ListNode **head);
int fpopList(struct ListNode **head); // call free() on data ptr as you pop
int pushList(struct ListNode **head, void *data);

#endif
