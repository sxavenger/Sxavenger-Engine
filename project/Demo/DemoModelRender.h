#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Module/Behavior/ModelBehavior.h>

//* engine
#include <Engine/Asset/SxavengerAsset.h>

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

	void DrawSystematic(_MAYBE_UNUSED const SxavGraphicsFrame* frame) override;

private:
};
