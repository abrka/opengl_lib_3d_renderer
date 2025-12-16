#pragma once

#include <fstream>
#include <filesystem>

#include <entt/entt.hpp>
#include <cereal/cereal.hpp>
#include <cereal/archives/xml.hpp>
#include <imgui.h>

#include "engine/components/components.h"
#include "engine/serialize/cereal_serialize_custom_types.h"
#include "engine/serialize/serialize_entt.h"

namespace Editor {
	namespace LoadSceneButton {
		template<typename Archive>
		void render(std::filesystem::path saved_file, std::string button_text, entt::registry& entt_registry, Engine::snapshot_loader_get_func<Archive> snapshot_loader_get_fn) {
			if (ImGui::Button(button_text.c_str())) {
				std::ifstream ifs(saved_file);
				Archive archive{ ifs };
				entt_registry.clear();
				Engine::deserialize_entt(entt_registry, archive, snapshot_loader_get_fn);
			}
		}
	};
}