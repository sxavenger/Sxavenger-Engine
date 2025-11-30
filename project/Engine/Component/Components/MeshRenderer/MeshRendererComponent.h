#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* mesh renderer
#include "MeshRendererCommon.h"

//* component
#include "../BaseComponent.h"
#include "../Transform/TransformComponent.h"

//* engine
#include <Engine/Preview/Asset/AssetMesh.h>
#include <Engine/Preview/Asset/AssetMaterial.h>
#include <Engine/Preview/Asset/AssetParameter.h>

//* lib
#include <Lib/Sxl/Flag.h>

//* c++
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 3Dメッシュの描画component.
class MeshRendererComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	MeshRendererComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~MeshRendererComponent() override = default;

	void ShowComponentInspector() override;

	//* setter *//

	void SetMesh(const Uuid& mesh) { mesh_ = mesh; }
	void SetMesh(const std::shared_ptr<AssetMesh>& mesh) { mesh_ = mesh; }

	void SetMaterial(const Uuid& material) { material_ = material; }
	void SetMaterial(const std::shared_ptr<AssetMaterial>& material) { material_ = material; }

	void SetEnable(bool isEnable = true) { isEnable_ = isEnable; }

	//* getter *//

	bool IsEnable() const { return !mesh_.Empty() && !material_.Empty() && isEnable_; }

	std::shared_ptr<AssetMesh> GetMesh() const;
	std::shared_ptr<AssetMaterial> GetMaterial() const;

	const AssetParameter<AssetMesh>& GetMeshParameter() const { return mesh_; }
	const AssetParameter<AssetMaterial>& GetMaterialParameter() const { return material_; }

	uint8_t GetMask() const { return mask_.Get(); }

	//* behaviour option *//

	//! @brief TransformComponentを取得
	//! @throw TransformComponentがEntityに存在しない場合に例外をスローする
	const TransformComponent* RequireTransform() const;

	//* json option *//

	json PerseToJson() const override;

	void InputJson(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	bool isEnable_ = true;

	//* parameter *//

	AssetParameter<AssetMesh> mesh_;
	AssetParameter<AssetMaterial> material_;

	//* config *//

	Sxl::Flag<MeshInstanceMask> mask_ = MeshInstanceMask::Default;

	uint8_t stencil_ = 0;

};
