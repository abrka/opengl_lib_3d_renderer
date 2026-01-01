#include "reflect_sol.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "engine/components/components.h"
#include "input/input.h"

namespace Reflect {
	static void register_sol_usertype_glm_vec3(sol::state& sol_state) {
		// Register glm::vec3 as a usertype in Lua
		sol_state.new_usertype<glm::vec3>(
			"vec3",
			sol::constructors<glm::vec3(), glm::vec3(float), glm::vec3(float, float, float)>(),
			// Expose data members (x, y, z)
			"x", &glm::vec3::x,
			"y", &glm::vec3::y,
			"z", &glm::vec3::z,
			// Expose functions (e.g., length, normalize - which are free functions in GLM)
			"length", sol::overload(static_cast<float(*)(glm::vec3 const&)>(&glm::length)),
			"normalize", sol::overload(static_cast<glm::vec3(*)(glm::vec3 const&)>(&glm::normalize)),
			// Expose operators for natural Lua syntax
			sol::meta_function::addition, sol::overload(static_cast<glm::vec3(*)(glm::vec3 const&, glm::vec3 const&)>(&glm::operator+)),
			sol::meta_function::subtraction, sol::overload(static_cast<glm::vec3(*)(glm::vec3 const&, glm::vec3 const&)>(&glm::operator-)),
			sol::meta_function::multiplication, sol::overload(
				static_cast<glm::vec3(*)(glm::vec3 const&, float)>(&glm::operator*),
				static_cast<glm::vec3(*)(float, glm::vec3 const&)>(&glm::operator*),
				static_cast<glm::vec3(*)(glm::vec3 const&, glm::vec3 const&)>(&glm::operator*) // Component-wise
			),
			sol::meta_function::division, sol::overload(
				static_cast<glm::vec3(*)(glm::vec3 const&, float)>(&glm::operator/),
				static_cast<glm::vec3(*)(float, glm::vec3 const&)>(&glm::operator/),
				static_cast<glm::vec3(*)(glm::vec3 const&, glm::vec3 const&)>(&glm::operator/) // Component-wise
			)
		);
	}
	static glm::vec3 get_translation_from_mat4(glm::mat4& mat) {
		return glm::vec3(mat[3]);
	}
	static void set_translation_to_mat4(glm::mat4& mat, glm::vec3 translation) {
		mat[3].x = translation.x;
		mat[3].y = translation.y;
		mat[3].z = translation.z;
	}
	static void register_sol_usertype_glm_mat4(sol::state& sol_state) {
		sol_state.new_usertype<glm::mat4>(
			"mat4",
			sol::constructors<glm::mat4(), glm::mat4(float)>(), // mat4(1.0f) creates an identity matrix
			// You might not expose all 16 floats directly but rely on transformation functions
			// Operators (multiplication for matrix multiplication)
			"translation", sol::property(get_translation_from_mat4, set_translation_to_mat4),
			sol::meta_function::multiplication, sol::overload(
				static_cast<glm::mat4(*)(glm::mat4 const&, glm::mat4 const&)>(&glm::operator*),
				static_cast<glm::vec4(*)(glm::mat4 const&, glm::vec4 const&)>(&glm::operator*)
			)
		);
	}
	static void register_sol_module_math(sol::state& sol_state) {
		sol_state.create_named_table("Math");
		sol_state["Math"]["translate"] = sol::overload(static_cast<glm::mat4(*)(glm::mat4 const&, glm::vec3 const&)>(&glm::translate));
		sol_state["Math"]["rotate"] = sol::overload(static_cast<glm::mat4(*)(glm::mat4 const&, float, glm::vec3 const&)>(&glm::rotate));
		sol_state["Math"]["scale"] = sol::overload(static_cast<glm::mat4(*)(glm::mat4 const&, glm::vec3 const&)>(&glm::scale));
	}
	void register_sol_usertypes(sol::state& sol_state)
	{
		register_sol_usertype_glm_vec3(sol_state);
		register_sol_usertype_glm_mat4(sol_state);
		register_sol_module_math(sol_state);
		sol_state.new_usertype<Engine::NameComponent>("NameComponent",
			"name", &Engine::NameComponent::name);
		sol_state.new_usertype<Engine::TransformComponent>("TransformComponent",
			"transform", &Engine::TransformComponent::transform);
		sol_state.new_usertype<Input::Input>("Input",
			"is_key_pressed", &Input::Input::is_key_pressed_str);
	}
}