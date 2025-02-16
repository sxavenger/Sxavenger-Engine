#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include "../AActor.h"
#include "../Camera/ACameraActor.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxBindBuffer.h>
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/Content/Component/Transform2dComponent.h>
#include <Engine/Content/Component/MaterialComponent.h>

//* lib
#include <Lib/CXXAttributeConfig.h>
#include <Lib/Geometry/Vector2.h>
#include <Lib/Sxl/Holder.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AGeometryActor class
////////////////////////////////////////////////////////////////////////////////////////////
class AGeometryActor //!< geometry actor
	: public AActor, public Transform2dComponent {
	//* Geometry(3d描画)を持つActor
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Container = std::list<AGeometryActor*>;
	using Iterator  = Container::iterator;

	////////////////////////////////////////////////////////////////////////////////////////////
	// RendererContext structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RendererContext {
		//* 描画情報
		const DirectXThreadContext* context;
		Vector2ui size;                      //!< 描画サイズ

		//* 対象情報
		MaterialComponent::BlendMode target; //!< 対象

		DxObject::BindBufferDesc parameter; //!< パラメータ
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// SetupContext structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SetupContext {
		//* セットアップ情報
		const DirectXThreadContext* context; //!< DirectXスレッドコンテキスト
		DxrObject::TopLevelAS*      toplevelAS;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AGeometryActor()          = default;
	virtual ~AGeometryActor() = default;

	//* render *//

	//virtual void RenderOpaque(const RendererContext& context)      = 0;

	//virtual void RenderTransparent(const RendererContext& context) = 0;

	virtual void Render(const RendererContext& context) = 0;

	//* raytracing option *//

	virtual void SetupToplevelAS(const SetupContext& context) = 0;

	//* children option *//

	Iterator AddChild(AGeometryActor* child);

	void ClearChildren();

	const Container& GetChildren() const { return children_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* parameter *//

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Container children_; //!< 子Actor

};
