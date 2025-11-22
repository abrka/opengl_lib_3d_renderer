#pragma once

#include <assert.h>
#include <vector>
#include <memory>
#include <span>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GL3D/mesh.h>

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
		size_t index{}; // index for multiple texture coordinates
	};
	std::vector<int> get_num_floats_per_attribute(const std::vector<VertexAttrib>& vertex_attribs) {
		std::vector<int> num_floats_per_attribute{};
		for (const auto& vertex_attrib : vertex_attribs) {
			num_floats_per_attribute.push_back(vertex_attrib.size);
		}
		return num_floats_per_attribute;
	}
	struct MeshData {
		std::unique_ptr<GL3D::Mesh> mesh{};
		std::vector<VertexAttrib> vertex_attribs{};
	};
	MeshData process_mesh(const aiMesh* mesh) {
		std::vector<float> vertices{};
		std::vector<unsigned int> indices{};

		std::vector<VertexAttrib> vertex_attribs{};
		if (mesh->HasPositions()) {
			vertex_attribs.push_back({3, VertexAttribType::position});
		}
		if (mesh->HasNormals()) {
			vertex_attribs.push_back({3, VertexAttribType::normal });
		}
		size_t valid_tex_coord_idx = 0;
		for (size_t i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++)
		{
			auto ai_tex_coord = mesh->mTextureCoords[i];
			if (!ai_tex_coord) {
				continue;
			}
			vertex_attribs.push_back({ 2, VertexAttribType::tex_coord });
			valid_tex_coord_idx++;
		}

		// process vertices
		for (size_t i = 0; i < mesh->mNumVertices; i++)
		{
			// process position;
			auto ai_pos = mesh->mVertices[i];
			vertices.push_back(ai_pos.x);
			vertices.push_back(ai_pos.y);
			vertices.push_back(ai_pos.z);
			// process normals
			if (mesh->HasNormals()) {
				auto ai_normal = mesh->mNormals[i];
				vertices.push_back(ai_normal.x);
				vertices.push_back(ai_normal.y);
				vertices.push_back(ai_normal.z);
			}
			// process tex coords
			for (size_t j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; j++)
			{
				auto ai_tex_coord = mesh->mTextureCoords[j];
				if (!ai_tex_coord) {
					continue;
				}
				vertices.push_back(ai_tex_coord[i].x);
				vertices.push_back(ai_tex_coord[i].y);
			}
		} 
		// process indices
		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			auto& ai_face = mesh->mFaces[i];
			for (size_t j = 0; j < ai_face.mNumIndices; j++)
			{
				auto indice = ai_face.mIndices[j];
				indices.push_back(indice);
			}
		}

		auto num_floats_per_attr = get_num_floats_per_attribute(vertex_attribs);
		auto created_mesh = std::make_unique<GL3D::Mesh>(std::span<float>(vertices.data(), vertices.size()), std::span<int>(num_floats_per_attr.data(), num_floats_per_attr.size()), std::span<unsigned int>(indices.data(), indices.size()));
		MeshData ret{};
		ret.mesh = std::move(created_mesh);
		ret.vertex_attribs = vertex_attribs;
		return ret;
	}
	struct NodeData {
		std::string name{};
		std::vector<MeshData> process_mesh_results{};
		std::vector<NodeData> child_nodes{};
	};
	NodeData process_single_node(const aiScene* scene, const aiNode* node) {
		NodeData node_data{};
		aiString node_name = node->mName;
		node_data.name = std::string(node_name.data, node_name.length);
		for (size_t i = 0; i < node->mNumMeshes; i++)
		{
			unsigned int mesh_idx = node->mMeshes[i];
			auto result_mesh = process_mesh(scene->mMeshes[mesh_idx]);
			node_data.process_mesh_results.push_back(std::move(result_mesh));
		}
		return node_data;
	}
	NodeData process_node(const aiScene* scene, const aiNode* parent_node) {
		NodeData node_data_result = process_single_node(scene, parent_node);
		// process children recursively
		for (size_t i = 0; i < parent_node->mNumChildren; i++) {
			NodeData node_child = process_node(scene, parent_node->mChildren[i]);
			node_data_result.child_nodes.push_back(std::move(node_child));
		}
		return node_data_result;
	}
	bool is_assimp_scene_valid(const aiScene* assimp_scene) {
		return !(!assimp_scene || assimp_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !assimp_scene->mRootNode);
	}
	NodeData build(std::string filepath) {
		Assimp::Importer assimp_importer{};
		const aiScene* assimp_scene = assimp_importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);
		assert(is_assimp_scene_valid(assimp_scene));

		return process_node(assimp_scene, assimp_scene->mRootNode);
	}
}