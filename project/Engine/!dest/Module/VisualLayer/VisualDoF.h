#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include "VisualLayer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// VisualDoF class
////////////////////////////////////////////////////////////////////////////////////////////
class VisualDoF //!< 被写界深度. Depth of Field.
	: public BaseVisualLayer {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {
		float focusLength;
		float f;
		uint32_t isDebugDoF;
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	VisualDoF()  = default;
	~VisualDoF() = default;

	void Init() override;

	void Term();

	//* interface *//

	void Process(_MAYBE_UNUSED const DirectXThreadContext* context, _MAYBE_UNUSED SxavGraphicsFrame* frame) override;

	void SetLayerImGui() override;

	//* dof option *//

	void SetForcus(const Camera3d* camera, const Vector3f& position);

	//* getter *//

	const Parameter& GetParameter() const { return (*parameter_)[0]; }
	Parameter& GetParameter() { return (*parameter_)[0]; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> parameter_;

};