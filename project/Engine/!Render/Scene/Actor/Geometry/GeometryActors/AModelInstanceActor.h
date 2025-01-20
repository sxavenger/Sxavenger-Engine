#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include "../AGeometryActor.h"

//* engine
#include <Engine/Asset/Model/AssetModel.h>
#include <Engine/Asset/Observer/AssetObserver.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AModelInstanceActor class
////////////////////////////////////////////////////////////////////////////////////////////
class AModelInstanceActor
	: public AGeometryActor {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AModelInstanceActor()          = default;
	virtual ~AModelInstanceActor() = default;

	void Init(uint32_t instanceCount);

	//* render *//

	virtual void RenderOpaque(const RendererContext& context) override;

	virtual void RenderTransparent(const RendererContext& context) override;

	//* raytracing option *//

	virtual void SetupToplevelAS(const SetupContext& context) override;

	//* setter *//

	void SetMatrix(uint32_t index, const Matrix4x4& mat) { mat_->At(index).Transfer(mat); }

	void SetModel(const AssetObserver<AssetModel>& model) { model_ = model; }

	void SetRenderWait(bool isRenderWait) { isRenderWait_ = isRenderWait; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* asset *//

	AssetObserver<AssetModel> model_;

	//* option *//

	bool isRenderWait_ = true;

	//* buffer *//

	std::unique_ptr<DxObject::DimensionBuffer<TransformationMatrix>> mat_;

};
