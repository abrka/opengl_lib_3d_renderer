#pragma once

#include <fstream>
#include <filesystem>

#include <entt/entt.hpp>
#include <cereal/cereal.hpp>
#include <imgui.h>

#include "engine/components/components.h"
#include "engine/serialize/cereal_serialize_custom_types.h"
#include "engine/serialize/entt_registry_serializer.h"

namespace Editor {
	namespace LoadSceneButton {
		template<typename InputArchive, typename OutputArchive>
		void render(std::filesystem::path saved_file, std::string button_text, entt::registry& entt_registry, Engine::EnttRegistrySerializer<InputArchive,OutputArchive>& serializer) {
			if (ImGui::Button(button_text.c_str())) {
				entt_registry.clear();
				std::ifstream ifs(saved_file);
				serializer.load(entt_registry, ifs);
			}
		}
	};
}