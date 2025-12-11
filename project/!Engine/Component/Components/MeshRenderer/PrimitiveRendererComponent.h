#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../BaseComponent.h"
#include "../Transform/TransformComponent.h"

//* engine
#include <Engine/Content/InputGeometry/InputPrimitive.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveRendererComponent class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief プリミティブ描画component.
class PrimitiveRendererComponent final
	: public BaseComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	PrimitiveRendererComponent(MonoBehaviour* behaviour) : BaseComponent(behaviour) {}
	~PrimitiveRendererComponent() override = default;

	void ShowComponentInspector() override;

	//* setter *//

	void SetPrimitive(const InputPrimitive* primitive) { primitive_ = primitive; }

	void SetTransform(const TransformComponent* transform) { transform_ = transform; }

	//* getter *//

	const InputPrimitive* GetPrimitive() const { return primitive_; }

	const TransformComponent* GetTransform() const;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	const InputPrimitive* primitive_ = nullptr;
	//!< todo: primitive material.

	const TransformComponent* transform_ = nullptr;

};
