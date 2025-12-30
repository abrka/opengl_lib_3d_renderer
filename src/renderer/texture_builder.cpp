#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <memory>
#include <optional>

#include <glad/glad.h>

#include "stb_image_raii.h"
#include "texture_builder.h"

namespace TextureBuilder {
	std::optional<GLenum> get_texture_format(unsigned int num_channels) {

		switch (num_channels)
		{
		case 1:
			return GL_RED;
			break;
		case 2:
			return GL_RG;
			break;
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
		case 1:
			return GL_RED;
			break;
		case 2:
			return GL_RG;
			break;
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
	}
	std::string texture_loader_error_to_string(TextureLoaderError err)
	{
		switch (err)
		{
		case TextureLoaderError::couldnt_get_texture_format_from_num_channels:
			return "Invalid number of channels in texture file";
			break;
		case TextureLoaderError::couldnt_get_internal_texture_format_from_num_channels:
			return "Invalid number of channels in texture file (internal format)";
			break;
		case TextureLoaderError::texture_file_doesnt_exist:
			return "File doesn't exist";
			break;
		default:
			return "Unknown error";
			break;
		}
	}
	tl::expected<std::unique_ptr<GL3D::Texture>, TextureLoaderError> build(const std::filesystem::path path) {
		try {
			STBImageRAII stb_texture{ path };
			GL3D::TextureSpec texture_spec{};

			auto texture_format_res = get_texture_format(stb_texture.num_channels);
			if (!texture_format_res.has_value()) {
				return tl::make_unexpected(TextureLoaderError::couldnt_get_texture_format_from_num_channels);
			}
			texture_spec.texture_format = texture_format_res.value();

			auto internal_texture_format_res = get_internal_texture_format(stb_texture.num_channels);
			if (!internal_texture_format_res.has_value()) {
				return tl::make_unexpected(TextureLoaderError::couldnt_get_internal_texture_format_from_num_channels);
			}
			texture_spec.internal_texture_format = internal_texture_format_res.value();

			return std::make_unique<GL3D::Texture>((unsigned int)stb_texture.width, (unsigned int)stb_texture.height, stb_texture.image_data, texture_spec);

		}
		catch (const std::exception&)
		{
			return tl::make_unexpected(TextureLoaderError::texture_file_doesnt_exist);
		}
	}
}