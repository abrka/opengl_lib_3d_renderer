#pragma once

#include <vector>
#include <entt/entt.hpp>
#include "parent_component.h"

namespace Engine {
	struct ChildrenComponent {
		std::vector<entt::entity> children{};
	};

	void add_child(entt::registry& entt_registry, entt::entity parent_entity, entt::entity child_entity){
		entt_registry.get_or_emplace<ParentComponent>(child_entity) = parent_entity;
		entt_registry.get_or_emplace<ChildrenComponent>(parent_entity).children.push_back(child_entity);
	}
	/**
	 * @brief private do not call
	 */
	void _destroy_entity(entt::registry& entt_registry, entt::entity entity) {
		auto& children = entt_registry.get<ChildrenComponent>(entity);
		for (const auto& child : children.children) {
			_destroy_entity(entt_registry, child);
		}
		entt_registry.destroy(entity);
	}

	void destroy_entity(entt::registry& entt_registry, entt::entity entity) {
		auto* parent = entt_registry.try_get<ParentComponent>(entity);
		if (parent) {
			auto* children = entt_registry.try_get<ChildrenComponent>(parent->entity);
			assert(children && "the parent of this entity does not contain a ChildrenComponent");
			auto num_erased = std::erase(children->children, entity);
			assert(num_erased == 1);
		}
		_destroy_entity(entt_registry, entity);
	}

}