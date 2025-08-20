#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Content/InputGeometry/InputMesh.h>
#include <Engine/Content/Animation/JointWeight.h>
#include <Engine/Content/Animation/BornNode.h>
#include <Engine/Content/Animation/Skeleton.h>
#include <Engine/Content/Material/Material.h>

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Model class
////////////////////////////////////////////////////////////////////////////////////////////
class Model {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// AssimpMesh structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct AssimpMesh {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		InputMesh                                        input;
		std::optional<uint32_t>                          materialIndex;
		std::unordered_map<std::string, JointWeightData> jointWeights;
		std::string                                      name;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// AssimpMaterial structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct AssimpMaterial {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// TextureType enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class TextureType : uint8_t {
			Diffuse,
			Bump,
			AmbientOcclusion,
			Roughness,
			Metallic,
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		const std::optional<std::filesystem::path>& GetTexture(TextureType type) const { return textures[static_cast<uint8_t>(type)]; }

		//=========================================================================================
		// public variables
		//=========================================================================================

		//* texture parameter *//

		std::array<std::optional<std::filesystem::path>, static_cast<uint8_t>(TextureType::Metallic) + 1> textures;

		//* value parameter *//

		Color3f color   = kWhite3<>;
		float roughness = 1.0f;
		float metallic  = 1.0f;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// using 
	////////////////////////////////////////////////////////////////////////////////////////////

	using MeshContainer     = std::vector<AssimpMesh>;
	using MaterialContainer = std::vector<AssimpMaterial>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Model()  = default;
	~Model() = default;

	void Load(const std::filesystem::path& filepath, uint32_t assimpOption = kDefaultAssimpOption_);

	//* meshes option *//

	const MeshContainer& GetMeshes() const { return meshes_; }
	MeshContainer& GetMeshes() { return meshes_; }

	const AssimpMesh& GetMesh(uint32_t index) const;
	AssimpMesh& GetMesh(uint32_t index);

	//* materials option *//

	const MaterialContainer& GetMaterials() const { return materials_; }

	//* node option *//

	const BornNode& GetRoot() const { return root_; }

	const Skeleton& GetSkeleton() const { return skeleton_; }

	//* option *//

	static const uint32_t GetDefaultAssimpOption() { return kDefaultAssimpOption_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	MeshContainer     meshes_;
	MaterialContainer materials_;

	BornNode root_;
	Skeleton skeleton_;

	//* parameter *//

	static inline const uint32_t kDefaultAssimpOption_
		= aiProcess_FlipWindingOrder
		| aiProcess_FlipUVs
		| aiProcess_Triangulate
		| aiProcess_CalcTangentSpace
		| aiProcess_ImproveCacheLocality;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper convert methods *//

	static Vector3f ConvertNormal(const aiVector3D& aiVector);
	static Vector3f ConvertPosition3(const aiVector3D& aiVector);
	static Vector4f ConvertPosition4(const aiVector3D& aiVector);
	static Quaternion ConvertQuaternion(const aiQuaternion& aiQuaternion);

	static std::optional<std::filesystem::path> GetTextureFilepath(const aiMaterial* aiMaterial, aiTextureType type, const std::filesystem::path& directory);

	//* load helper methods *//

	void LoadMesh(const aiMesh* aiMesh, AssimpMesh& mesh);
	void LoadMeshes(const aiScene* aiScene);

	void LoadMaterial(const aiMaterial* aiMaterial, const std::filesystem::path& directory, AssimpMaterial& material);
	void LoadMaterials(const aiScene* aiScene, const std::filesystem::path& directory);

	BornNode ReadNode(aiNode* node);


};
