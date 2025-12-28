#pragma once

#include <span>
#include <filesystem>

typedef unsigned char stbi_uc;


class STBImageRAII {
public:
	bool flip_image{ false };
	int width{};
	int height{};
	int num_channels{};
	std::span<stbi_uc> image_data{};

	// throws std::runtime_error
	STBImageRAII(const std::filesystem::path path);
	STBImageRAII(const STBImageRAII& rhs) = delete;
	STBImageRAII& operator=(const STBImageRAII& rhs) = delete;
	~STBImageRAII();

};
