#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* core
#include "Core/FRenderCoreGeometry.h"
#include "Core/FRenderCoreLight.h"

////////////////////////////////////////////////////////////////////////////////////////////
// FRenderCore class
////////////////////////////////////////////////////////////////////////////////////////////
class FRenderCore {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FRenderCore()  = default;
	~FRenderCore() = default;

	void Init();

	void Term();

	//* getter *//

	FRenderCoreGeometry* GetGeometry() { return geometry_.get(); }

	FRenderCoreLight* GetLight() { return light_.get(); }

	//* singleton *//

	static FRenderCore* GetInstance();
	
private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<FRenderCoreGeometry> geometry_;
	std::unique_ptr<FRenderCoreLight>    light_;


};
