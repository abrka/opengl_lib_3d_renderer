#include "asset_builder.h"

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

namespace AssetBuilder {
	template<>
	const int* Material::get_uniform<int>(const std::string& name) const {
		if (uniforms_int.contains(name)) {
			return &uniforms_int.at(name);
		}
		return nullptr;
	}
	template<>
	const std::string* Material::get_uniform<std::string>(const std::string& name) const {
		if (uniforms_string.contains(name)) {
			return &uniforms_string.at(name);
		}
		return nullptr;
	}
	template<>
	const glm::vec3* Material::get_uniform<glm::vec3>(const std::string& name) const {
		if (uniforms_vec3.contains(name)) {
			return &uniforms_vec3.at(name);
		}
		return nullptr;
	}
	template<>
	const glm::mat4* Material::get_uniform<glm::mat4>(const std::string& name) const {
		if (uniforms_mat4.contains(name)) {
			return &uniforms_mat4.at(name);
		}
		return nullptr;
	}
	template<>
	const Texture* Material::get_uniform<Texture>(const std::string& name) const {
		if (uniforms_texture.contains(name)) {
			return &uniforms_texture.at(name);
		}
		return nullptr;
	}

	std::vector<int> get_num_floats_per_attribute(const std::vector<VertexAttrib>& vertex_attribs) {
		std::vector<int> num_floats_per_attribute{};
		for (const auto& vertex_attrib : vertex_attribs) {
			num_floats_per_attribute.push_back((int)vertex_attrib.size);
		}
		return num_floats_per_attribute;
	}
	std::vector<std::filesystem::path> get_all_texture_paths_from_type(const aiMaterial* ai_material, const aiTextureType ai_texture_type) {
		std::vector<std::filesystem::path> texture_paths{};
		auto num_textures = ai_material->GetTextureCount(ai_texture_type);
		for (size_t i = 0; i < num_textures; i++) {
			aiString texture_path{};
			aiReturn ret = ai_material->GetTexture(ai_texture_type, (unsigned int)i, &texture_path);
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
			{ aiTextureType_NONE				   , "texture_none"					},
			{ aiTextureType_DIFFUSE				   , "texture_diffuse"				},
			{ aiTextureType_SPECULAR			   , "texture_specular"				},
			{ aiTextureType_AMBIENT				   , "texture_ambient"				},
			{ aiTextureType_EMISSIVE			   , "texture_emissive"				},
			{ aiTextureType_HEIGHT				   , "texture_height"					},
			{ aiTextureType_NORMALS				   , "texture_normals"				},
			{ aiTextureType_SHININESS			   , "texture_shininess"				},
			{ aiTextureType_OPACITY				   , "texture_opacity"				},
			{ aiTextureType_DISPLACEMENT		   , "texture_displacement"			},
			{ aiTextureType_LIGHTMAP			   , "texture_lightmap"				},
			{ aiTextureType_REFLECTION			   , "texture_reflection"				},
			{ aiTextureType_BASE_COLOR			   , "texture_base_color"				},
			{ aiTextureType_NORMAL_CAMERA		   , "texture_normal_camera"			},
			{ aiTextureType_EMISSION_COLOR		   , "texture_emission_color"			},
			{ aiTextureType_METALNESS 			   , "texture_metalness"				},
			{ aiTextureType_DIFFUSE_ROUGHNESS	   , "texture_diffuse_roughness"		},
			{ aiTextureType_AMBIENT_OCCLUSION 	   , "texture_ambient_occlusion"		},
			{ aiTextureType_UNKNOWN 			   , "texture_unknown"				},
			{ aiTextureType_SHEEN 				   , "texture_sheen"					},
			{ aiTextureType_CLEARCOAT			   , "texture_clearcoat"				},
			{ aiTextureType_TRANSMISSION		   , "texture_transmission"			},
			{ aiTextureType_MAYA_BASE			   , "texture_maya_base"				},
			{ aiTextureType_MAYA_SPECULAR		   , "texture_maya_specular"			},
			{ aiTextureType_MAYA_SPECULAR_COLOR	   , "texture_maya_specular_color"	},
			{ aiTextureType_MAYA_SPECULAR_ROUGHNESS, "texture_maya_specular_roughness"},
			{ aiTextureType_ANISOTROPY			   , "texture_anisotropy"				},
			{ aiTextureType_GLTF_METALLIC_ROUGHNESS, "texture_gltf_metallic_roughness"}
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
			mat.set_uniform("u_material." + texture_uniform_name, Texture{ i_valid, texture });
			mat.set_uniform("u_material.has_" + texture_uniform_name, 1);
			i_valid++;
		}

		aiColor4D ai_base_color{};
		aiReturn ret = ai_material->Get(AI_MATKEY_BASE_COLOR, ai_base_color);
		if (ret == aiReturn_SUCCESS) {
			glm::vec4 color = assimp_vec4_to_glm_vec4(ai_base_color);
			mat.set_uniform("u_material.base_color", glm::vec3(color));
		}

		aiColor4D ai_ambient_color{};
		ret = ai_material->Get(AI_MATKEY_COLOR_AMBIENT, ai_ambient_color);
		if (ret == aiReturn_SUCCESS) {
			glm::vec4 color = assimp_vec4_to_glm_vec4(ai_ambient_color);
			mat.set_uniform("u_material.ambient_color", glm::vec3(color));
		}

		aiColor4D ai_specular_color{};
		ret = ai_material->Get(AI_MATKEY_COLOR_SPECULAR, ai_specular_color);
		if (ret == aiReturn_SUCCESS) {
			glm::vec4 color = assimp_vec4_to_glm_vec4(ai_specular_color);
			mat.set_uniform("u_material.specular_color", glm::vec3(color));
		}

		aiColor4D ai_emissive_color{};
		ret = ai_material->Get(AI_MATKEY_COLOR_EMISSIVE, ai_emissive_color);
		if (ret == aiReturn_SUCCESS) {
			glm::vec4 color = assimp_vec4_to_glm_vec4(ai_emissive_color);
			mat.set_uniform("u_material.emissive_color", glm::vec3(color));
		}

		ai_real ai_shininess{};
		ret = ai_material->Get(AI_MATKEY_SHININESS, ai_shininess);
		if (ret == aiReturn_SUCCESS) {
			mat.set_uniform("u_material.specular_strength", ai_shininess);
		}
		return mat;
	}

