#pragma once

#include <memory>
#include <filesystem>
#include <tl/expected.hpp>
#include <GL3D/shader.h>
#include <GL3D/shader_program.h>

namespace ShaderBuilder {
	enum class ShaderBuilderErrorWhich {
		frag_shader_compile_error,
		vert_shader_compile_error,
		shader_link_error,
		frag_shader_file_not_found,
		vert_shader_file_not_found
	};
	struct ShaderBuilderError {
		ShaderBuilderErrorWhich err{};
		std::string err_msg{};
	};

	tl::expected<std::unique_ptr<GL3D::Shader<GL3D::VertexShaderTag>>, ShaderBuilderError> build_vert(std::filesystem::path vertex_shader_filepath);
	tl::expected<std::unique_ptr<GL3D::Shader<GL3D::FragmentShaderTag>>, ShaderBuilderError> build_frag(std::filesystem::path frag_shader_filepath);
	tl::expected<std::unique_ptr<GL3D::ShaderProgram>, ShaderBuilderError> build(std::filesystem::path frag_shader_filepath, std::filesystem::path vertex_shader_filepath);
};
