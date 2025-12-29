#pragma once

#include <filesystem>
#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp> 
#include "engine/components/components.h"


// WARNING: REMEMBER TO USE REFERENCES IN SERIALIZE FUNCTION INSTEAD OF VALUES
namespace cereal {
	template <class Archive>
	void save(Archive& archive, const std::filesystem::path& t)
	{
		// Use path.string() or path.u8string() for the underlying string representation
		archive(t.string());
	}

	// Loading std::filesystem::path: construct from std::string
	template <class Archive>
	void load(Archive& archive, std::filesystem::path& t)
	{
		std::string s;
		archive(s);
		t = std::filesystem::path(s);
	}
}
namespace glm {
	template<class Archive>
	void serialize(Archive& archive, glm::mat4& t) {
		archive(
			cereal::make_nvp("v0", t[0]),
			cereal::make_nvp("v1", t[1]),
			cereal::make_nvp("v2", t[2]),
			cereal::make_nvp("v3", t[3])
		);
	}
	template<class Archive>
	void serialize(Archive& archive, glm::vec4& t) {
		archive(
			cereal::make_nvp("x", t.x),
			cereal::make_nvp("y", t.y),
			cereal::make_nvp("z", t.z),
			cereal::make_nvp("w", t.w)
		);
	}
	template<class Archive>
	void serialize(Archive& archive, glm::vec3& t) {
		archive(
			cereal::make_nvp("x", t.x),
			cereal::make_nvp("y", t.y),
			cereal::make_nvp("z", t.z)
		);
	}
}

namespace Engine {
	template<class Archive>
	void serialize(Archive& archive, NameComponent& t) {
		archive(cereal::make_nvp("name", t.name));
	}
	template<class Archive>
	void serialize(Archive& archive, TransformComponent& t) {
		archive(cereal::make_nvp("transform", t.transform));
	}
	template<class Archive>
	void serialize(Archive& archive, ParentComponent& t) {
		archive(cereal::make_nvp("parent", t.entity));
	}
	template<class Archive>
	void serialize(Archive& archive, ChildrenComponent& t) {
		archive(cereal::make_nvp("children", t.children));
	}
	template<class Archive>
	void serialize(Archive& archive, PointLightComponent& t) {
		archive(cereal::make_nvp("point_light", t.light));
	}
	template<class Archive>
	void serialize(Archive& archive, CameraComponent& t) {
		archive(cereal::make_nvp("camera", t.camera));
	}
	template<class Archive>
	void serialize(Archive& archive, MeshComponent& t) {
		archive(cereal::make_nvp("filepath", t.filepath));
	}
	template<class Archive>
	void serialize(Archive& archive, ShaderComponent& t) {
		archive(
			cereal::make_nvp("vertex_filepath", t.vertex_filepath),
			cereal::make_nvp("fragment_filepath", t.fragment_filepath)
		);
	}
	template<class Archive>
	void serialize(Archive& archive, ScriptComponent& t) {
		archive(cereal::make_nvp("filepath", t.filepath));
	}
}

namespace Renderer {
	template<class Archive>
	void serialize(Archive& archive, color_t& t) {
		archive(cereal::make_nvp("color", t.color));
	}
	template<class Archive>
	void serialize(Archive& archive, Camera& t) {
		archive(
			cereal::make_nvp("position", t.position),
			cereal::make_nvp("orientation", t.orientation),
			cereal::make_nvp("fov", t.fov),
			cereal::make_nvp("near_plane_distance", t.near_plane_dist),
			cereal::make_nvp("far_plane_distance", t.far_plane_dist),
			cereal::make_nvp("aspect_ratio", t.aspect_ratio)
		);
	}
	template<class Archive>
	void serialize(Archive& archive, PointLight& t) {
		archive(
			cereal::make_nvp("color", t.color),
			cereal::make_nvp("ambient_strength", t.ambient_strength),
			cereal::make_nvp("diffuse_strength", t.diffuse_strength),
			cereal::make_nvp("specular_strength", t.specular_strength),
			cereal::make_nvp("constant", t.constant),
			cereal::make_nvp("linear", t.linear),
			cereal::make_nvp("quadratic", t.quadratic)
		);
	}
}