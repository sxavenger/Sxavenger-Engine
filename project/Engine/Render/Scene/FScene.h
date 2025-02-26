#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include "Actor/Geometry/AGeometryActor.h"
#include "Actor/Light/ALightActor.h"

//* engine
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* c++
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// FScene class
////////////////////////////////////////////////////////////////////////////////////////////
class FScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FScene()  = default;
	~FScene() = default;

	//* actor option *//

	AGeometryActor::Iterator AddGeometry(AGeometryActor* geometry);

	ALightActor::Iterator AddLight(ALightActor* light);

	//* raytracing option *//

	void SetupTopLevelAS(const DirectXThreadContext* context);

	//* getter *//

	const AGeometryActor::Container& GetGeometries() const { return geometries_; }

	const ALightActor::Container& GetLights() const { return lights_; }

	const DxrObject::TopLevelAS& GetTopLevelAS() const { return topLevelAS_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* geometry actors *//

	AGeometryActor::Container geometries_;
	//* sceneに描画されるactorのリスト

	DxrObject::TopLevelAS topLevelAS_;

	//* light actors *//

	ALightActor::Container lights_;
	//* sceneに描画されるactorのリスト

	//=========================================================================================
	// private variables
	//=========================================================================================

	void SetupTopLevelASContainer(const AGeometryActor::SetupContext& context, const AGeometryActor::Container& container);

};
