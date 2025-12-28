#include "stb_image_raii.h"

#include <iostream>
#include <exception>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


STBImageRAII::STBImageRAII(const std::filesystem::path path) {
	stbi_set_flip_vertically_on_load(flip_image);
	stbi_uc* image_data_ptr = stbi_load(path.string().c_str(), &width, &height, &num_channels, 0);
	if (!image_data_ptr) {
		throw std::runtime_error("no image file exists at specified filepath");
	}
	size_t image_data_size = width * height;
	image_data = std::span<stbi_uc>{ image_data_ptr, image_data_size };
}
STBImageRAII::~STBImageRAII() {
	stbi_image_free(image_data.data());
}
