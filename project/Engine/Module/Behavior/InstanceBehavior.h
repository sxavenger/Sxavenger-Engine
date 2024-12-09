#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "BaseBehavior.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/Content/Model/Model.h>
#include <Engine/Module/Transform/Transform.h>
#include <Engine/Module/Material/MaterialComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// InstanceBehavior class
////////////////////////////////////////////////////////////////////////////////////////////
class InstanceBehavior
	: public BaseBehavior, public MaterialComponent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	InstanceBehavior()  { Init(); }
	~InstanceBehavior() = default;

	void Init();

	void CreateInstance(uint32_t size);

	//* derivative behaivor methods *//
	//* Draw methods

	virtual void DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame* frame);
	virtual void DrawAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame);
	virtual void DrawLateAdaptive(_MAYBE_UNUSED const SxavGraphicsFrame* frame);

	//* getter *//

	const TransformationMatrix& GetMatrix(uint32_t index) const { return (*matrixInstance_)[index]; }
	TransformationMatrix& GetMatrix(uint32_t index) { return (*matrixInstance_)[index]; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	Model* model_;

	std::unique_ptr<DxObject::DimensionBuffer<TransformationMatrix>> matrixInstance_;

};

