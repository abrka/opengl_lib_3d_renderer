#pragma once

#include <assert.h>
#include <vector>
#include <memory>
#include <span>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GL3D/mesh.h>

#include "assimp_glm.h"

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
	struct Mesh {
		std::unique_ptr<GL3D::Mesh> mesh{};
		std::vector<VertexAttrib> vertex_attribs{};
	};
	Mesh process_mesh(const aiMesh* ai_mesh) {
		std::vector<float> vertices{};
		std::vector<unsigned int> indices{};

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
		return Mesh{ std::move(created_mesh), vertex_attribs };
	}
	struct Node {
		std::string name{};
		glm::mat4 transform{};
		std::vector<Mesh> meshes{};
		Node* parent{};
		std::vector<std::unique_ptr<Node>> child_nodes{};
	};
	std::unique_ptr<Node> process_single_node(const aiScene* scene, const aiNode* node) {
		auto node_data = std::make_unique<Node>();
		node_data->name = std::string(node->mName.data, node->mName.length);
		node_data->transform = assimp_matrix_to_glm_matrix(node->mTransformation);
		for (size_t i = 0; i < node->mNumMeshes; i++)
		{
			unsigned int mesh_idx = node->mMeshes[i];
			auto result_mesh = process_mesh(scene->mMeshes[mesh_idx]);
			node_data->meshes.push_back(std::move(result_mesh));
		}
		return node_data;
	}
	std::unique_ptr<Node> process_node(const aiScene* scene, const aiNode* parent_node) {
		auto node_data_result = process_single_node(scene, parent_node);
		// process children recursively
		for (size_t i = 0; i < parent_node->mNumChildren; i++) {
			auto node_child = process_node(scene, parent_node->mChildren[i]);
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
	std::optional<Scene> build(std::string filepath) {
		Assimp::Importer assimp_importer{};
		const aiScene* assimp_scene = assimp_importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!is_assimp_scene_valid(assimp_scene)) {
			return std::nullopt;
		}
		auto root_node = process_node(assimp_scene, assimp_scene->mRootNode);
		std::string scene_name = std::string(assimp_scene->mName.data, assimp_scene->mName.length);
		return Scene{ std::move(root_node), scene_name };
	}
}