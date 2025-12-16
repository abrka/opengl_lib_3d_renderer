#pragma once

#include <functional>
#include <entt/entt.hpp>

namespace Engine {
	template<typename Archive, typename Snapshot>
	using snapshot_get_func = std::function<void(Archive&, Snapshot&)>;

	template<typename Archive>
	void serialize_entt(entt::registry& entt_registry, Archive& archive, snapshot_get_func<Archive, entt::snapshot> snapshot_get_fn) {
		entt::snapshot snapshot{ entt_registry };
		snapshot.get<entt::entity>(archive);
		snapshot_get_fn(archive, snapshot);
	}
	template<typename Archive>
	void deserialize_entt(entt::registry& entt_registry, Archive& archive, snapshot_get_func<Archive, entt::snapshot_loader> snapshot_get_fn) {
		entt::snapshot_loader snapshot_loader{ entt_registry };
		snapshot_loader.get<entt::entity>(archive);
		snapshot_get_fn(archive, snapshot_loader);
	}
}