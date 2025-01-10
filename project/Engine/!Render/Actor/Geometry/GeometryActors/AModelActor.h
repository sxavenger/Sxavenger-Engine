#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* actor
#include "../AGeometryActor.h"

//* engine
//#include <Engine/Asset/ModelAnimator/Model/Model.h>

////////////////////////////////////////////////////////////////////////////////////////////
// AModelActor class
////////////////////////////////////////////////////////////////////////////////////////////
class AModelActor
	: public AGeometryActor {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AModelActor()          = default;
	virtual ~AModelActor() = default;

	void Init();

	//* render *//

	virtual void RenderOpaque() override;
	virtual void RenderTransparent() override;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//std::shared_ptr<Model> model_;
	// hack: observer

};
