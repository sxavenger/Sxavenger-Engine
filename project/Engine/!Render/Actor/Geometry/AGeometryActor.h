#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include "../AActor.h"
#include "../Camera/ACameraActor.h"

//* engine
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>
#include <Engine/System/DirectX/DirectXContext.h>

//* lib
#include <Lib/CXXAttributeConfig.h>
#include <Lib/Geometry/Vector2.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AGeometryActor class
////////////////////////////////////////////////////////////////////////////////////////////
class AGeometryActor //!< geometry actor
	: public AActor {
	//* Geometry(3d描画)を持つActor
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Transparency enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Transparency : uint8_t {
		Opaque,      //!< 不透明
		Transparent, //!< 半透明
	};
	// hack: material側に持たせる

	////////////////////////////////////////////////////////////////////////////////////////////
	// RendererContext structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RendererContext {
		//* 描画情報
		const DirectXThreadContext* context; //!< DirectXスレッドコンテキスト
		Vector2ui size;                      //!< 描画サイズ

		ACameraActor* camera; //!< カメラ
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AGeometryActor()          = default;
	virtual ~AGeometryActor() = default;

	//* render *//

	virtual void RenderOpaque(const RendererContext& context)      = 0;
	virtual void RenderTransparent(const RendererContext& context) = 0;
	//* 描画情報を引数に渡す

	bool CheckVisibility(Transparency target) const;

	//* getter *//

	const Transparency GetTransparency() const { return transparency_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* parameter *//

	bool isActive_ = true;

	Transparency transparency_ = Transparency::Opaque;

	//* raytracing *//

	DxrObject::BottomLevelAS bottomLevelAS_;
	// todo: raytracing

};
