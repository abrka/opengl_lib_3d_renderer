#pragma once

#include <memory>
#include <entt/entt.hpp>
#include "renderer/asset_builder.h"


namespace Engine {
	using MeshComponent = entt::resource<AssetBuilder::Scene>;
}