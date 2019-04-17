//
// Created by giuseppe on 17/04/19.
//
#include <unordered_set>


template<typename N>
class cmp {
public:
	cmp() = default;

	bool operator()(N* n1, N* n2)
	{
		return n1 < n2;
	}
};


template<typename T>
class Node {
	T n;
	std::unordered_multiset<Node*> children;

	size_t visit_children(Node* p);
public:
	Node() : n {nullptr}, children {}
	{}

	Node(T const& t, std::unordered_multiset<Node*> const& c) :
			n {t}, children {c}
	{}

	T get_n() const
	{
		return n;
	}

	Node& add_child(Node* c);

	size_t height();
};


template<typename T>
size_t Node<T>::height()
{
	return visit_children(this);
}


template<typename T>
size_t Node<T>::visit_children(Node* p)
{
	size_t h {0};
	size_t hTemp {0};

	if (!(p->children).empty()) {
		++h;
		for (auto c : p->children)
			hTemp = std::max(hTemp, visit_children(c));
	}

	return h + hTemp;
}


template<typename T>
Node<T>& Node<T>::add_child(Node* c)
{
	children.insert(c);
	return *this;
}
