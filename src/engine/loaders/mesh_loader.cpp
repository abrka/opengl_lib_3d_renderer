#include "mesh_loader.h"

namespace Engine {
	MeshLoader::error_type MeshLoader::error{};

	MeshLoader::result_type MeshLoader::operator()(std::filesystem::path path) {
		auto res = AssetBuilder::build(path);
		if (!res.has_value()) {
			error = res.error();
			return result_type{};
		}
		return result_type(std::move(res.value()));
	}
	MeshLoader::error_type MeshLoader::get_last_error() {
		return error;
	}
}