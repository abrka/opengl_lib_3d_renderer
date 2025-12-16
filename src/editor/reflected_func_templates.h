#pragma once

#include <entt/entt.hpp>
#include <imgui.h>
#include <ImReflect.hpp>

namespace Editor {
	template<typename T>
	T* get_component_from_entity(entt::registry* reg, entt::entity entity) {
		assert(reg);
		return reg->try_get<T>(entity);
	}
	template<typename T>
	void render_imgui_for_component(const char* name, T* comp) {
		if (!comp) {
			return;
		}
		const char* output_name = name ? name : "No name registered in entt::meta system for component";
		ImReflect::Input(output_name, comp);
	}
}