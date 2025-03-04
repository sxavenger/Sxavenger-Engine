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
// AModelActor class
////////////////////////////////////////////////////////////////////////////////////////////
class AModelActor
	: public AGeometryActor {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AModelActor()          = default;
	virtual ~AModelActor() = default;

	void Init();

	//* render *//

	virtual void Render(const RendererContext& context) override;

	//* raytracing option *//

	virtual void SetupToplevelAS(const SetupContext& context) override;

	//* setter *//

	void SetModel(const AssetObserver<AssetModel>& model) { model_ = model; }

	void SetRenderWait(bool isRenderWait) { isRenderWait_ = isRenderWait; }

	//* inspector *//

	virtual void InspectorImGui() override;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* asset *//

	AssetObserver<AssetModel> model_;

	//* option *//

#ifdef _DEVELOPMENT
	bool isRenderWait_ = false;
#else
	bool isRenderWait_ = true;
#endif

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void RenderOpaque(const RendererContext& context, const DxObject::BindBufferDesc& parameter);

	void RenderTranslucent(const RendererContext& context, const DxObject::BindBufferDesc& parameter);

};
