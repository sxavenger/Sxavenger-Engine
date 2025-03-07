#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include <Engine/Component/ComponentStorage.h>
#include <Engine/Component/Components/MeshRenderer/MeshRendererComponent.h>

//* engine
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>
#include <Engine/System/DirectX/DirectXContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// FScene class
////////////////////////////////////////////////////////////////////////////////////////////
class FScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Create();

	void Term();

	void SetupTopLevelAS(const DirectXThreadContext* context);

	const DxrObject::TopLevelAS* GetTopLevelAS() const { return topLevelAS_.get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxrObject::TopLevelAS> topLevelAS_;

};
