#include "children_component.h"
#include <cassert>
#include <entt/entt.hpp>

namespace Engine {
	void add_child(entt::registry& entt_registry, entt::entity parent_entity, entt::entity child_entity) {
		assert(entt_registry.valid(parent_entity));
		assert(entt_registry.valid(child_entity));
		entt_registry.get_or_emplace<ParentComponent>(child_entity) = parent_entity;
		entt_registry.get_or_emplace<ChildrenComponent>(parent_entity).children.push_back(child_entity);
	}
	void _destroy_entity(entt::registry& entt_registry, entt::entity entity) {
		assert(entt_registry.valid(entity));
		auto* children = entt_registry.try_get<ChildrenComponent>(entity);
		if (children) {
			for (const auto& child : children->children) {
				_destroy_entity(entt_registry, child);
			}
		}
		entt_registry.destroy(entity);
	}

	void destroy_entity(entt::registry& entt_registry, entt::entity entity) {
		assert(entt_registry.valid(entity));
		auto* parent = entt_registry.try_get<ParentComponent>(entity);
		if (parent) {
			assert(entt_registry.valid(parent->entity));
			auto* children = entt_registry.try_get<ChildrenComponent>(parent->entity);
			assert(children && "the parent of this entity does not contain a ChildrenComponent");
			auto num_erased = std::erase(children->children, entity);
			assert(num_erased == 1);
		}
		_destroy_entity(entt_registry, entity);
	}
}