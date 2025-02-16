#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../Texture/Texture.h"

//* engine
#include <Engine/Content/Component/MaterialComponent.h>
#include <Engine/Content/InputAssembler/InputMesh.h>
#include <Engine/Content/Animation/JointWeight.h>
#include <Engine/Content/Animation/BornNode.h>

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
	// Mesh structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Mesh {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		InputMesh                                        input;
		std::optional<uint32_t>                          materialIndex;
		std::unordered_map<std::string, JointWeightData> jointWeights;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Material structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Material
		: public MaterialComponent {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// TextureType enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class TextureType : uint8_t {
			Diffuse,
			Bump,
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void CreateComponent();

		//=========================================================================================
		// public variables
		//=========================================================================================

		std::array<std::shared_ptr<Texture>, static_cast<uint8_t>(TextureType::Bump) + 1> textures_;

		// FIXME: ここにMaterialComponentを持たせるのはModelクラスの定義制約に反する.
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// using 
	////////////////////////////////////////////////////////////////////////////////////////////

	using MeshContainer     = std::vector<Mesh>;
	using MaterialContainer = std::vector<Material>;

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Model()  = default;
	~Model() { Term(); }

	void Load(const DirectXThreadContext* context, const std::filesystem::path& filepath, uint32_t assimpOption = kDefaultAssimpOption_);

	void Term();

	//* material option *//

	const MaterialContainer& GetMaterials() const { return materials_; }
	MaterialContainer& GetMaterials() { return materials_; }

	const Material& GetMaterial(uint32_t index) const;
	Material& GetMaterial(uint32_t index);

	//* meshes option *//

	void CreateBottomLevelAS(const DirectXThreadContext* context);

	const uint32_t GetMeshSize() const { return static_cast<uint32_t>(meshes_.size()); }

	const MeshContainer& GetMeshes() const { return meshes_; }
	MeshContainer& GetMeshes() { return meshes_; }

	const Mesh& GetMesh(uint32_t index) const;
	Mesh& GetMesh(uint32_t index);

	//* mesh option *//

	void SetIABuffer(const DirectXThreadContext* context, uint32_t meshIndex) const;

	void DrawCall(const DirectXThreadContext* context, uint32_t meshIndex, uint32_t instanceCount = 1) const;

	uint32_t GetMaterialIndex(uint32_t meshIndex) const;

	const Material& GetMeshMaterial(uint32_t meshIndex) const;

	//* root option *//

	const BornNode& GetRoot() const { return root_; }

	//* parameter option *//

	static const uint32_t GetDefaultAssimpOption() { return kDefaultAssimpOption_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	MeshContainer     meshes_;
	MaterialContainer materials_;

	BornNode root_;

	//* parameter *//

	static const uint32_t kDefaultAssimpOption_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* sub methods *//

	static Vector3f ConvertNormal(const aiVector3D& aiVector);
	static Vector3f ConvertPosition3(const aiVector3D& aiVector);
	static Vector4f ConvertPosition4(const aiVector3D& aiVector);
	static Quaternion ConvertQuaternion(const aiQuaternion& aiQuaternion);

	//* mesh methods *//

	void LoadMesh(const aiScene* aiScene);

	void LoadMaterial(const aiScene* aiScene, const DirectXThreadContext* context, const std::filesystem::path& directory);

	BornNode ReadNode(aiNode* node);

	bool CheckMeshIndex(uint32_t meshIndex) const;
	bool CheckMaterialIndex(uint32_t materialIndex) const;

};
