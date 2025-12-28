#include "shader_builder.h"

#include <memory>
#include <filesystem>

#include <tl/expected.hpp>

#include <GL3D/shader.h>
#include <GL3D/shader_program.h>
#include "utils.h"

namespace ShaderBuilder {
	tl::expected<std::unique_ptr<GL3D::Shader<GL3D::VertexShaderTag>>, ShaderBuilderError> build_vert(std::filesystem::path vertex_shader_filepath) {
		auto vert_shader_source_str = read_string_from_filepath(vertex_shader_filepath);
		if (!vert_shader_source_str.has_value()) {
			return tl::unexpected(ShaderBuilderError{ ShaderBuilderErrorWhich::vert_shader_file_not_found, "vertex shader file not found" });
		}
		std::unique_ptr<GL3D::Shader<GL3D::VertexShaderTag>> vertex_shader{};
		try
		{
			vertex_shader = std::make_unique<GL3D::Shader<GL3D::VertexShaderTag>>(vert_shader_source_str.value());
		}
		catch (const std::exception& e)
		{
			return tl::unexpected(ShaderBuilderError{ ShaderBuilderErrorWhich::vert_shader_compile_error, e.what() });
		}
		return vertex_shader;
	}

	tl::expected<std::unique_ptr<GL3D::Shader<GL3D::FragmentShaderTag>>, ShaderBuilderError> build_frag(std::filesystem::path frag_shader_filepath) {
		auto frag_shader_source_str = read_string_from_filepath(frag_shader_filepath);
		if (!frag_shader_source_str.has_value()) {
			return tl::unexpected(ShaderBuilderError{ ShaderBuilderErrorWhich::frag_shader_file_not_found, "fragment shader file not found" });
		}
		std::unique_ptr<GL3D::Shader<GL3D::FragmentShaderTag>> fragment_shader{};
		try
		{
			fragment_shader = std::make_unique<GL3D::Shader<GL3D::FragmentShaderTag>>(frag_shader_source_str.value());
		}
		catch (const std::exception& e)
		{
			return tl::unexpected(ShaderBuilder::ShaderBuilderError{ ShaderBuilderErrorWhich::frag_shader_compile_error, e.what() });
		}
		return fragment_shader;
	}


	tl::expected<std::unique_ptr<GL3D::ShaderProgram>, ShaderBuilderError> build(std::filesystem::path frag_shader_filepath, std::filesystem::path vertex_shader_filepath) {
		auto vertex_shader_result = build_vert(vertex_shader_filepath);
		if (!vertex_shader_result.has_value()) {
			return tl::unexpected(vertex_shader_result.error());
		}
		auto frag_shader_result = build_frag(frag_shader_filepath);
		if (!frag_shader_result.has_value()) {
			return tl::unexpected(frag_shader_result.error());
		}
		std::unique_ptr<GL3D::ShaderProgram> shader_program{};
		try {
			shader_program = std::make_unique<GL3D::ShaderProgram>(*vertex_shader_result.value(), *frag_shader_result.value());
		}
		catch (const std::exception& e)
		{
			return tl::unexpected(ShaderBuilderError{ ShaderBuilderErrorWhich::shader_link_error, e.what() });
		}
		return shader_program;
	}
}