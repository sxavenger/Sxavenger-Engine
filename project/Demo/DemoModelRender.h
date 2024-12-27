#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Module/Behavior/ModelBehavior.h>

//* engine
#include <Engine/Asset/SxavengerAsset.h>
#include <Engine/Module/Collider/Collider.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DemoModelRender class
////////////////////////////////////////////////////////////////////////////////////////////
class DemoModelRender
	: public ModelBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	DemoModelRender()  = default;
	~DemoModelRender() = default;

	void Init(const std::filesystem::path& filepath, bool isUseMeshShader);

private:
};
