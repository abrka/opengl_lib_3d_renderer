#pragma once

#include <functional>
#include <istream>
#include <ostream>

#include <entt/entt.hpp>

namespace Engine {
	template<typename InputArchive, typename OutputArchive>
	struct EnttRegistrySerializer {
		using snapshot_get_func = std::function<void(OutputArchive&, entt::snapshot&)>;
		snapshot_get_func snapshot_get_fn{};

		using snapshot_loader_get_func = std::function<void(InputArchive&, entt::snapshot_loader&)>;
		snapshot_loader_get_func snapshot_loader_get_fn{};

		void save(entt::registry& entt_registry, OutputArchive& archive) {
			entt::snapshot snapshot{ entt_registry };
			snapshot.get<entt::entity>(archive);
			snapshot_get_fn(archive, snapshot);
		}
		void save(entt::registry& entt_registry, std::ostream& os) {
			OutputArchive archive{ os };
			save(entt_registry, archive);
		}

		void load(entt::registry& entt_registry, InputArchive& archive) {
			entt::snapshot_loader snapshot_loader{ entt_registry };
			snapshot_loader.get<entt::entity>(archive);
			snapshot_loader_get_fn(archive, snapshot_loader);
		}
		void load(entt::registry& entt_registry, std::istream& is) {
			InputArchive archive{ is };
			load(entt_registry, archive);
		}
	};
}