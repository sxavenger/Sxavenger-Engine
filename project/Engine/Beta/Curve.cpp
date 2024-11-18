#include "Curve.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <algorithm>

//* engine
#include <Engine/System/Logger.h>

//* lib
#include <Lib/Adapter/Json.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Curve class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Curve::TEST_Create() {
	points_ = {
		{ 0.0f / 4.0f, 0.0f / 4.0f },
		{ 1.0f / 4.0f, 1.0f / 4.0f },
		{ 2.0f / 4.0f, 2.0f / 4.0f },
		{ 3.0f / 4.0f, 3.0f / 4.0f },
		{ 4.0f / 4.0f, 4.0f / 4.0f },
	};
}

float Curve::GetOutput(float inputT) const {
	Assert(points_.size() >= 4); //!< 制御pointが少ない

	if (inputT <= points_.front().input) {
		return points_.front().output; //!< 先頭よりinputが小さいので先頭outputの返却
	}

	if (inputT >= points_.back().input) {
		return points_.back().output; //!< 最後尾よりinputが大きいので最後尾outputを返却
	}

	// 制御点を探す
	size_t segmentIndex = 0;
	while (segmentIndex < points_.size() - 2) {

		if (inputT >= points_[segmentIndex].input && inputT <= points_[segmentIndex + 1].input) {
			break;
		}

		segmentIndex++;
	}

	// indexの決定
	size_t index0 = segmentIndex - 1;
	size_t index1 = segmentIndex;
	size_t index2 = segmentIndex + 1;
	size_t index3 = segmentIndex + 2;

	if (index1 == 0) {
		index0 = 0;
	}

	if (index3 >= points_.size()) {
		index3 = index2;
	}

	const auto& p0 = points_[index0];
	const auto& p1 = points_[index1];
	const auto& p2 = points_[index2];
	const auto& p3 = points_[index3];

	// inputTをセグメント内の位置に正規化
	float t = (inputT - p1.input) / (p2.input - p1.input);

	return CatmullRomInterpolation(p0.output, p1.output, p2.output, p3.output, t);
}

void Curve::SetImGuiCommand() {

	float (*func)(void*, int) = [](void* data, int index) {
		Curve* self = static_cast<Curve*>(data);
		return self->GetOutput(index / 124.0f);
	};

	ImGui::PlotLines("plot", func, this, 124, 0, "", 0.0f, 1.0f, { 0.0f, 128.0f});

	if (ImGui::TreeNode("points")) {

		for (size_t i = 0; i < points_.size(); ++i) {

			std::string label = std::format("point {:p}", reinterpret_cast<const void*>(&points_[i]));
			if (ImGui::DragFloat2(label.c_str(), &points_[i].input, 0.01f)) {
				std::sort(
					points_.begin(), points_.end(),
					[](const CurvePoint& a, const CurvePoint& b) { return a.input < b.input; }
				);
			}
		}

		ImGui::TreePop();
	}

	ImGui::InputText("filename", buf_, _countof(buf_));

	if (ImGui::Button("output json")) {
		OutputJson(buf_);
	}

	if (ImGui::Button("load json")) {
		LoadJson(buf_);
	}

}

void Curve::OutputJson(const std::string& filename) {

	Json root = Json::object();

	auto& points = root["points"] = Json::array();

	for (size_t i = 0; i < points_.size(); ++i) {
		points.push_back({
			{ "input",  points_[i].input },
			{ "output", points_[i].output }
		});
	}

	JsonAdapter::WriteJson(directory_ + filename, root);
}

void Curve::LoadJson(const std::string& filename) {

	points_.clear();

	Json root = JsonAdapter::LoadJson(directory_ + filename);

	for (const auto& item : root["points"]) {

		CurvePoint point = {
			.input  = item["input"],
			.output = item["output"]
		};

		points_.emplace_back(point);
	}
}

float Curve::CatmullRomInterpolation(float p0, float p1, float p2, float p3, float t) {
	float t2 = t * t;
	float t3 = t2 * t;

	float e3 = -p0 + (p1 - p2) * 3.0f + p3; // t^3
	float e2 = p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3; // t^2
	float e1 = -p0 + p2; // t
	float e0 = p1 * 2.0f;

	return (e3 * t3 + e2 * t2 + e1 * t + e0) * 0.5f;
}