	Mesh process_mesh(std::filesystem::path model_dir, const aiScene* ai_scene, const aiMesh* ai_mesh, const std::map<std::filesystem::path, std::shared_ptr<GL3D::Texture>>& textures, Node& node) {
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
		return Mesh{ std::move(created_mesh), vertex_attribs, std::move(material), &node };
	}

	std::unique_ptr<Node> process_single_node(std::filesystem::path model_dir, const aiScene* ai_scene, const aiNode* ai_node, const std::map<std::filesystem::path, std::shared_ptr<GL3D::Texture>>& textures) {
		auto node_data = std::make_unique<Node>();
		node_data->name = std::string(ai_node->mName.data, ai_node->mName.length);
		node_data->transform = assimp_matrix_to_glm_matrix(ai_node->mTransformation);
		for (size_t i = 0; i < ai_node->mNumMeshes; i++)
		{
			unsigned int mesh_idx = ai_node->mMeshes[i];
			auto result_mesh = process_mesh(model_dir, ai_scene, ai_scene->mMeshes[mesh_idx], textures, *node_data);
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
				std::string err_str = "Error loading texture:\"" + tex_path.string() + "\" error was: " + TextureBuilder::texture_loader_error_to_string(err);
				return tl::make_unexpected(err_str);
			}
			textures[tex_path] = std::move(texture_res.value());
		}

		auto root_node = process_node(model_dir, assimp_scene, assimp_scene->mRootNode, textures);
		std::string scene_name = std::string(assimp_scene->mName.data, assimp_scene->mName.length);
		return std::make_unique<Scene>(std::move(root_node), scene_name);
	}

}