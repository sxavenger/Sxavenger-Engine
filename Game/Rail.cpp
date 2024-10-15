#include "Rail.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>

//* lib
#include <Lib/MyMath.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Rail class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Rail::Init() {
	
	points_ = { //!< 山and回転
		kOrigin3,
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 2.0f, 1.0f },
		{ 0.0f, 2.0f, 2.0f },
		{ 0.0f, 0.0f, 6.0f },
		{ 0.0f, 1.0f, 7.0f },
		{ 0.0f, 2.0f, 6.0f },
		{ 0.0f, 1.0f, 5.0f },
		{ 0.0f, 0.0f, 6.0f },
		{ 0.0f, 0.0f, 8.0f },
	};

	/*points_ = {
		kOrigin3,
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 2.0f },
		{ 0.0f, 0.0f, 3.0f },
	};*/

	SetName("rail");
}

void Rail::Term() {
}

void Rail::Draw() {
	DrawCatmullrom(100);
}

void Rail::SetAttributeImGui() {

	for (uint32_t i = 0; i < points_.size(); ++i) {
		std::string label = "position ##" + std::format("{:p}", reinterpret_cast<void*>(&points_[i]));
		ImGui::DragFloat3(label.c_str(), &points_[i].x, 0.01f);
	}
}

void Rail::DrawCatmullrom(uint32_t kSubdivision) {
	for (uint32_t i = 0; i < kSubdivision; ++i) {

		float t     = static_cast<float>(i) / kSubdivision;
		float nextT = static_cast<float>(i + 1) / kSubdivision;

		SxavengerGame::DrawLine(
			CatmullRomPosition(points_, t),
			CatmullRomPosition(points_, nextT),
			ToColor4f(0xFA0000FF)
		);
	}
}
