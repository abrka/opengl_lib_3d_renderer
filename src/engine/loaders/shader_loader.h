#pragma once

#include <memory>
#include <filesystem>
#include <entt/entt.hpp>
#include "renderer/shader_builder.h"

namespace Engine {
	struct ShaderLoader {
		using result_type = std::shared_ptr<GL3D::ShaderProgram>;
		using error_type = ShaderBuilder::ShaderBuilderError;

	private:
		error_type error{};

	public:
		result_type operator()(std::filesystem::path fragment_shader_path, std::filesystem::path vertex_shader_path) {
			auto res = ShaderBuilder::build(fragment_shader_path, vertex_shader_path);
			if (!res.has_value()) {
				error = res.error();
				return result_type{};
			}
			return result_type(std::move(res.value()));
		}
		error_type get_last_error() {
			return error;
		}
	};
}