#pragma once

#include <assert.h>
#include <vector>
#include <memory>
#include <span>
#include <map>
#include <set>

#include <tl/expected.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GL3D/mesh.h>
#include <GL3D/texture.h>

#include "assimp_glm.h"
#include "texture_builder.h"

namespace MeshBuilder {

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
		std::map<std::string, Texture> uniforms_texture{};
		std::map<std::string, int> uniforms_int{};
		std::map<std::string, float> uniforms_float{};
		std::map<std::string, glm::vec3> uniforms_vec3{};
		std::map<std::string, glm::mat4> uniforms_mat4{};

		template<typename MapType>
		void set_uniforms(const GL3D::ShaderProgram& shader, MapType& uniform_map) const {
			for (const auto& [uniform_name, value] : uniform_map) {
				shader.set_uniform(uniform_name, value);
			}
		}
		void set_uniforms(const GL3D::ShaderProgram& shader, std::map<std::string, Texture> textures_map) const {
			for (const auto& [uniform_name, texture] : uniforms_texture) {
				shader.set_texture(uniform_name, *texture.texture, texture.texture_unit);
			}
		}
		void set_all_uniforms(const GL3D::ShaderProgram& shader) const {
			set_uniforms(shader, uniforms_texture);
			set_uniforms(shader, uniforms_int);
			set_uniforms(shader, uniforms_float);
			set_uniforms(shader, uniforms_vec3);
			set_uniforms(shader, uniforms_mat4);
		}
	};
	struct Mesh {
		std::unique_ptr<GL3D::Mesh> mesh{};
		std::vector<VertexAttrib> vertex_attribs{};
		Material material{};
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
	};
	struct Scene {
		std::unique_ptr<Node> root_node{};
		std::string name{};

		std::vector<Material*> get_all_materials() {
			return root_node->get_all_materials();
		}
	};


	namespace detail {
		std::vector<int> get_num_floats_per_attribute(const std::vector<VertexAttrib>& vertex_attribs) {
			std::vector<int> num_floats_per_attribute{};
			for (const auto& vertex_attrib : vertex_attribs) {
				num_floats_per_attribute.push_back(vertex_attrib.size);
			}
			return num_floats_per_attribute;
		}


		std::vector<std::filesystem::path> get_all_texture_paths_from_type(const aiMaterial* ai_material, const aiTextureType ai_texture_type) {
			std::vector<std::filesystem::path> texture_paths{};
			auto num_textures = ai_material->GetTextureCount(ai_texture_type);
			for (auto i = 0; i < num_textures; i++) {
				aiString texture_path{};
				aiReturn ret = ai_material->GetTexture(ai_texture_type, i, &texture_path);
				std::string texture_path_str = std::string{ texture_path.data, texture_path.length };
				texture_paths.push_back(texture_path_str);
				if (ret != aiReturn_SUCCESS) {
					return {};
				}
			}
			return texture_paths;
		}

		std::string ai_texture_type_to_uniform_name(const aiTextureType tex_type) {
			static std::map<aiTextureType, std::string> map
			{
				{ aiTextureType_NONE				   , "u_texture_none"},
				{ aiTextureType_DIFFUSE				   , "u_texture_diffuse"},
				{ aiTextureType_SPECULAR			   , "u_texture_specular"},
				{ aiTextureType_AMBIENT				   , "u_texture_ambient"},
				{ aiTextureType_EMISSIVE			   , "u_texture_emissive"},
				{ aiTextureType_HEIGHT				   , "u_texture_height"},
				{ aiTextureType_NORMALS				   , "u_texture_normals"},
				{ aiTextureType_SHININESS			   , "u_texture_shininess"},
				{ aiTextureType_OPACITY				   , "u_texture_opacity"},
				{ aiTextureType_DISPLACEMENT		   , "u_texture_displacement"},
				{ aiTextureType_LIGHTMAP			   , "u_texture_lightmap"},
				{ aiTextureType_REFLECTION			   , "u_texture_reflection"},
				{ aiTextureType_BASE_COLOR			   , "u_texture_base_color"},
				{ aiTextureType_NORMAL_CAMERA		   , "u_texture_nomral_camera"},
				{ aiTextureType_EMISSION_COLOR		   , "u_texture_emission_color"},
				{ aiTextureType_METALNESS 			   , "u_texture_metalness"},
				{ aiTextureType_DIFFUSE_ROUGHNESS	   , "u_texture_diffuse_roughness"},
				{ aiTextureType_AMBIENT_OCCLUSION 	   , "u_texture_ambient_occlusion"},
				{ aiTextureType_UNKNOWN 			   , "u_texture_unknown"},
				{ aiTextureType_SHEEN 				   , "u_texture_sheen"},
				{ aiTextureType_CLEARCOAT			   , "u_texture_clearcoat"},
				{ aiTextureType_TRANSMISSION		   , "u_texture_transmission"},
				{ aiTextureType_MAYA_BASE			   , "u_texture_maya_base"},
				{ aiTextureType_MAYA_SPECULAR		   , "u_texture_maya_specular"},
				{ aiTextureType_MAYA_SPECULAR_COLOR	   , "u_texture_maya_specular_color"},
				{ aiTextureType_MAYA_SPECULAR_ROUGHNESS, "u_texture_maya_specular_roughness"},
				{ aiTextureType_ANISOTROPY			   , "u_texture_anisotropy"},
				{ aiTextureType_GLTF_METALLIC_ROUGHNESS, "u_texture_gltf_metallic_roughness"}
			};
			return map.at(tex_type);
		}
		Material process_material(std::filesystem::path model_dir, const aiMaterial* ai_material, const std::map<std::filesystem::path, std::shared_ptr<GL3D::Texture>>& textures) {
			Material mat{};

			unsigned int i_valid{};
			for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			{
				aiTextureType tex_type = static_cast<aiTextureType>(i);
				auto texture_paths = get_all_texture_paths_from_type(ai_material, tex_type);
				if (texture_paths.empty()) {
					continue;
				}
				if (texture_paths.size() > 1) {
					assert(false && "more than 1 texture found for a type");
				}
				std::filesystem::path texture_path = texture_paths[0];
				std::string texture_uniform_name = ai_texture_type_to_uniform_name(tex_type);
				auto texture = textures.at(texture_path);
				assert(texture);
				mat.uniforms_texture[texture_uniform_name] = Texture{ i_valid, texture };
				i_valid++;
			}
			return mat;
		}



		Mesh process_mesh(std::filesystem::path model_dir, const aiScene* ai_scene, const aiMesh* ai_mesh, const std::map<std::filesystem::path, std::shared_ptr<GL3D::Texture>>& textures) {
			std::vector<VertexAttrib> vertex_attribs{};
			if (ai_mesh->HasPositions()) {
				vertex_attribs.push_back({ 3, VertexAttribType::position });
			}
			if (ai_mesh->HasNormals()) {
				vertex_attribs.push_back({ 3, VertexAttribType::normal });
			}
			for (size_t i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++)
			{
				auto ai_tex_coord = ai_mesh->mTextureCoords[i];
				if (!ai_tex_coord) {
					continue;
				}
				vertex_attribs.push_back({ 2, VertexAttribType::tex_coord });
			}

			std::vector<float> vertices{};
			// process vertices
			for (size_t i = 0; i < ai_mesh->mNumVertices; i++)
			{
				// process position;
				auto ai_pos = ai_mesh->mVertices[i];
				vertices.push_back(ai_pos.x);
				vertices.push_back(ai_pos.y);
				vertices.push_back(ai_pos.z);
				// process normals
				if (ai_mesh->HasNormals()) {
					auto ai_normal = ai_mesh->mNormals[i];
					vertices.push_back(ai_normal.x);
					vertices.push_back(ai_normal.y);
					vertices.push_back(ai_normal.z);
				}
				// process tex coords
				for (size_t j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; j++)
				{
					auto ai_tex_coord = ai_mesh->mTextureCoords[j];
					if (!ai_tex_coord) {
						continue;
					}
					vertices.push_back(ai_tex_coord[i].x);
					vertices.push_back(ai_tex_coord[i].y);
				}
			}
			std::vector<unsigned int> indices{};
			// process indices
			for (size_t i = 0; i < ai_mesh->mNumFaces; i++)
			{
				auto& ai_face = ai_mesh->mFaces[i];
				for (size_t j = 0; j < ai_face.mNumIndices; j++)
				{
					auto indice = ai_face.mIndices[j];
					indices.push_back(indice);
				}
			}

			auto num_floats_per_attr = get_num_floats_per_attribute(vertex_attribs);
			auto created_mesh = std::make_unique<GL3D::Mesh>(std::span<float>(vertices.data(), vertices.size()), std::span<int>(num_floats_per_attr.data(), num_floats_per_attr.size()), std::span<unsigned int>(indices.data(), indices.size()));

			auto ai_material = ai_scene->mMaterials[ai_mesh->mMaterialIndex];
			Material material = process_material(model_dir, ai_material, textures);
			return Mesh{ std::move(created_mesh), vertex_attribs, std::move(material) };
		}

		std::unique_ptr<Node> process_single_node(std::filesystem::path model_dir, const aiScene* scene, const aiNode* node, const std::map<std::filesystem::path, std::shared_ptr<GL3D::Texture>>& textures) {
			auto node_data = std::make_unique<Node>();
			node_data->name = std::string(node->mName.data, node->mName.length);
			node_data->transform = assimp_matrix_to_glm_matrix(node->mTransformation);
			for (size_t i = 0; i < node->mNumMeshes; i++)
			{
				unsigned int mesh_idx = node->mMeshes[i];
				auto result_mesh = process_mesh(model_dir, scene, scene->mMeshes[mesh_idx], textures);
				node_data->meshes.push_back(std::move(result_mesh));
			}
			return node_data;
		}
		std::unique_ptr<Node> process_node(std::filesystem::path model_dir, const aiScene* scene, const aiNode* parent_node, const std::map<std::filesystem::path, std::shared_ptr<GL3D::Texture>>& textures) {
			auto node_data_result = process_single_node(model_dir, scene, parent_node, textures);
			// process children recursively
			for (size_t i = 0; i < parent_node->mNumChildren; i++) {
				auto node_child = process_node(model_dir, scene, parent_node->mChildren[i], textures);
				node_child->parent = node_data_result.get();
				node_data_result->child_nodes.push_back(std::move(node_child));
			}
			return node_data_result;
		}
		bool is_assimp_scene_valid(const aiScene* assimp_scene) {
			return !(!assimp_scene || assimp_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !assimp_scene->mRootNode);
		}
		std::set<std::filesystem::path> get_all_texture_paths_in_scene(const aiScene* ai_scene) {
			std::set<std::filesystem::path> paths{};
			for (size_t i = 0; i < ai_scene->mNumMaterials; i++)
			{
				aiMaterial* ai_material = ai_scene->mMaterials[i];
				for (size_t j = 0; j < AI_TEXTURE_TYPE_MAX; j++)
				{
					aiTextureType tex_type = static_cast<aiTextureType>(j);
					auto texture_paths = get_all_texture_paths_from_type(ai_material, tex_type);
					for (const auto& texture_path : texture_paths) {
						paths.insert(texture_path);
					}
				}
			}
			return paths;
		}
		tl::expected<std::unique_ptr<Scene>, std::string> build(std::filesystem::path filepath) {
			std::filesystem::path model_dir = filepath.parent_path();

			Assimp::Importer assimp_importer{};
			const aiScene* assimp_scene = assimp_importer.ReadFile(filepath.string().c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
			if (!is_assimp_scene_valid(assimp_scene)) {
				return tl::unexpected{ std::string{assimp_importer.GetErrorString()} };
			}
			std::map<std::filesystem::path, std::shared_ptr<GL3D::Texture>> textures{};
			for (const auto& tex_path : get_all_texture_paths_in_scene(assimp_scene)) {
				auto texture_complete_path = model_dir / tex_path;
				auto texture_res = TextureBuilder::build(texture_complete_path);
				if (!texture_res.has_value()) {
					auto err = texture_res.error();
					std::string err_str = "Error loading texture. error was" + std::to_string((int)err);
					return tl::make_unexpected(err_str);
				}
				textures[tex_path] = std::move(texture_res.value());
			}

			auto root_node = process_node(model_dir, assimp_scene, assimp_scene->mRootNode, textures);
			std::string scene_name = std::string(assimp_scene->mName.data, assimp_scene->mName.length);
			return std::make_unique<Scene>(std::move(root_node), scene_name);
		}
	}

	tl::expected<std::unique_ptr<Scene>, std::string> build(std::filesystem::path filepath) {
		return detail::build(filepath);
	}







}