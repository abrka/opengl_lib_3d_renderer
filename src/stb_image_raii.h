#pragma once

#include <span>
#include <iostream>
#include <exception>
#include <filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


class STBImageRAII {
public:
	int width{};
	int height{};
	int num_channels{};
	std::span<stbi_uc> image_data{};

	// throws std::runtime_error
	STBImageRAII(const std::filesystem::path path) {
		stbi_uc* image_data_ptr = stbi_load(path.string().c_str(), &width, &height, &num_channels, 0);
		if (!image_data_ptr) {
			throw std::runtime_error("no image file exists at specified filepath" );
		}
		size_t image_data_size = width * height;
		image_data = std::span<stbi_uc>{ image_data_ptr, image_data_size };
	}

	STBImageRAII(const STBImageRAII& rhs) = delete;

	STBImageRAII& operator=(const STBImageRAII& rhs) = delete;

	~STBImageRAII() {
		stbi_image_free(image_data.data());
	}

};