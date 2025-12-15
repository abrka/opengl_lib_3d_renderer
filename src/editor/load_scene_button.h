#pragma once

#include <fstream>
#include <filesystem>

#include <entt/entt.hpp>
#include <cereal/cereal.hpp>
#include <cereal/archives/xml.hpp>
#include <imgui.h>

#include "engine/components/components.h"
#include "engine/serialize/serialize.h"

namespace Editor {
	namespace LoadSceneButton {
		void render(std::filesystem::path saved_file, std::string button_text, entt::registry& entt_registry) {
			if (ImGui::Button(button_text.c_str())) {
				std::ifstream ifs(saved_file);
				cereal::XMLInputArchive archive{ ifs };
				entt_registry.clear();
				entt::snapshot_loader{ entt_registry }
					.get<entt::entity>(archive)
					.get<Engine::RootComponent>(archive)
					.get<Engine::ParentComponent>(archive)
					.get<Engine::ChildrenComponent>(archive)
					.get<Engine::NameComponent>(archive)
					.get<Engine::TransformComponent>(archive)
					.orphans()
					;
			}
		}
	};
}