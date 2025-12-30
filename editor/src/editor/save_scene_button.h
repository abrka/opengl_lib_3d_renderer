#pragma once

#include <filesystem>
#include <fstream>

#include <entt/entt.hpp>
#include <cereal/cereal.hpp>
#include <imgui.h>

#include "engine/components/components.h"
#include "engine/serialize/cereal_serialize_custom_types.h"
#include "engine/serialize/entt_registry_serializer.h"

namespace Editor {
	namespace SaveSceneButton {
		template<typename InputArchive, typename OutputArchive>
		void render(std::filesystem::path saved_file, std::string button_text, entt::registry& entt_registry, Engine::EnttRegistrySerializer<InputArchive, OutputArchive>& serializer) {
			if (ImGui::Button(button_text.c_str())) {
				std::ofstream of(saved_file);
				{
					serializer.save(entt_registry, of);
				}
			}
		}
	};
}