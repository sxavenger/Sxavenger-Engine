#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include "../ALightActor.h"

//* lib
#include <Lib/Geometry/Color.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ASpotLightActor class
////////////////////////////////////////////////////////////////////////////////////////////
class ASpotLightActor
	: public ALightActor {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Parameter structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Parameter {

		//* member *//

		Color3f color;
		float intensity;
		float distance;
		float falloff;
		float angle;

		//* method *//

		void Init() {
			color     = { 1.0f, 1.0f, 1.0f };
			intensity = 1.0f;
			distance  = 8.0f;
			falloff   = std::cos(pi_v);
			angle     = std::cos(pi_v / 3.0f);
		}
		
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ASpotLightActor()          = default;
	virtual ~ASpotLightActor() = default;

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

	std::unique_ptr<DxObject::DimensionBuffer<Parameter>> cb_;

};
