#pragma once

#include "renderer/asset_builder.h"

namespace Engine {
	struct MaterialComponent {
		MaterialComponent() {
			material.set_uniform("u_material.specular_alpha", 32.0f);
			material.set_uniform("u_material.ambient_strength", 1.0f);
			material.set_uniform("u_material.diffuse_strength", 1.0f);
			material.set_uniform("u_material.specular_strength", 1.0f);
		}
		AssetBuilder::Material material{};
	};
}