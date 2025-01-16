#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include "../ALightActor.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/Content/InputAssembler/TriangleInputAssembler.h>

//* lib
#include <Lib/Geometry/Vector4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// APointLightActor class
////////////////////////////////////////////////////////////////////////////////////////////
class APointLightActor
	: public ALightActor {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {
		Color4f color_intensity; //!< rgb: color, a: intensity
		float radius;

		//* method *//

		void Init() {
			color_intensity = { 1.0f, 1.0f, 1.0f, 1.0f };
			radius          = 10.0f;
		}

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	APointLightActor()          = default;
	virtual ~APointLightActor() = default;

	void Init();

	//* render *//

	virtual void Render(const RendererContext& context) override;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> cb_; //!< constant buffer

private: //!< TEST CODE
	TriangleInputAssembler<Vector4f> ia_;
	std::unique_ptr<DxObject::ReflectionGraphicsPipelineState> pipeline_;

	void TestInit();
	void TestRender(const RendererContext& context);

};
