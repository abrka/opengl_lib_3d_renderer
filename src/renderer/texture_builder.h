#pragma once

#include <memory>
#include <filesystem>
#include <tl/expected.hpp>
#include <glad/glad.h>
#include <GL3D/texture.h>

namespace TextureBuilder {
	enum class TextureLoaderError {
		couldnt_get_texture_format_from_num_channels,
		couldnt_get_internal_texture_format_from_num_channels,
		texture_file_doesnt_exist
	};
	tl::expected<std::unique_ptr<GL3D::Texture>, TextureLoaderError> build(const std::filesystem::path path);

}