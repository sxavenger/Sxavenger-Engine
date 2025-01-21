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
	// Category enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Category : uint8_t {
		Default,
		PBR, //* 未実装
	};


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

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	Category category_ = Category::Default;
	//? scene rendererにおいて, PBRとDefaultを使い分けるかどうか

	//=========================================================================================
	// protected methods
	//=========================================================================================


	// todo:

	virtual void RenderDefault(const RendererContext& context) { context; }

	virtual void RenderPBR(const RendererContext& context) { context; }


};
