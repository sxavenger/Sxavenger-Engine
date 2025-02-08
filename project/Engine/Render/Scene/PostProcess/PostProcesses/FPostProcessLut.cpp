#include "FPostProcessLut.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include <Engine/Render/FRenderCore.h>

//* engine
#include <Engine/System/SxavengerSystem.h>

//* external
#include <imgui.h>

////////////////////////////////////////////////////////////////////////////////////////////
// config methods
////////////////////////////////////////////////////////////////////////////////////////////

float FPostProcessLut::CatmullRomInterpolation(float p0, float p1, float p2, float p3, float t) {
	float t2 = t * t;
	float t3 = t2 * t;

	float e3 = -p0 + (p1 - p2) * 3.0f + p3; // t^3
	float e2 = p0 * 2.0f - p1 * 5.0f + p2 * 4.0f - p3; // t^2
	float e1 = -p0 + p2; // t
	float e0 = p1 * 2.0f;

	return (e3 * t3 + e2 * t2 + e1 * t + e0) * 0.5f;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Table structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void FPostProcessLut::Table::Init() {
	for (uint32_t i = 0; i < kPointCount; ++i) {
		points[i] = { i / static_cast<float>(kPointCount - 1), i / static_cast<float>(kPointCount - 1) };
	}
	//!< 0 ~ 1 の線形初期化
}

void FPostProcessLut::Table::Sort() {
	std::sort(points.begin(), points.end(), [](const auto& a, const auto& b) {
		return a.input < b.input;
	});
}

void FPostProcessLut::Table::SetImGuiCommand() {

	std::vector<float> values(256);

	for (uint32_t i = 0; i < 256; ++i) {
		values[i] = GetOutput(i / 255.0f);
	}

	ImGui::PlotLines("##plot", values.data(), int32_t(values.size()), 0, nullptr, FLT_MIN, FLT_MAX, ImVec2(0, 80));

	for (uint32_t i = 0; i < kPointCount; ++i) {
		std::string label = "## point " + std::to_string(i);
		ImGui::DragFloat2(label.c_str(), &points[i].input, 0.01f, 0.0f, 1.0f);
	}

	if (ImGui::Button("Reset")) {
		Init();
	}
}

float FPostProcessLut::Table::GetOutput(float input) {

	if (input <= points[0].input) { //!< 始点より小さい場合
		return points[0].output;
	}
	
	
	if (input >= points[3].input) { //!< 終点より大きい場合
		return points[3].output;
	}
	
	uint32_t segmentIndex = 0;
	for (uint32_t i = 0; i < kPointCount - 1; ++i) {
		if (input >= points[i].input && input < points[i + 1].input) {
			segmentIndex = i;
			break;
		}
	}
	
	// indexの決定
	uint32_t index0 = segmentIndex - 1;
	uint32_t index1 = segmentIndex;
	uint32_t index2 = segmentIndex + 1;
	uint32_t index3 = segmentIndex + 2;

	// fix index
	if (index1 == 0) {
		index0 = 0;
	}

	if (index3 >= kPointCount) {
		index3 = index2;
	}
	
	// inputTをセグメント内の位置に正規化
	float t = (input - points[index1].input) / (points[index2].input - points[index1].input);

	return CatmullRomInterpolation(points[index0].output, points[index1].output, points[index2].output, points[index3].output, t);
}


////////////////////////////////////////////////////////////////////////////////////////////
// FPostProcessLut class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FPostProcessLut::Init() {
	parameter_ = std::make_unique<DimensionBuffer<Parameter>>();
	parameter_->Create(SxavengerSystem::GetDxDevice(), 1);

	parameter_->At(0).r.Init();
	parameter_->At(0).g.Init();
	parameter_->At(0).b.Init();
}

void FPostProcessLut::Process(const ProcessContext& context) {
	context.textures->NextIndex();

	parameter_->At(0).r.Sort();
	parameter_->At(0).g.Sort();
	parameter_->At(0).b.Sort();

	FRenderCore::GetInstance()->GetProcess()->SetPipeline(FRenderCoreProcess::ProcessType::LUT, context.context);

	DxObject::BindBufferDesc parameter = context.parameter;

	parameter.SetHandle("gInput",  context.textures->GetPrevTexture(1)->GetGPUHandleUAV());
	parameter.SetHandle("gOutput", context.textures->GetPrevTexture(0)->GetGPUHandleUAV());
	parameter.SetAddress("gParameter", parameter_->GetGPUVirtualAddress());

	FRenderCore::GetInstance()->GetProcess()->BindComputeBuffer(FRenderCoreProcess::ProcessType::LUT, context.context, parameter);
	FRenderCore::GetInstance()->GetProcess()->Dispatch(context.context, context.size);
}

void FPostProcessLut::SetImGuiCommand() {
	if (ImGui::TreeNode("red")) {
		parameter_->At(0).r.SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("green")) {
		parameter_->At(0).g.SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("blue")) {
		parameter_->At(0).b.SetImGuiCommand();
		ImGui::TreePop();
	}
}
