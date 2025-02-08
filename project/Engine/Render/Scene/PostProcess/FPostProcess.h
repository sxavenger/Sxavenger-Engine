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
		//* system *//
		const DirectXThreadContext* context;
		Vector2ui                   size;

		//* textures *//
		FPostProcessTextures*       textures;

		//* parameter *//
		DxObject::BindBufferDesc parameter;
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

	//* getter *//

	bool IsActive() const { return isActive_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isActive_ = true;

};
