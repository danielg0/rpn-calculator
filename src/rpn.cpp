// rpn.cpp - implement the rpn functions

#include "rpn.hpp"

#include <algorithm>
#include <cmath>
#include <memory>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <vector>

namespace RPN {
	namespace {
		// Private helper functions

		// Array of all possible operators
		std::vector<std::string> operators {
			"+",
			"-",
			"*",
			"/",
			"^"
		};

		// A struct that represents a lead in a binary tree
		struct RPNTree {
			// Pointers to the left and right nodes
			std::shared_ptr<RPNTree> left;
			std::shared_ptr<RPNTree> right;

			// Weak pointer to this node's parent
			// So child doesn't own parent and cause memory leak			
			std::weak_ptr<RPNTree> parent;

			// String representing the number or operation this node represents
			std::string data;

			// Constructor for the RPN tree
			RPNTree(std::string data) : left(nullptr), right(nullptr),
				parent(), data(data) {}
		};

		// Get parts
		// Split a given string by spaces, returning a vector
		std::vector<std::string> Split(std::string expression) {
			std::string buffer = "";
			std::vector<std::string> results;

			// Iterate through each character in the string
			for(auto ch : expression) {
				// If space, push buffer to vector
				if(ch == ' ' && buffer != "") {
					results.push_back(buffer);
					buffer = "";
				}
				// Else, push char to buffer
				else if (ch != ' ') {
					buffer += ch;
				}
			}

			// Ensure buffer is emptyed
			if(buffer != "") {
				results.push_back(buffer);
			}

			return results;
		}

		// Perform an inorder traversal on a given RPNTree
		void Inorder(std::shared_ptr<RPNTree> tree,
			std::shared_ptr<std::string> results) {
			// Traverse left tree if not null
			if(tree->left != nullptr) {
				*results += "(";
				Inorder(tree->left, results);
				*results += ")";
			}

			*results += tree->data + " ";

			if(tree->right != nullptr) {
				*results += "(";
				Inorder(tree->right, results);
				*results += ")";
			}
		}
	}

	// Get RPN statement from infix expression
	std::string FromInfix(std::string infix);

	// Evaluate an RPN statement
	double Evaluate(std::string string) {
		// Check string is not null
		if(string == "") {
			throw std::invalid_argument("string passed is null");
		}

		// Split the string into it's parts
		auto rpn = Split(string);

		// Check that values were found
		if(rpn.size() == 0) {
			throw std::invalid_argument("string passed contained no symbols");
		}

		// Create a stack to hold the values being processed
		std::stack<double> stack;

		// Iterate through the seperate parts of the expression
		for(auto exp : rpn) {
			// Get location of exp in op list
			auto pos = std::find(operators.begin(), operators.end(), exp);

			// Check if expression is an operator
			if(pos != operators.end()) {
				// Check there are at least two numbers on stack
				if(stack.size() < 2) {
					// Throw exception
					throw std::invalid_argument("operation attempted without at least 2 operands at operator: " + exp);
				}

				// Pop the two operands off the stack - note the order!!
				auto b = stack.top();
				stack.pop();
				auto a = stack.top();
				stack.pop();

				double result;

				// Switch based on chosen operator
				switch(pos - operators.begin()) {
					case 0: result = a + b; break;
					case 1: result = a - b; break;
					case 2: result = a * b; break;
					case 3: result = a / b; break;
					case 4: result = std::pow(a, b); break;
					default: throw std::runtime_error("operator " + exp + " not implemented");
				}

				stack.push(result);
			}
			// Else, attempt to convert to double and push to stack
			else {
				try {
					stack.push(std::stod(exp));
				}
				catch(const std::invalid_argument& e) {
					throw std::invalid_argument("value in rpn statement that is neither a number nor an operator at: " + exp);
				}
				catch(const std::out_of_range& e) {
					throw std::invalid_argument("value in statement too large");
				}
			}
		}

		// Return the number on top of the stack
		return stack.top();
	}

	// Convert RPN statement to infix expression
	std::string ToInfix(std::string string) {
		// Seperate out the parts of the rpn expression
		auto rpn = Split(string);

		// Check that expression contained something
		if(rpn.size() == 0) {
			throw std::invalid_argument("string passed contained no symbols");
		}

		// Reverse the expression
		std::reverse(rpn.begin(), rpn.end());

		// Hold a pointer to the top of the tree so that it doesn't get deleted
		// Create another pointer tree, which indicates at what point in the
		// tree the program currently is
		auto root = std::make_shared<RPNTree>(rpn[0]);
		auto tree = root;

		// Remove the first element of the expression (already created)
		rpn.erase(rpn.begin());

		// Produce a tree representing the rpn tree, prioritising right subtrees
		// over left subtrees
		for(auto exp : rpn) {
			// Ensure there is space to place a node
			// If not, move up the tree
			while(tree != nullptr && tree->right != nullptr && tree->left != nullptr) {
				tree = tree->parent.lock();
			}

			// If there were no free branches in the rpn tree, this rpn
			// expression is invalid
			if(tree == nullptr) {
				throw std::invalid_argument("invalid rpn expression at: " + exp);
			}

			// If character is an operator
			if(std::find(operators.begin(), operators.end(), exp) != operators.end()) {
				// Insert in availible place and then move to new subtree
				if(tree->right == nullptr) {
					tree->right = std::make_shared<RPNTree>(exp);
					tree->right->parent = tree;
					tree = tree->right;
				}
				else if(tree->left == nullptr) {
					tree->left = std::make_shared<RPNTree>(exp);
					tree->left->parent = tree;
					tree = tree->left;
				}
			}
			// Else, presume exp is a number and place in free position beneath
			// the tree node
			else {
				if(tree->right == nullptr) {
					tree->right = std::make_shared<RPNTree>(exp);
					tree->right->parent = tree;
				}
				else if(tree->left == nullptr) {
					tree->left = std::make_shared<RPNTree>(exp);
					tree->left->parent = tree;
				
				}
			}
		}

		// Perform inorder tree traversal
		auto results = std::make_shared<std::string>("");
		Inorder(root, results);

		return *results;
	}
}
