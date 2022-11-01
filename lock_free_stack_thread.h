#pragma once
template<typename T>
class lock_free_stack
{
private:
	struct node
	{
		T data;
		node* next;

		node(T const& _data) :data(_data) {}
	};

	node* head;

public:
	void push(T const& _data)
	{
		node* const new_node = new node(data);
		new_node->next = head;
		head = new_node;
	}
};