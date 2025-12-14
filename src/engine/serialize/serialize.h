#pragma once

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp> 

#include "engine/components/parent_component.h"
#include "engine/components/children_component.h"
#include "engine/components/name_component.h"
#include "engine/components/transform_component.h"


// WARNING: REMEMBER TO USE REFERENCES IN SERIALIZE FUNCTION INSTEAD OF VALUES

namespace glm {
	template<class Archive>
	void serialize(Archive& archive, glm::mat4& t) {
		archive(cereal::make_nvp("0", t[0]), cereal::make_nvp("1", t[1]), cereal::make_nvp("2", t[2]), cereal::make_nvp("3", t[3]));
	}
	template<class Archive>
	void serialize(Archive& archive, glm::vec4& t) {
		archive(cereal::make_nvp("x", t.x), cereal::make_nvp("y", t.y), cereal::make_nvp("z", t.z), cereal::make_nvp("w", t.w));
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
		archive(cereal::make_nvp("parent", entt::to_integral(t.entity)));
	}
	template<class Archive>
	void serialize(Archive& archive, ChildrenComponent& t) {
		archive(cereal::make_nvp("children", t.children));
	}
}