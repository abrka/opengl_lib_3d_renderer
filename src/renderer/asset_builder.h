#pragma once

#include <filesystem>
#include <memory>
#include <map>
#include <string>

#include <tl/expected.hpp>
#include <glm/glm.hpp>

#include <GL3D/mesh.h>
#include <GL3D/texture.h>

namespace AssetBuilder {
	enum class VertexAttribType {
		none = 0,
		position,
		normal,
		tex_coord
	};
	struct VertexAttrib {
		size_t size{}; // this is the size of one vertex attribute based on the size of float. if an attrib contains 3 floats, the size will be 3
		VertexAttribType type{};
	};
	struct Texture {
		unsigned int texture_unit{};
		std::shared_ptr<GL3D::Texture> texture{};
	};
	struct Material {
		std::map<std::string, int> uniforms_int{};
		std::map<std::string, float> uniforms_float{};
		std::map<std::string, glm::vec3> uniforms_vec3{};
		std::map<std::string, glm::mat4> uniforms_mat4{};
		std::map<std::string, std::string> uniforms_string{};
		std::map<std::string, Texture> uniforms_texture{};

		void set_uniform(const std::string& name, int value) {
			uniforms_int[name] = value;
		}
		void set_uniform(const std::string& name, float value) {
			uniforms_float[name] = value;
		}
		void set_uniform(const std::string& name, glm::vec3 value) {
			uniforms_vec3[name] = value;
		}
		void set_uniform(const std::string& name, glm::mat4 value) {
			uniforms_mat4[name] = value;
		}
		void set_uniform(const std::string& name, Texture value) {
			uniforms_texture[name] = value;
		}
		void set_uniform(const std::string& name, std::string value) {
			uniforms_string[name] = value;
		}
		template<typename T>
		const T* get_uniform(const std::string& name) const {
			assert(false);
		}
	};
	struct Node;
	struct Mesh {
		std::unique_ptr<GL3D::Mesh> mesh{};
		std::vector<VertexAttrib> vertex_attribs{};
		Material material{};
		Node* node{};
	};
	struct Node {
		std::string name{};
		glm::mat4 transform{};
		std::vector<Mesh> meshes{};
		Node* parent{};
		std::vector<std::unique_ptr<Node>> child_nodes{};

		glm::mat4 get_global_transform() const {
			if (!parent) {
				return glm::mat4(1.0f);
			}
			return transform * parent->get_global_transform();
		}
		std::vector<Mesh*> get_all_meshes() {
			std::vector<Mesh*> all_meshes = get_all_meshes_single();
			for (size_t i = 0; i < child_nodes.size(); i++) {
				std::vector<Mesh*> child_meshes = child_nodes[i]->get_all_meshes();
				all_meshes.insert(std::end(all_meshes), std::begin(child_meshes), std::end(child_meshes));
			}
			return all_meshes;
		}
		std::vector<Material*> get_all_materials() {
			auto materials = get_all_materials_single();
			for (size_t i = 0; i < child_nodes.size(); i++)
			{
				auto child_materials = child_nodes[i]->get_all_materials();
				materials.insert(std::end(materials), std::begin(child_materials), std::end(child_materials));
			}
			return materials;
		}
	private:
		std::vector<Material*> get_all_materials_single() {
			std::vector<Material*> materials{};
			for (size_t i = 0; i < meshes.size(); i++)
			{
				Material* mat = &meshes[i].material;
				materials.push_back(mat);
			}
			return materials;
		}
		std::vector<Mesh*> get_all_meshes_single() {
			std::vector<Mesh*> all_meshes{};
			for (size_t i = 0; i < meshes.size(); i++)
			{
				all_meshes.push_back(&meshes[i]);
			}
			return all_meshes;
		}
	};
	struct Scene {
		std::unique_ptr<Node> root_node{};
		std::string name{};

		std::vector<Material*> get_all_materials() const {
			return root_node->get_all_materials();
		}

		std::vector<Mesh*> get_all_meshes() const {
			return root_node->get_all_meshes();
		}
	};
	tl::expected<std::unique_ptr<Scene>, std::string> build(std::filesystem::path filepath);
}