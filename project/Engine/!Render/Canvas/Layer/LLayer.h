#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxBindBuffer.h>
#include <Engine/System/DirectX/DirectXContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// LLayer class
////////////////////////////////////////////////////////////////////////////////////////////
class LLayer {
	//* canvasに描画されるUIの基底クラス
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// RenderContext structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RenderContext {
		//* member *//
		const DirectXThreadContext* context;
		Vector2ui                   size;

		DxObject::BindBufferDesc parameter;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	LLayer()          = default;
	virtual ~LLayer() = default;

	//* render *//

	virtual void Render(const RenderContext& context) = 0;

	bool CheckVisibility() const;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isActive_ = true;
	
};
