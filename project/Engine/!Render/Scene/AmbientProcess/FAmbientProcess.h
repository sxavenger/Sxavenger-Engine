#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../../FSceneTextures.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxBindBuffer.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* c++
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// FAmbientProcess class
////////////////////////////////////////////////////////////////////////////////////////////
class FAmbientProcess {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Container = std::list<FAmbientProcess*>;
	using Iterator  = Container::iterator;

	////////////////////////////////////////////////////////////////////////////////////////////
	// ProcessContext structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ProcessContext {
		//* system *//
		const DirectXThreadContext* context;
		Vector2ui                   size;

		//* textures *//
		FSceneTextures* textures;

		//* parameter *//
		DxObject::BindBufferDesc parameter;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FAmbientProcess()          = default;
	virtual ~FAmbientProcess() = default;

	//* process *//

	virtual void Process(const ProcessContext& context) = 0;

	//* debug *//

	virtual void SetImGuiCommand() {}

private:
};
