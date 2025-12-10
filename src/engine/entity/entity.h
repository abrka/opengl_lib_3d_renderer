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
		entt::entity entt_entity{};
	public:
		Entity(entt::registry& entt_registry): entt_registry(&entt_registry) {
			entt_entity = entt_registry.create();
		}
		template<typename Type, typename... Args>
		void add_component(Args &&...args) {
			entt_registry->emplace<Type>(entt_entity, std::forward<Args>(args)...);
		}
		template<typename Type>
		auto try_get_component() {
			return entt_registry->try_get<Type>(entt_entity);
		}
		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;
		~Entity() {
			entt_registry->destroy(entt_entity);
		}
	};
}