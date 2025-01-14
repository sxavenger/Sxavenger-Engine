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

	virtual void RenderOpaque(const RendererContext& context) override;
	virtual void RenderTransparent(const RendererContext& context) override;

	//* setter *//

	void SetModel(const AssetObserver<AssetModel>& model) { model_ = model; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	AssetObserver<AssetModel> model_;

};
