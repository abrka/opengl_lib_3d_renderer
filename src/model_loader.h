#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <memory>
#include <optional>

#include <GL3D/texture.h>

#include "stb_image_raii.h"

namespace AssetLoader {
	std::optional<GLenum> get_texture_format(unsigned int num_channels) {

		switch (num_channels)
		{
		case 3:
			return GL_RGB;
			break;
		case 4:
			return GL_RGBA;
			break;
		default:
			return std::nullopt;
			break;
		}
	};

	std::optional<GLenum> get_internal_texture_format(unsigned int num_channels) {

		switch (num_channels)
		{
		case 3:
			return GL_SRGB;
			break;
		case 4:
			return GL_SRGB_ALPHA;
			break;
		default:
			return std::nullopt;
			break;
		}
	}

	enum class TextureLoaderError {
		texture_file_doesnt_exist
	};
	tl::expected<std::unique_ptr<GL3D::Texture>, TextureLoaderError> load_texture_from_filepath(const std::filesystem::path path) {
		try {
			STBImageRAII stb_texture{ path };
			GL3D::TextureSpec texture_spec{};
			texture_spec.texture_format = get_texture_format(stb_texture.num_channels).value();
			texture_spec.internal_texture_format = get_internal_texture_format(stb_texture.num_channels).value();
			return std::make_unique<GL3D::Texture>( (unsigned int)stb_texture.width, (unsigned int)stb_texture.height, stb_texture.image_data, texture_spec );

		}
		catch (const std::exception&)
		{
			return tl::unexpected(TextureLoaderError::texture_file_doesnt_exist);
		}
	}

}