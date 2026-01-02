#pragma once

#include <filesystem>

namespace Engine {
	struct ScriptInfoComponent {
		std::filesystem::path filepath{};
		bool requires_reload{ true };
	};
}