#pragma once

#include <memory>
#include <filesystem>
#include <entt/entt.hpp>
#include "renderer/mesh_builder.h"

namespace Engine {
	struct MeshLoader {
		using result_type = std::shared_ptr<MeshBuilder::Scene>;
		using error_type = std::string;

	private:
		error_type error{};

	public:
		result_type operator()(std::filesystem::path path) {
			auto res = MeshBuilder::build(path);
			if (!res.has_value()) {
				error = res.error();
			}
			return result_type(std::move(res.value()));
		}
		error_type get_last_error() {
			return error;
		}
	};
}