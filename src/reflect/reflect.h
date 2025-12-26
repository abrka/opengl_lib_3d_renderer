#pragma once

#include <sol/sol.hpp>
#include <entt/entt.hpp>
#include <imgui.h>
#include <ImReflect.hpp>

namespace Reflect {
	constexpr entt::hashed_string get_component_func_name{ "get_component" };
	constexpr entt::hashed_string add_component_func_name{ "add_component" };
	constexpr entt::hashed_string sol_add_component_func_name{ "sol_add_component" };
	constexpr entt::hashed_string render_imgui_for_component_func_name{ "render_imgui_for_component" };

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
	void sol_add_component(sol::table* sol_module, T* component, const char* component_name) {
		assert(sol_module);
		assert(component_name);
		if (!component) {
			return;
		}
		sol::table components_table = (*sol_module)["components"];
		components_table[component_name] = component;
	}

	template<typename T>
	void register_component(const char* name) {
		using namespace entt::literals;
		entt::meta_factory<T>()
			.type(name)
			.template func<&get_component_from_entity<T>>(get_component_func_name)
			.template func<&render_imgui_for_component<T>>(render_imgui_for_component_func_name)
			.template func<&add_component_to_entity<T>>(add_component_func_name)
			.template func<&sol_add_component<T>>(sol_add_component_func_name);
	}
}