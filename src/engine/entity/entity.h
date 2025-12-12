#pragma once

#include <string>
#include <memory>
#include <vector>

#include <entt/entt.hpp>


namespace Engine {
	class Entity {
	public:
		std::string name{};
		Entity* parent{};
		std::vector<std::unique_ptr<Entity>> children{};
	private:
		entt::registry* entt_registry{};
		entt::entity entt_entity{entt::null};
	public:
		Entity(entt::registry& entt_registry): entt_registry(&entt_registry) {
			entt_entity = entt_registry.create();
		}

		void add_child(std::unique_ptr<Entity> child) {
			child->parent = this;
			children.push_back(std::move(child));
		}
		template<typename Type, typename... Args>
		void add_component(Args &&...args) {
			entt_registry->emplace<Type>(entt_entity, std::forward<Args>(args)...);
		}
		template<typename Type>
		auto try_get_component() {
			return entt_registry->try_get<Type>(entt_entity);
		}
		bool is_root() {
			return (parent == nullptr);
		}
		// returns if entity was successfully destroyed
		bool destroy() {
			if (is_root()) {
				return false;
			}
			parent->children.erase(
				std::remove_if(parent->children.begin(), parent->children.end(),
					[this](auto& entity) {
						return entity.get() == this;
					}), parent->children.end());

			return true;
		}
		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;
		~Entity() {
			entt_registry->destroy(entt_entity);
		}
	};
}