#pragma once

#include <string>
#include <memory>
#include <vector>

namespace Engine {
	class Node {
	public:
		std::string name{"Default Name"};
		Node* parent{};
		std::vector<std::unique_ptr<Node>> children{};

		virtual ~Node() = default;
	};
}