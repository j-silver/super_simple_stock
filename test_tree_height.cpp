//
// Created by giuseppe on 17/04/19.
//

#include "tree_height.cpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>


class TestTreeHeight : public ::testing::Test {
protected:
	Node<int> root {0, {}};
	Node<int> child1 {1, {}};
	Node<int> child2 {2, {}};
	Node<int> gchild11 {-2, {}};
	Node<int> gchild12 {3, {}};
	Node<int> gchild21 {100, {}};
	Node<int> gchild22 {12, {}};
};


TEST_F(TestTreeHeight, add_one_child)
{
	root.add_child(&child1);
	ASSERT_EQ(root.height(), 1);
}


TEST_F(TestTreeHeight, add_two_children)
{
	root.add_child(&child1).add_child(&child2);
	ASSERT_EQ(root.height(), 1);
}


TEST_F(TestTreeHeight, add_grand_children)
{
	child1.add_child(&gchild11).add_child(&gchild12);
	child2.add_child(&gchild21).add_child(&gchild22);
	root.add_child(&child1).add_child(&child2);
	ASSERT_EQ(root.height(), 2);
}

TEST_F(TestTreeHeight, create_tree)
{
	child1.add_child(&gchild11).add_child(&gchild12);
	child2.add_child(&gchild21).add_child(&gchild22);
	std::unordered_multiset<Node<int>*> tree {&child1, &child2};
	Node<int> anotherRoot {12, tree};
	EXPECT_EQ(anotherRoot.height(), 2);
	gchild12.add_child(new Node<int>(-1000, {}));
	gchild21.add_child(new Node<int>(-999, {}));
	gchild21.add_child(new Node<int>(-7, {}));
	ASSERT_EQ(anotherRoot.height(), 3);
}


int main()
{
	::testing::InitGoogleTest();
	return RUN_ALL_TESTS();
}
