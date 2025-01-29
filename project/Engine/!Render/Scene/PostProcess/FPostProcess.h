#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "FPostProcessTextures.h"
#include "../../FSceneTextures.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxBindBuffer.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* c++
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// FPostProcess class
////////////////////////////////////////////////////////////////////////////////////////////
class FPostProcess {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Contanier = std::list<FPostProcess*>;
	using Iterator  = Contanier::iterator;

	////////////////////////////////////////////////////////////////////////////////////////////
	// ProcessContext structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ProcessContext {
		const DirectXThreadContext* context;
		Vector2ui                   size;
		FPostProcessTextures*       textures;

		DxObject::BindBufferDesc parameter;

		//* src *//
		const FSceneTextures* sceneTextures;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FPostProcess()          = default;
	virtual ~FPostProcess() = default;

	//* process *//

	virtual void Process(const ProcessContext& context) = 0;

	//* debug *//

	virtual void SetImGuiCommand() {}

private:

	//=========================================================================================
	// private variables
	//=========================================================================================



};
