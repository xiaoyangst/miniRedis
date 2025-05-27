/**
  ******************************************************************************
  * @file           : BSTTree.h
  * @author         : xy
  * @brief          : 非递归模板实现 BST 树（非线程安全）
  * @attention      : None
  * @date           : 2025/5/27
  ******************************************************************************
  */

#ifndef MINIREDIS_SRC_DATASTRUCT_BSTTREE_H_
#define MINIREDIS_SRC_DATASTRUCT_BSTTREE_H_

#include <iostream>
#include <algorithm>
#include <queue>
#include <stack>
#include <optional>

template<typename K, typename V>
class BSTTree {
  using Result = std::optional<V>;
  struct TreeNode {
	K key;
	V val;
	TreeNode *left;
	TreeNode *right;

	TreeNode(const K &key, const V &val) : key(key), val(val), left(nullptr), right(nullptr) {}
  };

 public:
  explicit BSTTree() = default;

  ~BSTTree() {
	  clear(root_);
	  root_ = nullptr;
  }

  void insert(const K &Key, const V &Val) {
	  root_ = insert(root_, Key, Val);
  }

  void remove(const K &Key) {
	  if (root_) {
		  root_ = remove(root_, Key);
	  }
  }

  Result search(const K &Key) const {
	  return search(root_, Key);
  }

  [[nodiscard]] size_t size() const {
	  return size_;
  }

  BSTTree(const BSTTree &) = delete;
  BSTTree &operator=(const BSTTree &) = delete;
 private:
  void clear(TreeNode *root) {
	  if (root == nullptr) return;

	  std::queue<TreeNode *> qe;
	  qe.push(root);

	  while (!qe.empty()) {
		  auto node = qe.front();
		  qe.pop();
		  if (node->left) qe.push(node->left);
		  if (node->right) qe.push(node->right);
		  delete node;
	  }
  }

  Result search(TreeNode *cur, const K &key) const {
	  while (cur) {
		  if (key < cur->key) {
			  cur = cur->left;
		  } else if (key > cur->key) {
			  cur = cur->right;
		  } else if (key == cur->key) {
			  return std::make_optional(cur->val);
		  }
	  }
	  return std::nullopt;
  }

  TreeNode *insert(TreeNode *root, const K &key, const V &val) {
	  if (root == nullptr) {
		  size_++;
		  return new TreeNode(key, val);
	  } // 至少有一个节点
	  auto cur = root;

	  // 找到插入位置并完成插入
	  while (true) {
		  if (key < cur->key) {
			  // 按理要从左边继续探索
			  // 如果当前节点没有左节点，可直接作为它的左节点
			  if (cur->left == nullptr) {
				  size_++;
				  auto new_node = new TreeNode(key, val);
				  cur->left = new_node;
				  break;
			  }
			  cur = cur->left;
		  } else if (key > cur->key) {
			  // 按理要从右边继续探索
			  // 如果当前节点没有右节点，可直接作为它的右节点
			  if (cur->right == nullptr) {
				  size_++;
				  auto new_node = new TreeNode(key, val);
				  cur->right = new_node;
				  break;
			  }
			  cur = cur->right;
		  } else if (key == cur->key) {    // update，这就是为什么不需要单独写一个 update
			  cur->val = val;
			  return root;
		  }
	  }
	  return root;
  }

  TreeNode *remove(TreeNode *root, K key) {
	  TreeNode *parent = nullptr;

	  // 找到删除的节点
	  auto cur = exist(root, key, parent);
	  if (!cur) return root;

	  auto remove_node = cur;

	  // 开始删除，先考虑存在左右节点的可能性，进行降级
	  if (cur->left && cur->right) {
		  // 找到中序后继节点
		  auto successor_parent = cur;
		  auto successor = cur->right;
		  while (successor->left) {
			  successor_parent = successor;
			  successor = successor->left;
		  }

		  // 交换信息
		  cur->key = successor->key;
		  cur->val = successor->val;

		  // 保存待删除节点，及其父节点
		  parent = successor_parent;
		  remove_node = successor;
	  }

	  // 记录待删除节点的孩子信息

	  auto children = remove_node->left ? remove_node->left : remove_node->right;

	  if (parent == nullptr) {      // 待删除节点是根节点
		  delete remove_node;
		  size_--;
		  return children;            // 返回新的根节点
	  } else {
		  if (parent->left == remove_node) {
			  parent->left = children;
		  } else {
			  parent->right = children;
		  }
		  delete remove_node;
		  size_--;
	  }
	  return root;	// 删除的不是根节点，那就返回当前根节点即可
  }

  TreeNode *exist(TreeNode *root, K key, TreeNode *&parent) {	// parent 记得传引用，否则容易犯错
	  parent = nullptr;
	  auto cur = root;
	  while (cur) {
		  if (key < cur->key) {
			  parent = cur;
			  cur = cur->left;
		  } else if (key > cur->key) {
			  parent = cur;
			  cur = cur->right;
		  } else if (key == cur->key) {
			  return cur;
		  }
	  }
	  return nullptr;
  }

 private:
  TreeNode *root_;
  size_t size_;

 public:    // 用于测试
  void inorderPrint(TreeNode *node) const {
	  if (!node) return;
	  inorderPrint(node->left);
	  std::cout << "(" << node->key << ":" << node->val << ") ";
	  inorderPrint(node->right);
  }

  void print() const {
	  inorderPrint(root_);
	  std::cout << std::endl;
  }
};

#endif //MINIREDIS_SRC_DATASTRUCT_BSTTREE_H_
