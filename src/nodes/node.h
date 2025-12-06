#pragma once

#include <memory>
#include <vector>

namespace Engine {
	class Node {
	public:
		Node* parent{};
		std::vector<std::unique_ptr<Node>> children{};

		virtual ~Node() = default;
	};
}