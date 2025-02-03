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
#include <Engine/Module/Component/TextureComponent.h>

//* lib
#include <Lib/CXXAttributeConfig.h>
#include <Lib/Geometry/Vector2.h>
#include <Lib/Sxl/Holder.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AGeometryActor class
////////////////////////////////////////////////////////////////////////////////////////////
class AGeometryActor //!< geometry actor
	: public AActor, public TextureComponent {
	//* Geometry(3d描画)を持つActor
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// using
	////////////////////////////////////////////////////////////////////////////////////////////

	using Container = std::list<AGeometryActor*>;
	using Iterator  = Container::iterator;

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
		const DirectXThreadContext* context;
		Vector2ui size;                      //!< 描画サイズ

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

	virtual void RenderOpaque(const RendererContext& context)      = 0;

	virtual void RenderTransparent(const RendererContext& context) = 0;
	//* 描画情報を引数に渡す

	bool CheckVisibility(Transparency target) const;

	//* raytracing option *//

	virtual void SetupToplevelAS(const SetupContext& context) = 0;

	//* accessor *//

	const Transparency GetTransparency() const { return transparency_; }

	void SetTransparency(Transparency transparency) { transparency_ = transparency; }

	//* children option *//

	Iterator AddChild(AGeometryActor* child);

	void ClearChildren();

	const Container& GetChildren() const { return children_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* parameter *//

	Transparency transparency_ = Transparency::Opaque;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Container children_; //!< 子Actor

};
