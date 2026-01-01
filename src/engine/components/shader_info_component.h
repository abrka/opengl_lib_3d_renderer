#pragma once

#include <string>
#include <filesystem>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

namespace Engine {
	const std::string default_vertex_shader = TOSTRING(ENGINE_ASSET_DIR) + std::string{ "/shaders/pbr_vertex.glsl" };
	const std::string default_fragment_shader = TOSTRING(ENGINE_ASSET_DIR) + std::string{ "/shaders/pbr_frag.glsl" };
	
	struct ShaderInfoComponent {
		std::filesystem::path vertex_filepath{default_vertex_shader};
		std::filesystem::path fragment_filepath{default_fragment_shader};
	};
}