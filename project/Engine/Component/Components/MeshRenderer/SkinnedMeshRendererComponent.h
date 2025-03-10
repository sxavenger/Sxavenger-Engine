#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../BaseComponent.h"
#include "../Transform/TransformComponent.h"

//* engine
#include <Engine/Content/InputAssembler/InputMesh.h>
#include <Engine/Content/Material/Material.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SkinnedMeshRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
class SkinnedMeshRendererComponent final
	: public BaseComponent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// InputSkinnedMesh structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct InputSkinnedMesh {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================



	};


public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	SkinnedMeshRendererComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~SkinnedMeshRendererComponent() override = default;

	const TransformComponent* GetTransform() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================



};
