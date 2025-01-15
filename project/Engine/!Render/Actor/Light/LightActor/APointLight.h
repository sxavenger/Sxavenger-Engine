#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include "../ALightActor.h"

//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/Content/InputAssembler/InputAssembler.h>

//* lib
#include <Lib/Geometry/Vector4.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ALightActor class
////////////////////////////////////////////////////////////////////////////////////////////
class APointLight
	: public ALightActor {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {
		Color4f color_intensity; //!< rgb: color, a: intensity
		float radius;
		float factor;

		//* method *//

		void Init() {
			color_intensity = { 1.0f, 1.0f, 1.0f, 1.0f };
			radius          = 1.0f;
			factor          = 1.0f;
		}

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	APointLight()          = default;
	virtual ~APointLight() = default;

	void Init();

	//* render *//

	virtual void Render() override;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> cb_; //!< constant buffer

private: //!< TEST CODE
	InputAssembler<Vector4f> ia_;
	std::unique_ptr<DxObject::ReflectionGraphicsPipelineState> pipeline_;

	void TestInit();
	void TestRender();

};
