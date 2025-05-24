/**
  ******************************************************************************
  * @file           : AVL.h
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/5/23
  ******************************************************************************
  */

#ifndef MINIREDIS_SRC_DATASTRUCT_AVL_H_
#define MINIREDIS_SRC_DATASTRUCT_AVL_H_

#include <algorithm>
#include <queue>
#include <stack>
#include <optional>

template<typename K, typename V>
class AVLTree {
  using Result = std::optional<V>;
  struct TreeNode {
	K key;
	V val;
	TreeNode *left;
	TreeNode *right;
	int height;

	TreeNode(const K &key, const V &val) : key(key), val(val), left(nullptr), right(nullptr), height(1) {}
  };

 public:
  explicit AVLTree() = default;

  ~AVLTree() {
	  clear(root_);
	  root_ = nullptr;
  }

  void insert(const K &Key, const V &Val) {
	  root_ = insert(root_, Key, Val);
  }

  void remove(const K &Key) {
	  root_ = remove(root_, Key);
  }

  Result search(const K &Key) const {
	  return search(root_, Key);
  }

  [[nodiscard]] size_t size() const {
	  return size_;
  }

  AVLTree(const AVLTree &) = delete;
  AVLTree &operator=(const AVLTree &) = delete;

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

  int height(TreeNode *node) {
	  return node ? node->height : 0;
  }

  void updateHeight(TreeNode *node) {
	  node->height = std::max(height(node->left), height(node->right)) + 1;
  }

  int balanceFactor(TreeNode *node) {
	  return node ? height(node->left) - height(node->right) : 0;
  }

  TreeNode *leftRotate(TreeNode *cur) {
	  auto cur_right = cur->right;
	  auto cur_right_left = cur_right->left;

	  cur->right = cur_right_left;
	  cur_right->left = cur;

	  updateHeight(cur);
	  updateHeight(cur_right);

	  return cur_right;
  }

  TreeNode *rightRotate(TreeNode *cur) {
	  auto cur_left = cur->left;
	  auto cur_left_right = cur_left->right;

	  cur_left->right = cur;
	  cur->left = cur_left_right;

	  updateHeight(cur);
	  updateHeight(cur_left);

	  return cur_left;
  }

  TreeNode *balance(TreeNode *cur) {
	  updateHeight(cur);    // 确保是正确的高度信息，然后再去判断是否需要旋转
	  auto curBalanceFactor = balanceFactor(cur);

	  if (curBalanceFactor > 1) {
		  if (balanceFactor(cur->left) < 0) {
			  cur->left = leftRotate(cur->left);
		  }
		  return rightRotate(cur);
	  }

	  if (curBalanceFactor < -1) {
		  if (balanceFactor(cur->right) > 0) {
			  cur->right = rightRotate(cur->right);
		  }
		  return leftRotate(cur);
	  }

	  return cur;
  }

  TreeNode *insert(TreeNode *root, const K &key, const V &val) {
	  if (root == nullptr) {
		  size_++;
		  return new TreeNode(key, val);
	  } // 至少有一个节点

	  std::stack<TreeNode *> path;
	  auto cur = root;

	  // 找到插入位置并完成插入
	  while (true) {
		  path.push(cur);    // 记录走过的节点路径，这些节点会在插入一个节点之后而改变高度，进而引发是否失衡问题
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

	  // 回溯平衡节点
	  return backBalance(root, key, path);
  }

  TreeNode *remove(TreeNode *root, K key) {
	  if (!root) return nullptr;

	  std::stack<TreeNode *> path;
	  TreeNode *parent = nullptr;

	  // 找到目标节点和其父节点
	  auto cur = exist(root, key, path, parent);
	  if (!cur) return root;

	  auto remove_node = cur;

	  if (cur->left && cur->right) {
		  // 找中序后继
		  TreeNode *successor = cur->right;
		  TreeNode *successor_parent = cur;

		  while (successor->left) {
			  successor_parent = successor;
			  path.push(successor);		// 走的节点要加入到路径中
			  successor = successor->left;
		  }

		  // 替换当前节点内容
		  cur->key = successor->key;
		  cur->val = successor->val;

		  // 准备删除 successor
		  remove_node = successor;
		  parent = successor_parent;
	  }

	  // 已经从两个节点降级为处理单个节点
	  // 为什么优先考虑它的左节点？
	  TreeNode *child = remove_node->left ? remove_node->left : remove_node->right;

	  if (parent == nullptr) { // 删除的是根节点，没有父节点的节点就是根节点
		  delete remove_node;
		  size_--;
		  return child;
	  } else {
		  if (parent->left == remove_node) {
			  parent->left = child;
		  } else {
			  parent->right = child;
		  }
		  delete remove_node;
		  size_--;
	  }

	  // 回溯平衡路径
	  return backBalance(root, key, path);
  }

  // 由于要实际修改 parent，选择传递引用
  // 二级指针也可以，但是操作其他不美观且容易犯错
  TreeNode *exist(TreeNode *node, const K &key, std::stack<TreeNode *> &path, TreeNode *&parent) {
	  parent = nullptr;
	  while (node) {
		  path.push(node);
		  if (key < node->key) {
			  parent = node;
			  node = node->left;
		  } else if (key > node->key) {
			  parent = node;
			  node = node->right;
		  } else {
			  return node;
		  }
	  }
	  return nullptr;
  }

  TreeNode *minnum(TreeNode *node) {
	  while (node->left != nullptr) {
		  node = node->left;
	  }
	  return node;
  }

  TreeNode *backBalance(TreeNode *root, K key, std::stack<TreeNode *> &path) {
	  while (!path.empty()) {
		  auto cur = path.top();
		  path.pop();

		  // 平衡当前节点，会导致当前节点的位置和其他节点的位置发生变化
		  // 平衡的任务完成之后，还有一件事情要去处理，那就是 最新的 cur 究竟应该作为其父节点的左孩子，还是右孩子
		  // 如果没有父亲节点，那就不用考虑前面的问题
		  cur = balance(cur);

		  if (!path.empty()) {    // 如果不是根节点，就要调整，因为旋转导致节点位置变化，对应地值也发生变化
			  auto parent = path.top();
			  if (key < parent->key) {
				  parent->left = cur;
			  } else {
				  parent->right = cur;
			  }
		  } else {
			  root = cur;
		  }
	  }
	  return root;
  }

 private:
  TreeNode *root_ = nullptr;
  size_t size_ = 0;

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

#endif //MINIREDIS_SRC_DATASTRUCT_AVL_H_
