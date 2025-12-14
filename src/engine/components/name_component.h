#pragma once

#include <string>

#include <ImReflect.hpp>

namespace Engine {
	struct NameComponent {
		std::string name{};
	};
}
IMGUI_REFLECT(Engine::NameComponent, name)