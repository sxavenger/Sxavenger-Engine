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

	void AddGeometry(AGeometryActor* geometry);

	void AddLight(ALightActor* light);

	//* raytracing option *//

	void SetupTopLevelAS(const DirectXThreadContext* context);

	//* getter *//

	const std::list<AGeometryActor*>& GetGeometries() const { return geometries_; }

	const std::list<ALightActor*>& GetLights() const { return lights_; }

	const DxrObject::TopLevelAS& GetTopLevelAS() const { return topLevelAS_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* geometry actors *//

	std::list<AGeometryActor*> geometries_;
	//* sceneに描画されるactorのリスト

	DxrObject::TopLevelAS topLevelAS_;

	//* light actors *//

	std::list<ALightActor*> lights_;
	//* sceneに描画されるactorのリスト

	

};
