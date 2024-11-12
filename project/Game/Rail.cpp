#include "Rail.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>

//* lib
#include <Lib/MyMath.h>
#include <Lib/Adapter/Json.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Rail class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Rail::Init() {

	//points_ = { //!< 山and回転
	//	kOrigin3,
	//	{ 0.0f, 0.0f, 1.0f },
	//	{ 0.0f, 2.0f, 1.0f },
	//	{ 0.0f, 2.0f, 2.0f },
	//	{ 0.0f, 0.0f, 6.0f },
	//	{ 0.0f, 1.0f, 7.0f },
	//	{ 0.0f, 2.0f, 6.0f },
	//	{ 0.0f, 1.0f, 5.0f },
	//	{ 0.0f, 0.0f, 6.0f },
	//	{ 0.0f, 0.0f, 8.0f },
	//	{ 2.0f, 0.0f, 4.0f },
	//};

	points_ = { //!< 山and回転
		kOrigin3,
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 2.0f, 1.0f },
	};

	LoadJson();

	SetName("rail");

	BaseBehavior::renderingFlag_ = kBehaviorRender_Adaptive;

	CreatePipelineAndIA();

	Sxavenger::LoadTexture("resources/textures/rail.png");
}

void Rail::Term() {
}

void Rail::Update() {
	inputOffset_ = 0;
	inputCount_  = 0;
	ia_.GetVertexBuffer()->ClearData();
}

void Rail::DrawAdaptive(_MAYBE_UNUSED const Camera3D* camera) {
	DrawCatmullrom(256);
	DrawToScene(camera);
}

void Rail::SetAttributeImGui() {

	if (ImGui::Button("Add")) {
		points_.emplace_back(points_.back());
	}

	if (ImGui::Button("Delete")) {
		points_.erase(points_.end() - 1);
	}

	if (ImGui::Button("Output")) {
		OutputJson();
	}

	for (uint32_t i = 0; i < points_.size(); ++i) {
		std::string label = "position ##" + std::format("{:p}", reinterpret_cast<void*>(&points_[i]));
		ImGui::DragFloat3(label.c_str(), &points_[i].x, 0.01f);
	}
}

