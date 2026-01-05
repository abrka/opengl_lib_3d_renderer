#include "reflect.h"
#include "imreflect_custom_types.h"
#include "input/input.h"

namespace Reflect {
	void register_all_components()
	{
		register_component<Engine::NameComponent>("NameComponent");
		register_component<Engine::TransformComponent>("TransformComponent");
		register_component<Engine::PointLightComponent>("PointLightComponent");
		register_component<Engine::CameraComponent>("CameraComponent");
		register_component<Engine::MaterialComponent>("MaterialComponent");
		register_component<Engine::MeshInfoComponent>("MeshInfoComponent");
		register_component<Engine::ShaderInfoComponent>("ShaderInfoComponent");
		register_component<Engine::ScriptInfoComponent>("ScriptInfoComponent");
		register_component<Engine::PhysicsBodyInfoComponent>("PhysicsBodyInfoComponent");
	}
}