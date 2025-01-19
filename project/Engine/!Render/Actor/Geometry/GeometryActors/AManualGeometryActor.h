#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include "../AGeometryActor.h"

//* engine
#include <Engine/Content/InputAssembler/InputMesh.h>
#include <Engine/Asset/Texture/AssetTexture.h>
#include <Engine/Asset/Observer/AssetObserver.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AManualGeometryActor class
////////////////////////////////////////////////////////////////////////////////////////////
class AManualGeometryActor
	: public AGeometryActor {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AManualGeometryActor()          = default;
	virtual ~AManualGeometryActor() = default;

	void Init();

	//* render *//

	virtual void RenderOpaque(const RendererContext& context) override;

	virtual void RenderTransparent(const RendererContext& context) override;

	virtual void SetupToplevelAS(const SetupContext& context) override;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	InputMesh                   ia_;
	AssetObserver<AssetTexture> texture_;
	// todo: variant AssertObserver || D3D12_GPU_DESCRIPTOR_HANDLE
	
};
