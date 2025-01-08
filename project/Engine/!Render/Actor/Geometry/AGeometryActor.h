#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include "../AActor.h"

//* engine
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>

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

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AGeometryActor()          = default;
	virtual ~AGeometryActor() = default;

	//* render *//

	virtual void RenderOpaque()      = 0;
	virtual void RenderTransparent() = 0;
	//* 描画情報を引数に渡す

	//* getter *//

	const Transparency GetTransparency() const { return transparency_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* parameter *//

	Transparency transparency_ = Transparency::Opaque;

	//* raytracing *//

	DxrObject::BottomLevelAS bottomLevelAS_;
	// todo: raytracing

};
