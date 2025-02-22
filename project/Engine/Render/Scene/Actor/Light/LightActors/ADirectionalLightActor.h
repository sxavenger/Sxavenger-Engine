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
#include <Lib/Geometry/Color.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ADirectionalLightActor class
////////////////////////////////////////////////////////////////////////////////////////////
class ADirectionalLightActor
	: public ALightActor {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {

		//* member *//

		Color3f color;           //!< color
		float   intensity;       //!< intensity

		//* method *//

		void Init() {
			color     = { 1.0f, 1.0f, 1.0f };
			intensity = 1.0f;
		}
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ADirectionalLightActor()          = default;
	virtual ~ADirectionalLightActor() = default;

	void Init();

	//* render *//

	virtual void Render(const RendererContext& context) override;

	//* inspector *//

	virtual void InspectorImGui() override;

	//* getter *//

	const Parameter& GetParameter() const { return cb_->At(0); }
	Parameter& GetParameter() { return cb_->At(0); }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> cb_; //!< constant buffer

};
