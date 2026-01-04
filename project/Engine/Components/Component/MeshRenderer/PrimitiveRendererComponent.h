#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* mesh renderer
#include "MeshRendererCommon.h"

//* component
#include "../BaseComponent.h"
#include "../Transform/TransformComponent.h"

//* engine
#include <Engine/Graphics/InputGeometry/InputPrimitive.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief プリミティブ描画component.
class PrimitiveRendererComponent final
	: public BaseComponent, public MeshRendererCommon {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PrimitiveRendererComponent(EntityBehaviour* behaviour) : BaseComponent(behaviour) {}
	~PrimitiveRendererComponent() override = default;

	void ShowComponentInspector() override;

	//* setter *//

	void SetPrimitive(const InputPrimitive* primitive) { primitive_ = primitive; }

	//* getter *//

	const InputPrimitive* GetPrimitive() const { return primitive_; }

	//* behaviour option *//

	//! @brief TransformComponentを取得
	//! @return TransformComponentのptr
	//! @throw TransformComponentがEntityに存在しない場合に例外をスローする
	const TransformComponent* RequireTransform() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	const InputPrimitive* primitive_ = nullptr;
	//!< todo: primitive material.

};

SXAVENGER_ENGINE_NAMESPACE_END
