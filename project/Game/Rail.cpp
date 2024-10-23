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
		{ 2.0f, 0.0f, 4.0f },
	};

	SetName("rail");

	model_ = SxavengerGame::LoadModel("resources/model", "rail.obj");
	CreateInstance(kInstanceSize_);

	for (uint32_t i = 0; i < kInstanceSize_; ++i) {
		EulerTransform transform;
		transform.scale = { 0.1f, 0.01f, 0.1f };
		// FIXME: 次のrailの長さをz軸scaleとして

		transform.translate = LoopCatmullRomPosition(static_cast<float>(i) / kInstanceSize_);
		Vector3f nextPosition = LoopCatmullRomPosition(static_cast<float>(i + 1) / kInstanceSize_);

		Vector3f direction = Normalize(nextPosition - transform.translate);
		transform.rotate = CalculateEuler(direction);

		transform.translate.y -= 0.1f;

		(*instanceBuffer_)[i].Transfer(transform.ToMatrix());
	}
}

void Rail::Term() {
}

void Rail::DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) {
	DrawCatmullrom(128);
	SxavengerGame::DrawToScene(camera);
}

void Rail::SetAttributeImGui() {

	for (uint32_t i = 0; i < points_.size(); ++i) {
		std::string label = "position ##" + std::format("{:p}", reinterpret_cast<void*>(&points_[i]));
		ImGui::DragFloat3(label.c_str(), &points_[i].x, 0.01f);
	}
}

Vector3f Rail::LoopCatmullRomPosition(float t) {
	Assert(points_.size() >= 3, "rail points must over 3.");

	// 区間数
	size_t division = points_.size();

	// 1区間の長さ
	float areaWidth = 1.0f / division;

	// 区間内の始点を0.0f, 終点を1.0fとしたときの現在の位置
	float divisionT = std::fmod(t, areaWidth) * division;
	divisionT = std::clamp(divisionT, 0.0f, 1.0f);

	// 区間番号
	size_t index = static_cast<size_t>(t / areaWidth);
	index = std::clamp<size_t>(index, 0, points_.size() - 1);

	size_t index0 = index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;

	// 区間が始点の場合
	if (index == 0) {
		index0 = points_.size() - 1; //!< ループさせるので
	}

	index2 %= points_.size();
	index3 %= points_.size();

	const Vector3f& p0 = points_[index0];
	const Vector3f& p1 = points_[index1];
	const Vector3f& p2 = points_[index2];
	const Vector3f& p3 = points_[index3];

	return CatmullRomInterpolation(p0, p1, p2, p3, divisionT);
}

void Rail::DrawCatmullrom(uint32_t kSubdivision) {
	for (uint32_t i = 0; i < kSubdivision; ++i) {

		float t = static_cast<float>(i) / kSubdivision;
		float nextT = static_cast<float>(i + 1) / kSubdivision;

		SxavengerGame::DrawLine(
			LoopCatmullRomPosition(t),
			LoopCatmullRomPosition(nextT),
			ToColor4f(0xFA0000FF)
		);
	}
}
