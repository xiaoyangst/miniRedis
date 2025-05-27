#include <iostream>
#include "BSTTree.h"

// 假设你的 BSTTree 已经定义好了，包含一个打印函数

void testBSTTree() {
	BSTTree<int, std::string> tree{};

	std::cout << "插入节点:\n";
	tree.insert(10, "ten");
	tree.insert(20, "twenty");
	tree.insert(30, "thirty");
	tree.insert(40, "forty");
	tree.insert(50, "fifty");
	tree.insert(25, "twenty-five");

	std::cout << "中序遍历(应排序): ";
	tree.print();  // 需要你自己实现 print 和 inorder 遍历辅助函数，或者下面我帮你写

	std::cout << "树的大小: " << tree.size() << "\n";

	std::cout << "查找 key=20: ";
	auto result = tree.search(20);
	if (result) {
		std::cout << result.value() << "\n";
	} else {
		std::cout << "未找到\n";
	}

	std::cout << "删除 key=20\n";
	tree.remove(20);

	std::cout << "中序遍历(删除后): ";
	tree.print();

	std::cout << "树的大小: " << tree.size() << "\n";

	std::cout << "查找 key=20: ";
	result = tree.search(20);
	if (result) {
		std::cout << result.value() << "\n";
	} else {
		std::cout << "未找到\n";
	}


	tree.insert(25, "repeat twenty-five");
	tree.print();
}



int main() {

	testBSTTree();

	return 0;
}