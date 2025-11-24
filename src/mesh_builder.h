#pragma once

#include <assert.h>
#include <vector>
#include <memory>
#include <span>

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
	std::vector<int> get_num_floats_per_attribute(const std::vector<VertexAttrib>& vertex_attribs) {
		std::vector<int> num_floats_per_attribute{};
		for (const auto& vertex_attrib : vertex_attribs) {
			num_floats_per_attribute.push_back(vertex_attrib.size);
		}
		return num_floats_per_attribute;
	}

	struct Material {
		std::unique_ptr<GL3D::Texture> diffuse_texture{};
		std::unique_ptr<GL3D::Texture> metallic_texture{};
		std::unique_ptr<GL3D::Texture> roughness_texture{};
		std::unique_ptr<GL3D::Texture> normal_texture{};
	};
	std::vector<std::string> get_all_texture_paths_from_type(const aiMaterial* ai_material, const aiTextureType ai_texture_type) {
		std::vector<std::string> texture_paths{};
		auto num_textures = ai_material->GetTextureCount(ai_texture_type);
		for (auto i = 0; i < num_textures; i++) {
			aiString texture_path{};
			aiReturn ret = ai_material->GetTexture(ai_texture_type, i, &texture_path);
			texture_paths.push_back(std::string{ texture_path.data, texture_path.length });
			if (ret != aiReturn_SUCCESS) {
				return {};
			}
		}
		return texture_paths;
	}
	std::unique_ptr<GL3D::Texture> process_texture(std::filesystem::path model_dir, const aiMaterial* ai_material,const aiTextureType ai_texture_type) {
		auto texture_paths = get_all_texture_paths_from_type(ai_material, ai_texture_type);
		assert(texture_paths.size() == 1);
		std::filesystem::path texture_path = texture_paths[0];
		auto texture = TextureBuilder::build(model_dir / texture_path).value_or(nullptr);
		return texture;
	}
	Material process_material(std::filesystem::path model_dir, const aiMaterial* ai_material) {
		auto diffuse_texture = process_texture(model_dir, ai_material, aiTextureType_BASE_COLOR);
		auto metallic_texture = process_texture(model_dir, ai_material, aiTextureType_BASE_COLOR);
		auto roughness_texture = process_texture(model_dir, ai_material, aiTextureType_BASE_COLOR);
		auto normal_texture = process_texture(model_dir, ai_material, aiTextureType_BASE_COLOR);
		return Material{ std::move(diffuse_texture), std::move(metallic_texture), std::move(roughness_texture), std::move(normal_texture) };
	}

	struct Mesh {
		std::unique_ptr<GL3D::Mesh> mesh{};
		std::vector<VertexAttrib> vertex_attribs{};
		Material material{};
	};

	Mesh process_mesh(std::filesystem::path model_dir, const aiScene* ai_scene, const aiMesh* ai_mesh) {
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
		Material material = process_material(model_dir, ai_material);
		return Mesh{ std::move(created_mesh), vertex_attribs, std::move(material) };
	}
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
	};
	std::unique_ptr<Node> process_single_node(std::filesystem::path model_dir, const aiScene* scene, const aiNode* node) {
		auto node_data = std::make_unique<Node>();
		node_data->name = std::string(node->mName.data, node->mName.length);
		node_data->transform = assimp_matrix_to_glm_matrix(node->mTransformation);
		for (size_t i = 0; i < node->mNumMeshes; i++)
		{
			unsigned int mesh_idx = node->mMeshes[i];
			auto result_mesh = process_mesh(model_dir, scene, scene->mMeshes[mesh_idx]);
			node_data->meshes.push_back(std::move(result_mesh));
		}
		return node_data;
	}
	std::unique_ptr<Node> process_node(std::filesystem::path model_dir, const aiScene* scene, const aiNode* parent_node) {
		auto node_data_result = process_single_node(model_dir, scene, parent_node);
		// process children recursively
		for (size_t i = 0; i < parent_node->mNumChildren; i++) {
			auto node_child = process_node(model_dir, scene, parent_node->mChildren[i]);
			node_child->parent = node_data_result.get();
			node_data_result->child_nodes.push_back(std::move(node_child));
		}
		return node_data_result;
	}
	bool is_assimp_scene_valid(const aiScene* assimp_scene) {
		return !(!assimp_scene || assimp_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !assimp_scene->mRootNode);
	}
	struct Scene {
		std::unique_ptr<Node> root_node{};
		std::string name{};
	};
	tl::expected<Scene, std::string> build(std::filesystem::path filepath) {
		Assimp::Importer assimp_importer{};
		const aiScene* assimp_scene = assimp_importer.ReadFile(filepath.string().c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!is_assimp_scene_valid(assimp_scene)) {
			return tl::unexpected{ std::string{assimp_importer.GetErrorString()} };
		}
		std::filesystem::path model_dir = filepath.parent_path();
		auto root_node = process_node(model_dir, assimp_scene, assimp_scene->mRootNode);
		std::string scene_name = std::string(assimp_scene->mName.data, assimp_scene->mName.length);
		return Scene{ std::move(root_node), scene_name };
	}
}