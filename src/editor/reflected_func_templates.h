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
	template<typename T>
	void add_component_to_entity(entt::registry* reg, entt::entity entity) {
		assert(reg);
		if (reg->all_of<T>(entity)) {
			return;
		}
		reg->emplace<T>(entity);
	}

	template<typename T>
	void register_component(const char* name) {
		using namespace entt::literals;
		entt::meta_factory<T>()
			.type(name)
			.template func<&get_component_from_entity<T>>("get_component"_hs)
			.template func<&render_imgui_for_component<T>>("render_imgui_for_component"_hs)
			.template func<&add_component_to_entity<T>>("add_component"_hs);
	}
}