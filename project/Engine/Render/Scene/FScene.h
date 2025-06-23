#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include <Engine/Component/Components/ComponentStorage.h>
#include <Engine/Component/Components/MeshRenderer/MeshRendererComponent.h>
#include <Engine/Component/Components/MeshRenderer/SkinnedMeshRendererComponent.h>

//* engine
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>
#include <Engine/System/DirectX/DxrObject/DxrStateObjectContext.h>
#include <Engine/System/DirectX/DirectXContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FScene class
////////////////////////////////////////////////////////////////////////////////////////////
class FScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	//* tlas option *//

	void SetupTopLevelAS(const DirectXThreadContext* context);

	const DxrObject::TopLevelAS& GetTopLevelAS() const { return topLevelAS_; }

	//* state object option *//

	void SetupStateObject();

	const DxrObject::StateObjectContext& GetStateObjectContext() const { return stateObjectContext_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* raytracing parameter *//

	DxrObject::TopLevelAS topLevelAS_;
	DxrObject::StateObjectContext stateObjectContext_;

	//* light parameter *//



};
