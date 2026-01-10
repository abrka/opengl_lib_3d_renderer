#pragma once

#include <functional>
#include <istream>
#include <ostream>

#include <entt/entt.hpp>
#include <cereal/archives/xml.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/ObjectStream/ObjectStreamTextIn.h>
#include <Jolt/ObjectStream/ObjectStreamTextOut.h>

#include "engine/loaders/mesh_loader.h"
#include "engine/loaders/shader_loader.h"
#include "engine/systems/script_system.h"

#include "cereal_serialize_custom_types.h"
#include "entt_registry_serializer_get_func.h"

namespace Engine {
	template<typename InputArchive, typename OutputArchive>
	struct EnttRegistrySerializer {
		using snapshot_get_func = std::function<void(OutputArchive&, entt::snapshot&)>;
		snapshot_get_func snapshot_get_fn{};
		using snapshot_loader_get_func = std::function<void(InputArchive&, entt::snapshot_loader&)>;
		snapshot_loader_get_func snapshot_loader_get_fn{};

		EnttRegistrySerializer(snapshot_get_func f1 = snapshot_get_func_engine_types<OutputArchive,entt::snapshot>, snapshot_loader_get_func f2 = snapshot_get_func_engine_types<InputArchive, entt::snapshot_loader>) : snapshot_get_fn(f1), snapshot_loader_get_fn(f2) {};

		void save(entt::registry& entt_registry, OutputArchive& archive) {
			entt::snapshot snapshot{ entt_registry };
			snapshot.get<entt::entity>(archive);
			snapshot_get_fn(archive, snapshot);
		}
		tl::expected<void, std::string> save(entt::registry& entt_registry, std::ostream& os) {
			try
			{
				OutputArchive archive{ os };
				save(entt_registry, archive);
				
			}
			catch (const std::exception& e)
			{
				return tl::make_unexpected(std::string{ e.what() });
			}
			return {};
		}

		void load(entt::registry& entt_registry, InputArchive& archive) {
			entt_registry.clear();
			entt::snapshot_loader snapshot_loader{ entt_registry };
			snapshot_loader.get<entt::entity>(archive);
			snapshot_loader_get_fn(archive, snapshot_loader);
		}
		tl::expected<void,std::string> load(entt::registry& entt_registry, std::istream& is) {
			try
			{
				InputArchive archive{ is };
				load(entt_registry, archive);
			}
			catch (const std::exception& e)
			{
				return tl::make_unexpected(std::string{ e.what() });
			}
			return {};
		}
	};
}