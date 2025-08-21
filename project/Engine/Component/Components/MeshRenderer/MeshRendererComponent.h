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
#include <Engine/Preview/Asset/UAssetMesh.h>
#include <Engine/Preview/Asset/UAssetMaterial.h>
#include <Engine/Preview/Asset/UAssetParameter.h>

//* lib
#include <Lib/Sxl/Flag.h>

//* c++
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// MeshRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
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
	void SetMesh(const std::shared_ptr<UAssetMesh>& mesh) { mesh_ = mesh; }

	void SetMaterial(const Uuid& material) { material_ = material; }
	void SetMaterial(const std::shared_ptr<UAssetMaterial>& material) { material_ = material; }

	//* getter *//

	bool IsEnabled() const { return !mesh_.Empty() && !material_.Empty() && isEnable_; }

	std::shared_ptr<UAssetMesh> GetMesh() const;
	std::shared_ptr<UAssetMaterial> GetMaterial() const;

	const TransformComponent* GetTransform() const;

	uint8_t GetMask() const { return mask_.Get(); }

	//* json option *//

	json PerseToJson() const override;

	void InputJson(const json& data) override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	bool isEnable_ = true;

	//* parameter *//

	UAssetParameter<UAssetMesh> mesh_;
	UAssetParameter<UAssetMaterial> material_;

	//* config *//

	Sxl::Flag<MeshInstanceMask> mask_ = MeshInstanceMask::Default;

};
