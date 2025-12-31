#include "shader_loader.h"

namespace Engine {
	ShaderLoader::error_type ShaderLoader::error{};

	ShaderLoader::result_type ShaderLoader::operator()(std::filesystem::path fragment_shader_path, std::filesystem::path vertex_shader_path) {
		auto res = ShaderBuilder::build(fragment_shader_path, vertex_shader_path);
		if (!res.has_value()) {
			error = res.error();
			return result_type{};
		}
		return result_type(std::move(res.value()));
	}
	ShaderLoader::error_type ShaderLoader::get_last_error() {
		return error;
	}
}