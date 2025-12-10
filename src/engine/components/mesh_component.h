#pragma once

#include <memory>
#include "renderer/mesh_builder.h"


namespace Engine {
	using MeshComponent = std::shared_ptr<MeshBuilder::Scene>;
}