#pragma once

#include <functional>
#include <entt/entt.hpp>

namespace Engine {
	template<typename Archive>
	using snapshot_get_func = std::function<void(Archive&, entt::snapshot&)>;

	template<typename Archive>
	using snapshot_loader_get_func = std::function<void(Archive&, entt::snapshot_loader&)>;

	template<typename Archive>
	void serialize_entt(entt::registry& entt_registry, Archive& archive, snapshot_get_func<Archive> snapshot_get_fn) {
		entt::snapshot snapshot{ entt_registry };
		snapshot.get<entt::entity>(archive);
		snapshot_get_fn(archive, snapshot);
	}
	template<typename Archive>
	void deserialize_entt(entt::registry& entt_registry, Archive& archive, snapshot_loader_get_func<Archive> snapshot_loader_get_fn) {
		entt::snapshot_loader snapshot_loader{ entt_registry };
		snapshot_loader.get<entt::entity>(archive);
		snapshot_loader_get_fn(archive, snapshot_loader);
	}
}