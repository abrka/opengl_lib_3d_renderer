#pragma once

#include <memory>
#include <filesystem>
#include <entt/entt.hpp>
#include "renderer/asset_builder.h"

namespace Engine {
	struct MeshLoader {
		using result_type = std::shared_ptr<AssetBuilder::Scene>;
		using error_type = std::string;

	private:
		static error_type error;

	public:
		result_type operator()(std::filesystem::path path);
		static error_type get_last_error();
	};
}