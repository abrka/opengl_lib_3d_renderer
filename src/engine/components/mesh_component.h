#pragma once

#include <filesystem>
#include <entt/fwd.hpp>
#include <entt/resource/resource.hpp>
#include "renderer/asset_builder.h"

namespace Engine {
	struct MeshComponent {
		entt::resource<AssetBuilder::Scene> scene{};
	};
}