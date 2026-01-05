#pragma once

#include "engine/components/components.h"

namespace Engine {
	template<typename Archive, typename Snapshot>
	void snapshot_get_func_engine_types(Archive& archive, Snapshot& snapshot) {
		snapshot.template get<Engine::RootComponent>(archive);
		snapshot.template get<Engine::ParentComponent>(archive);
		snapshot.template get<Engine::ChildrenComponent>(archive);
		snapshot.template get<Engine::NameComponent>(archive);
		snapshot.template get<Engine::TransformComponent>(archive);
		snapshot.template get<Engine::PointLightComponent>(archive);
		snapshot.template get<Engine::CameraComponent>(archive);
		snapshot.template get<Engine::MeshInfoComponent>(archive);
		snapshot.template get<Engine::ShaderInfoComponent>(archive);
		snapshot.template get<Engine::ScriptInfoComponent>(archive);
		snapshot.template get<Engine::MaterialComponent>(archive);
		snapshot.template get<Engine::PhysicsBodyInfoComponent>(archive);
	}
}