Vector3f Rail::LoopCatmullRomPosition(float t) {
	if (points_.size() < 3) {
		return {};
	}

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

//RailElement Rail::LoopCatmullRomRailElement(float t) {
//	Assert(railElements_.size() >= 3, "rail points must over 3.");
//
//	// 区間数
//	size_t division = railElements_.size();
//
//	// 1区間の長さ
//	float areaWidth = 1.0f / division;
//
//	// 区間内の始点を0.0f, 終点を1.0fとしたときの現在の位置
//	float divisionT = std::fmod(t, areaWidth) * division;
//	divisionT = std::clamp(divisionT, 0.0f, 1.0f);
//
//	// 区間番号
//	size_t index = static_cast<size_t>(t / areaWidth);
//	index = std::clamp<size_t>(index, 0, railElements_.size() - 1);
//
//	size_t index0 = index - 1;
//	size_t index1 = index;
//	size_t index2 = index + 1;
//	size_t index3 = index + 2;
//
//	// 区間が始点の場合
//	if (index == 0) {
//		index0 = railElements_.size() - 1; //!< ループさせるので
//	}
//
//	index2 %= railElements_.size();
//	index3 %= railElements_.size();
//
//	RailElement result = {};
//
//	const Vector3f& p0 = railElements_[index0].position;
//	const Vector3f& p1 = railElements_[index1].position;
//	const Vector3f& p2 = railElements_[index2].position;
//	const Vector3f& p3 = railElements_[index3].position;
//
//	result.position = CatmullRomInterpolation(p0, p1, p2, p3, divisionT);
//
//	const Vector3f& u0 = railElements_[index0].upVector;
//	const Vector3f& u1 = railElements_[index1].upVector;
//	const Vector3f& u2 = railElements_[index2].upVector;
//	const Vector3f& u3 = railElements_[index3].upVector;
//
//	result.upVector = CatmullRomInterpolation(u0, u1, u2, u3, divisionT);
//
//	return result;
//}

void Rail::DrawCatmullrom(uint32_t kSubdivision) {

	const float kWidth = 0.1f;

	// 始点を先に設定.
	Vector3f beginPotisionLeft  = {};
	Vector3f beginPotisionRight = {};

	{
		float t     = 0.0f;
		float nextT = 1.0f / kSubdivision;

		Vector3f position = LoopCatmullRomPosition(t);
		Vector3f nextPosition = LoopCatmullRomPosition(nextT);

		Vector3f zAxis = Normalize(nextPosition - position);
		Vector3f yAxis = { 0.0f, 1.0f, 0.0f };
		Vector3f xAxis = Normalize(Cross(yAxis, zAxis));

		beginPotisionLeft  = position - xAxis * kWidth;
		beginPotisionRight = position + xAxis * kWidth;
	}

	BindIA(beginPotisionLeft, beginPotisionRight, ToColor4f(0xFAFAFAFF));

	for (uint32_t i = 1; i < kSubdivision; ++i) {

		float t = static_cast<float>(i) / kSubdivision;
		float nextT = static_cast<float>(i + 1) / kSubdivision;

		Vector3f position     = LoopCatmullRomPosition(t);
		Vector3f nextPosition = LoopCatmullRomPosition(nextT);

		Vector3f zAxis = Normalize(nextPosition - position);
		Vector3f yAxis = { 0.0f, 1.0f, 0.0f };
		Vector3f xAxis = Normalize(Cross(yAxis, zAxis));

		BindIA(position - xAxis * kWidth, position + xAxis * kWidth, ToColor4f(0xFAFAFAFF));
	}

	BindIA(beginPotisionLeft, beginPotisionRight, ToColor4f(0xFAFAFAFF));
}

//void Rail::DrawCatmullromRail(uint32_t kSubdivision) {
//	const float kWidth = 0.1f;
//
//	// 始点を先に設定.
//	Vector3f beginPotisionLeft  = {};
//	Vector3f beginPotisionRight = {};
//
//	{
//		float t     = 0.0f;
//		float nextT = 1.0f / kSubdivision;
//
//		RailElement element     = LoopCatmullRomRailElement(t);
//		RailElement nextElement = LoopCatmullRomRailElement(nextT);
//
//		Vector3f zAxis = Normalize(nextElement.position - element.position);
//		Vector3f yAxis = element.upVector;
//		Vector3f xAxis = Normalize(Cross(yAxis, zAxis));
//
//		beginPotisionLeft  = element.position - xAxis * kWidth;
//		beginPotisionRight = element.position + xAxis * kWidth;
//	}
//
//	BindIA(beginPotisionLeft, beginPotisionRight, ToColor4f(0xFAFAFAFF));
//
//	for (uint32_t i = 1; i < kSubdivision; ++i) {
//
//		float t = static_cast<float>(i) / kSubdivision;
//		float nextT = static_cast<float>(i + 1) / kSubdivision;
//
//		RailElement element     = LoopCatmullRomRailElement(t);
//		RailElement nextElement = LoopCatmullRomRailElement(nextT);
//
//		Vector3f zAxis = Normalize(nextElement.position - element.position);
//		Vector3f yAxis = element.upVector;
//		Vector3f xAxis = Normalize(Cross(yAxis, zAxis));
//
//		Vector3f left  = element.position - xAxis * kWidth;
//		Vector3f right = element.position + xAxis * kWidth;
//
//		BindIA(left, right, ToColor4f(0xFAFAFAFF));
//	}
//
//	BindIA(beginPotisionLeft, beginPotisionRight, ToColor4f(0xFAFAFAFF));
//}

void Rail::CreatePipelineAndIA() {

	//ia_ = std::make_unique<BufferResource<RailIA>>(Sxavenger::GetDevicesObj(), kMaxVertexCount_ * kPrimitiveCount_);

	ia_.Create(kMaxVertexCount_, kMaxVertexCount_ * kPrimitiveCount_);

	auto indexBuffer = ia_.GetIndexBuffer();

	uint32_t index = 0;

	for (uint32_t i = 0; i < kMaxVertexCount_; ++i) {
		(*indexBuffer)[index] = i;
		index++;

		(*indexBuffer)[index] = i + 1;
		index++;
	}

	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"rail/rail.VS.hlsl", GRAPHICS_VERTEX);
	blob_->Create(L"rail/rail.GS.hlsl", GRAPHICS_GEOMETRY);
	blob_->Create(L"rail/rail.PS.hlsl", GRAPHICS_PIXEL);

	pipeline_ = std::make_unique<GraphicsPipeline>();

	GraphicsRootSignatureDesc rootDesc;
	rootDesc.SetCBV(0, VISIBILITY_ALL, 10);
	rootDesc.SetSRV(1, VISIBILITY_PIXEL, 0);
	rootDesc.SetSamplerAnisotropic(MODE_WRAP, VISIBILITY_PIXEL, 0, 8);

	pipeline_->CreateRootSignature(Sxavenger::GetDevicesObj(), rootDesc);

	GraphicsPipelineDesc desc;
	desc.CreateDefaultDesc();

	desc.elements.clear();
	desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	desc.SetElement("POSITION", 1, DXGI_FORMAT_R32G32B32A32_FLOAT);
	desc.SetElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	desc.SetPrimitive(PRIMITIVE_LINE);

	desc.SetDSVFormat(DXGI_FORMAT_D32_FLOAT_S8X24_UINT);

	pipeline_->CreatePipeline(Sxavenger::GetDevicesObj(), blob_.get(), desc);
}

void Rail::BindIA(const Vector3f& positionLeft, const Vector3f& positionRight, const Color4f& color) {
	Assert(inputCount_ + inputOffset_ < ia_.GetVertexBuffer()->GetIndexSize(), "vertex count is over.");

	uint32_t index = inputOffset_ + inputCount_;

	(*ia_.GetVertexBuffer())[index].positionLeft  = { positionLeft.x, positionLeft.y, positionLeft.z };
	(*ia_.GetVertexBuffer())[index].positionRight = { positionRight.x, positionRight.y, positionRight.z };
	(*ia_.GetVertexBuffer())[index].color         = color;

	inputCount_++;
}

void Rail::DrawToScene(const Camera3D* camera) {

	auto commandList = Sxavenger::GetCommandList();

	pipeline_->SetPipeline(commandList);

	ia_.BindIABuffer();

	commandList->SetGraphicsRootConstantBufferView(0, camera->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(1, Sxavenger::GetTextureHandleGPU("resources/textures/rail.png"));
	commandList->DrawIndexedInstanced(inputCount_ * kPrimitiveCount_, 1, 0, inputOffset_, 0);

	inputOffset_ += inputCount_;
	inputCount_   = 0;
}

void Rail::LoadJson() {
	
	Json root;

	if (JsonAdapter::LoadJson(kFilename, root)) {
		points_.clear();

		for (size_t i = 0; i < root["points"].size(); ++i) {
			points_.emplace_back(JsonAdapter::ToVector3(root["points"][i]));
		}
	}
}

void Rail::OutputJson() {

	Json root = Json::object();

	auto& points = root["points"] = Json::array();

	for (size_t i = 0; i < points_.size(); ++i) {
		points.emplace_back(JsonAdapter::ToJson(points_[i]));
	}

	JsonAdapter::WriteJson(kFilename, root);
}

