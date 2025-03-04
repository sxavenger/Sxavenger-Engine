#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include "../AActor.h"
#include "../Camera/ACameraActor.h"

//* render
#include <Engine/Render/FTexture.h>
#include <Engine/Render/FSceneDepth.h>

//* engine
#include <Engine/System/DirectX/DxObject/DxBindBuffer.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/Sxl/Flag.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ALightActor class
////////////////////////////////////////////////////////////////////////////////////////////
class ALightActor //!< light actor
	: public AActor {
	//* Light(光源)を持つActor
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Container = std::list<ALightActor*>;
	using Iterator  = Container::iterator;

	////////////////////////////////////////////////////////////////////////////////////////////
	// RendererContext structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RendererContext {
		//* 描画情報
		const DirectXThreadContext* context; //!< DirectXスレッドコンテキスト
		Vector2ui size;                      //!< 描画サイズ

		DxObject::BindBufferDesc parameter;
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// RayQueryShadow structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RayQueryShadow {
	public:

		//* member *//

		float strength;                                //!< 影の強さ
		Sxl::FlagMask<D3D12_RAY_FLAGS, uint32_t> flag; //!< フラグ

		//* method *//

		void Init() {
			strength = 1.0f;
			flag     = D3D12_RAY_FLAG_CULL_BACK_FACING_TRIANGLES;
		}
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

	const RayQueryShadow& GetShadow() const;
	RayQueryShadow& GetShadow();

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//Category category_ = Category::Default;
	//? scene rendererにおいて, PBRとDefaultを使い分けるかどうか

	std::unique_ptr<DxObject::DimensionBuffer<RayQueryShadow>> shadow_; //!< shadow parameter

};
