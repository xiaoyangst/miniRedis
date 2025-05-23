#include <iostream>
#include <vector>
#include <stack>
using namespace std;

struct TreeNode {
  int val;
  TreeNode *left;
  TreeNode *right;

  TreeNode() : val(0), left(nullptr), right(nullptr) {}

  TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}

  TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Solution {
 public:
  vector<int> inorderTraversal(TreeNode* root) {
	  vector<int> result;
	  stack<TreeNode*> sk;
	  auto cur = root;

	  while (cur || !sk.empty()){
		  while (cur){
			  sk.push(cur);
			  cur = cur->left;
		  }

		  while (!sk.empty()){
			  auto node = sk.top();
			  result.push_back(node->val);
			  sk.pop();

			  if (node->right){
				  cur = node->right;
				  break;
			  }
		  }
	  }

	  return result;
  }
};

int main() {
	std::cout << "Hello, World!" << std::endl;
	return 0;
}
