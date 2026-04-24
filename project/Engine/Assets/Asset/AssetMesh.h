#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "BaseAsset.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>
#include <Engine/Graphics/InputGeometry/InputMesh.h>
#include <Engine/Graphics/Animation/JointWeight.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//* c++
#include <unordered_map>
#include <optional>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Asset namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Asset {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mesh class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Mesh final
		: public BaseAsset {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor / destructor *//

		Mesh(const Uuid& id) : BaseAsset(id) {}

		~Mesh() override = default;

		//* setup option *//

		void Setup(const DirectXQueueContext* context, const aiMesh* mesh);

		//* mesh option *//

		const InputMesh& GetInputMesh() const;

		const std::unordered_map<std::string, JointWeightData>& GetJointWeights() const { return jointWeights_; }
		// hack: meshの読み込みが完了してから取得する.

		const std::string& GetName() const { return name_; }
		// hack: meshの読み込みが完了してから取得する.

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		//* mesh data
		InputMesh                                        input_;
		std::unordered_map<std::string, JointWeightData> jointWeights_;
		std::string                                      name_;

		//=========================================================================================
		// private methods
		//=========================================================================================

		//* helper convert methods *//

		static Vector3f ConvertVector3(const aiVector3D& aiVector);

		static Vector4f ConvertVector4(const aiVector3D& aiVector);

		static Quaternion ConvertQuaternion(const aiQuaternion& aiQuaternion);

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
