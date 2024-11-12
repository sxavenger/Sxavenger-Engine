#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/Game/Behavior/BaseBehavior.h>

//* engine
#include <Engine/System/DxObject/DxShaderBlob.h>
#include <Engine/System/DxObject/DxGraphicsPipeline.h>
#include <Engine/Game/InputAssembler.h>

//* lib
#include <Lib/Geometry/Vector3.h>

//* c++
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////
// RailElement structure
////////////////////////////////////////////////////////////////////////////////////////////
struct RailElement {
	Vector3f position;
	Vector3f upVector;
};


////////////////////////////////////////////////////////////////////////////////////////////
// Rail class
////////////////////////////////////////////////////////////////////////////////////////////
class Rail
	: public BaseBehavior {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Rail() = default;
	~Rail() = default;

	void Init();

	void Term();

	void Update();

	void DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) override;

	void SetAttributeImGui() override;

	//* getter *//

	Vector3f LoopCatmullRomPosition(float t);
	//RailElement LoopCatmullRomRailElement(float t);

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// RailIA structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct RailIA {
		Vector4f positionRight;
		Vector4f positionLeft;
		Color4f  color;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* rail *//

	std::vector<Vector3f> points_;

	const std::string kFilename = "rail.json";

	//* behavior *//

	static const uint32_t kInstanceSize_ = 128;

	//* draw variables *//

	std::unique_ptr<DxObject::GraphicsBlob>     blob_;
	std::unique_ptr<DxObject::GraphicsPipeline> pipeline_;

	static const uint32_t kMaxVertexCount_ = 514;
	static const uint32_t kPrimitiveCount_ = 2;

	InputAssembler<RailIA> ia_;

	uint32_t inputCount_  = 0;
	uint32_t inputOffset_ = 0;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void DrawCatmullrom(uint32_t kSubdivision);
	//void DrawCatmullromRail(uint32_t kSubdivision);

	void CreatePipelineAndIA();

	//* IA option *//

	void BindIA(const Vector3f& positionLeft, const Vector3f& positionRight, const Color4f& color);
	void DrawToScene(const Camera3D* camera);

	void LoadJson();
	void OutputJson();
};