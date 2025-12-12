#pragma once

#include <vector>
#include <entt/entt.hpp>
#include "parent_component.h"

namespace Engine {
	using ChildrenComponent = std::vector<entt::entity>;

	void add_child(entt::registry& entt_registry, entt::entity parent_entity, entt::entity child_entity){
		entt_registry.get_or_emplace<ParentComponent>(child_entity) = parent_entity;
		entt_registry.get_or_emplace<ChildrenComponent>(parent_entity).push_back(child_entity);
	}
	/**
	 * @brief private do not call
	 */
	void _destroy_entity(entt::registry& entt_registry, entt::entity entity) {
		auto& children = entt_registry.get<ChildrenComponent>(entity);
		for (const auto& child : children) {
			_destroy_entity(entt_registry, child);
		}
		entt_registry.destroy(entity);
	}

	void destroy_entity(entt::registry& entt_registry, entt::entity entity) {
		auto& parent_opt = entt_registry.get<ParentComponent>(entity);
		if (parent_opt.has_value()) {
			auto& parent = parent_opt.value();
			auto& children = entt_registry.get<ChildrenComponent>(parent);
			auto num_erased = std::erase(children, entity);
			assert(num_erased == 1);
		}
		_destroy_entity(entt_registry, entity);
	}

}