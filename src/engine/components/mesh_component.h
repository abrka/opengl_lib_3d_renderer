#pragma once

#include <memory>
#include <entt/entt.hpp>
#include "renderer/mesh_builder.h"


namespace Engine {
	using MeshComponent = entt::resource<MeshBuilder::Scene>;
}