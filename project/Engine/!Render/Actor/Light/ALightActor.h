#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include "../AActor.h"
#include "../Camera/ACameraActor.h"

//* render
#include <Engine/!Render/FTexture.h>
#include <Engine/!Render/FSceneDepth.h>

//* engine
#include <Engine/System/DirectX/DxObject/DxBindBuffer.h>
#include <Engine/System/DirectX/DirectXContext.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ALightActor class
////////////////////////////////////////////////////////////////////////////////////////////
class ALightActor //!< light actor
	: public AActor {
	//* Light(光源)を持つActor
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// RendererContext structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RendererContext {
		//* 描画情報
		const DirectXThreadContext* context; //!< DirectXスレッドコンテキスト
		Vector2ui size;                      //!< 描画サイズ

		DxObject::BindBufferDesc parameter;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ALightActor()          = default;
	virtual ~ALightActor() = default;

	//* render *//

	virtual void Render(const RendererContext& context) = 0; //!< DeferredでのLight効果範囲描画

	//* getter *//

	bool IsActive() const { return isActive_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isActive_ = true;

};
