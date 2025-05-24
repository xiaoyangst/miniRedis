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

  Result search(const K &Key) {
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

  Result search(TreeNode *cur, const K &key) {
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

  TreeNode *insert(TreeNode *root, K key, V val) {
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
	  return backBalance(root, nullptr, key, path);
  }

  TreeNode *remove(TreeNode *root, K key) {
	  if (!root) return nullptr;

	  std::stack<TreeNode *> path;
	  TreeNode *parent = nullptr;

	  // 找到要删除的节点
	  auto cur = exist(root, key, path);
	  if (cur == nullptr) return root;    // 表明 节点 就不存在

	  // 删除节点
	  auto remove_node = cur;
	  if (cur->left && cur->right) {    // 优先处理存在左右节点的，降级处理
		  // 找中序后继节点，记录路径
		  auto successor = cur->right;
		  auto successor_parent = cur;
		  path.push(cur);

		  while (successor->left) {
			  successor_parent = successor;
			  path.push(successor);
			  successor = successor->left;
		  }

		  // 替换
		  cur->key = successor->key;
		  cur->val = successor->val;

		  // 删除
		  remove_node = successor;
		  parent = successor_parent;
		  cur = successor;
	  } else {
		  parent = path.empty() ? nullptr : path.top();
	  }

	  auto children = remove_node->left ? remove_node->left : remove_node->right;
	  if (!parent) {
		  delete remove_node;
		  return children;
	  } else {
		  if (parent->left == remove_node) {
			  parent->left = children;
		  } else {
			  parent->right = children;
		  }
		  delete remove_node;
	  }

	  size_--;

	  // 回溯来平衡节点
	  return backBalance(root, children, key, path);
  }

  TreeNode *exist(TreeNode *target, K key, std::stack<TreeNode *> &path) {
	  while (target) {
		  path.push(target);
		  if (key < target->key) {
			  target = target->left;
		  } else if (key > target->key) {
			  target = target->right;
		  } else if (key == target->key) {    // 已找到，退出循环
			  break;
		  }
	  }

	  return target;
  }

  TreeNode *minnum(TreeNode *node) {
	  while (node->left != nullptr) {
		  node = node->left;
	  }
	  return node;
  }

  TreeNode *backBalance(TreeNode *root, TreeNode *children, K key, std::stack<TreeNode *> &path) {
	  while (!path.empty()) {
		  auto node = path.top();
		  path.pop();

		  node = balance(node);
		  updateHeight(node);    // 虽然 balance 里做了，保险再做一次也行

		  if (!path.empty()) {    // 如果不是根节点，就要调整，因为旋转导致节点变化，值也发生变化
			  auto parent = path.top();

			  if (parent->left == children
				  || (!children && key < parent->key)) {
				  parent->left = node;
			  } else {
				  parent->right = node;
			  }
		  } else {
			  root = node;
		  }

		  children = node;
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
