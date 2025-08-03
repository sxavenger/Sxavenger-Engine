#include "DebugPrimitive.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Config/SxavengerConfig.h>
#include <Engine/System/SxavengerSystem.h>
#include <Engine/Component/Components/Camera/CameraComponent.h>

//* lib
#include <Lib/Geometry/GeometryMath.h>
#include <Lib/Geometry/VectorComparision.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseDebugPrimitive class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseDebugPrimitive::Draw(const DirectXQueueContext* context) {
	// commandListの取得
	auto commandList = context->GetCommandList();

	// 登録されたinputの描画
	const D3D12_VERTEX_BUFFER_VIEW vbv = input_->GetVertexBufferView();
	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->DrawInstanced(inputCount_, 1, inputOffset_, 0);

	CountBufferOffset();
}

void BaseDebugPrimitive::Reset() {
	inputOffset_ = 0;
	inputCount_  = 0;
}

void BaseDebugPrimitive::CountBufferOffset() {
	inputOffset_ += inputCount_;
	inputCount_   = 0;
}

void BaseDebugPrimitive::CreateInputBuffer(uint32_t size) {
	input_ = std::make_unique<VertexDimensionBuffer<PrimitiveInput>>();
	input_->Create(SxavengerSystem::GetDxDevice(), size);
}

void BaseDebugPrimitive::SetVertexBuffer(const Vector3f& position, const Color4f& color, float thickness) {
	Exception::Assert(inputOffset_ + inputCount_ < input_->GetSize()); //!< indexの超過

	(*input_)[inputOffset_ + inputCount_].position  = { position.x, position.y, position.z, 1.0f };
	(*input_)[inputOffset_ + inputCount_].color     = color;
	(*input_)[inputOffset_ + inputCount_].thickness = thickness;

	// 登録したので
	inputCount_++;
}

////////////////////////////////////////////////////////////////////////////////////////////
// DebugPrimitivePoint class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DebugPrimitivePoint::Init() {
	BaseDebugPrimitive::CreateInputBuffer(kMaxPointNum_);
}

void DebugPrimitivePoint::PushPoint(const Vector3f& v, const Color4f& color, float thickness) {
	BaseDebugPrimitive::SetVertexBuffer(v, color, thickness);
}

////////////////////////////////////////////////////////////////////////////////////////////
// DebugPrimitiveLine class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DebugPrimitiveLine::Init() {
	BaseDebugPrimitive::CreateInputBuffer(kMaxLineNum_ * kVertexNum_);
}

void DebugPrimitiveLine::PushLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color, float thickness) {
	BaseDebugPrimitive::SetVertexBuffer(v1, color, thickness);
	BaseDebugPrimitive::SetVertexBuffer(v2, color, thickness);
}

////////////////////////////////////////////////////////////////////////////////////////////
// DebugPrimitive class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DebugPrimitive::Init() {
	CreatePipeline();
	CreatePrimitive();
}

void DebugPrimitive::Term() {
}

void DebugPrimitive::DrawToScene(const DirectXQueueContext* context, const CameraComponent* camera) {

	{ //!< Line
		BindBufferDesc desc = {};
		desc.SetAddress("gCamera", camera->GetGPUVirtualAddress());
		desc.Set32bitConstants("Infomation", 2, &kMainWindowSize);

		pipelines_[static_cast<uint32_t>(PipelineType::Line)]->SetPipeline(context->GetDxCommand());
		pipelines_[static_cast<uint32_t>(PipelineType::Line)]->BindGraphicsBuffer(context->GetDxCommand(), desc);

		line_->Draw(context);
	}

	{ //!< Line Overlay
		BindBufferDesc desc = {};
		desc.SetAddress("gCamera", camera->GetGPUVirtualAddress());
		desc.Set32bitConstants("Infomation", 2, &kMainWindowSize);

		pipelines_[static_cast<uint32_t>(PipelineType::LineOverlay)]->SetPipeline(context->GetDxCommand());
		pipelines_[static_cast<uint32_t>(PipelineType::LineOverlay)]->BindGraphicsBuffer(context->GetDxCommand(), desc);

		lineOverlay_->Draw(context);
	}

	{ //!< Point
		BindBufferDesc desc = {};
		desc.SetAddress("gCamera", camera->GetGPUVirtualAddress());
		desc.Set32bitConstants("Infomation", 2, &kMainWindowSize);

		pipelines_[static_cast<uint32_t>(PipelineType::Point)]->SetPipeline(context->GetDxCommand());
		pipelines_[static_cast<uint32_t>(PipelineType::Point)]->BindGraphicsBuffer(context->GetDxCommand(), desc);

		point_->Draw(context);
	}

	{ //!< Point Overlay
		BindBufferDesc desc = {};
		desc.SetAddress("gCamera", camera->GetGPUVirtualAddress());
		desc.Set32bitConstants("Infomation", 2, &kMainWindowSize);

		pipelines_[static_cast<uint32_t>(PipelineType::PointOverlay)]->SetPipeline(context->GetDxCommand());
		pipelines_[static_cast<uint32_t>(PipelineType::PointOverlay)]->BindGraphicsBuffer(context->GetDxCommand(), desc);

		pointOverlay_->Draw(context);
	}
}

void DebugPrimitive::ResetPrimitive() {
	line_->Reset();
	lineOverlay_->Reset();
	point_->Reset();
	pointOverlay_->Reset();
}

void DebugPrimitive::PushLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color, float thickness) {
	line_->PushLine(v1, v2, color, thickness);
}

void DebugPrimitive::PushLineOverlay(const Vector3f& v1, const Vector3f& v2, const Color4f& color, float thickness) {
	lineOverlay_->PushLine(v1, v2, color, thickness);
}

void DebugPrimitive::PushPoint(const Vector3f& v, const Color4f& color, float thickness) {
	point_->PushPoint(v, color, thickness);
}

void DebugPrimitive::PushPointOverlay(const Vector3f& v, const Color4f& color, float thickness) {
	pointOverlay_->PushPoint(v, color, thickness);
}

void DebugPrimitive::PushGrid(const Vector3f& center, float size) {
	static const float cell    = 1.0f;
	const Color4f defaultColor = Color4f::Convert(0x909090FF);
	const Color4f centerColor  = Color4f::Convert(0xF2630DFF);

	float half = size * 0.5f;


	for (float w = -half; w <= half; w += cell) {

		Color4f color = defaultColor;

		if (w == 0.0f) {
			color = centerColor;
		}

		PushLine(
			{ center.x + w, center.y, center.z - half },
			{ center.x + w, center.y, center.z + half },
			color
		);

		PushLine(
			{ center.x - half, center.y, center.z + w },
			{ center.x + half, center.y, center.z + w },
			color
		);
	}
}

void DebugPrimitive::PushGrid(const CameraComponent* camera, const Vector2f& size, float radius) {

	Vector3f positions[2] = {};

	for (size_t i = 0; i < 2; ++i) {
		Vector3f target = Matrix4x4::Transform({ 0.0f, 0.0f, static_cast<float>(i) }, camera->GetCamera().projInv);
		positions[i]    = Matrix4x4::Transform(target, camera->GetCamera().world);;
	}

	// 画面中心の地面の位置を計算
	Vector3f direction = positions[1] - positions[0];

	if (direction.y == 0.0f) {
		return;
	}

	float t = -positions[0].y / direction.y;

	if (t < 0.0f || t > 1.0f) {
		return;
	}

	Vector3f point = positions[0] + direction * t;
	const Vector3f& position = camera->GetPosition();

	Vector3f min = point - Vector3f({ size.x * 0.5f, 0.0f, size.y * 0.5f });
	Vector3f max = point + Vector3f({ size.x * 0.5f, 0.0f, size.y * 0.5f });

	float alpha = 1.0f;

	float distance = (point - position).Length();

	if (distance > radius) {
		alpha = 1.0f - std::clamp(distance - radius / 12.0f, 0.0f, 1.0f) ;
	}


	// x軸の線
	for (int32_t i = static_cast<int32_t>(min.z); i <= static_cast<int32_t>(max.z); ++i) {

		float thickness = (i == 0) ? 1.0f : 0.1f;
		Color4f color   = (i % 5 == 0) ? Color4f::Convert(0x202020FF) : Color4f::Convert(0x909090FF);
		color.a         = alpha;

		PushLine({ min.x, 0.0f, static_cast<float>(i) }, { max.x, 0.0f, static_cast<float>(i) }, color, thickness);
	}

	// z軸の線
	for (int32_t i = static_cast<int32_t>(min.x); i <= static_cast<int32_t>(max.x); ++i) {

		float thickness = (i == 0) ? 1.0f : 0.1f;
		Color4f color   = (i % 5 == 0) ? Color4f::Convert(0x202020FF) : Color4f::Convert(0x909090FF);
		color.a         = alpha;

		PushLine({ static_cast<float>(i), 0.0f, min.z }, { static_cast<float>(i), 0.0f, max.z }, color, thickness);
	}

}
	

void DebugPrimitive::PushAxis(const Vector3f& center, float length) {
	PushLine(center, center + Vector3f(length, 0.0f, 0.0f), Color4f::Convert(0xFA0000FF), 0.6f); //!< x軸
	PushLine(center, center + Vector3f(0.0f, length, 0.0f), Color4f::Convert(0x00FA00FF), 0.6f); //!< y軸
	PushLine(center, center + Vector3f(0.0f, 0.0f, length), Color4f::Convert(0x0000FAFF), 0.6f); //!< z軸
}

void DebugPrimitive::PushBox(const Vector3f& min, const Vector3f& max, const Color4f& color) {
	// ボックスの頂点を計算
	Vector3f v0 = { min.x, min.y, min.z };
	Vector3f v1 = { max.x, min.y, min.z };
	Vector3f v2 = { max.x, max.y, min.z };
	Vector3f v3 = { min.x, max.y, min.z };
	Vector3f v4 = { min.x, min.y, max.z };
	Vector3f v5 = { max.x, min.y, max.z };
	Vector3f v6 = { max.x, max.y, max.z };
	Vector3f v7 = { min.x, max.y, max.z };

	// 下部の四角形
	PushLine(v0, v1, color, 0.6f);
	PushLine(v1, v2, color, 0.6f);
	PushLine(v2, v3, color, 0.6f);
	PushLine(v3, v0, color, 0.6f);

	// 上部の四角形
	PushLine(v4, v5, color, 0.6f);
	PushLine(v5, v6, color, 0.6f);
	PushLine(v6, v7, color, 0.6f);
	PushLine(v7, v4, color, 0.6f);

	// 側面の線
	PushLine(v0, v4, color, 0.6f);
	PushLine(v1, v5, color, 0.6f);
	PushLine(v2, v6, color, 0.6f);
	PushLine(v3, v7, color, 0.6f);
}

void DebugPrimitive::PushCube(const Vector3f& center, const Vector3f& size, const Color4f& color) {
	Vector3f min = center - size * 0.5f;
	Vector3f max = center + size * 0.5f;
	PushBox(min, max, color);
}

void DebugPrimitive::PushSphere(const Vector3f& center, float radius, const Color4f& color) {
	static const uint32_t kSubdivision = 1 << 5;          //!< parameter
	static const float kRoundEvery = kTau / kSubdivision; //!< 1周 / 分割数

	// xz軸の円
	for (uint32_t lon = 0; lon < kSubdivision; ++lon) {
		float theta = kRoundEvery * lon;

		// 単位円の生成
		Vector3f start = { //!< 始点
			std::sin(theta),
			0.0f,
			std::cos(theta),
		};

		Vector3f end = { //!< 終点
			std::sin(theta + kRoundEvery),
			0.0f,
			std::cos(theta + kRoundEvery),
		};

		// 半径を使ってローカル座標での円座標にする
		start *= radius;
		end   *= radius;

		PushLine(start + center, end + center, color);
	}

	// xy軸の円
	for (uint32_t lat = 0; lat < kSubdivision; ++lat) {
		float phi = kRoundEvery * lat;

		// 単位円の生成
		Vector3f start = { //!< 始点
			std::cos(phi),
			std::sin(phi),
			0.0f
		};

		Vector3f end = { //!< 終点
			std::cos(phi + kRoundEvery),
			std::sin(phi + kRoundEvery),
			0.0f
		};

		// 半径を使ってローカル座標での円座標にする
		start *= radius;
		end   *= radius;

		PushLine(start + center, end + center, color);
	}

	// yz軸の円
	for (uint32_t lat = 0; lat < kSubdivision; ++lat) {
		float phi = kRoundEvery * lat;

		// 単位円の生成
		Vector3f start = { //!< 始点
			0.0f,
			std::sin(phi),
			std::cos(phi)
		};

		Vector3f end = { //!< 終点
			0.0f,
			std::sin(phi + kRoundEvery),
			std::cos(phi + kRoundEvery),
		};

		// 半径を使ってローカル座標での円座標にする
		start *= radius;
		end   *= radius;

		PushLine(start + center, end + center, color);
	}
}

void DebugPrimitive::CreatePrimitive() {
	line_        = std::make_unique<DebugPrimitiveLine>();
	lineOverlay_ = std::make_unique<DebugPrimitiveLine>();

	point_        = std::make_unique<DebugPrimitivePoint>();
	pointOverlay_ = std::make_unique<DebugPrimitivePoint>();
}

void DebugPrimitive::CreatePipeline() {

	{
		auto& pipeline = pipelines_[static_cast<uint32_t>(PipelineType::Line)];
		pipeline = std::make_unique<ReflectionGraphicsPipelineState>();
		pipeline->CreateBlob(kPackagesShaderDirectory / "render/debug/Line/PrimitiveLine.vs.hlsl", GraphicsShaderType::vs);
		pipeline->CreateBlob(kPackagesShaderDirectory / "render/debug/Line/PrimitiveLine.gs.hlsl", GraphicsShaderType::gs);
		pipeline->CreateBlob(kPackagesShaderDirectory / "render/debug/Line/PrimitiveLine.ps.hlsl", GraphicsShaderType::ps);

		pipeline->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

		GraphicsPipelineDesc desc = {};
		desc.CreateDefaultDesc();

		desc.elements.clear();
		desc.SetElement("POSITION", 0,  DXGI_FORMAT_R32G32B32A32_FLOAT);
		desc.SetElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		desc.SetElement("THICKNESS", 0, DXGI_FORMAT_R32_FLOAT);

		desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
		desc.SetPrimitive(PrimitiveType::LineList);

		desc.SetDepthStencil(true);

		desc.SetRTVFormat(0, DXGI_FORMAT_R16G16B16A16_FLOAT);
		desc.SetBlendMode(0, BlendMode::kBlendModeNormal_AlphaMax);

		pipeline->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);
	}

	{
		auto& pipeline = pipelines_[static_cast<uint32_t>(PipelineType::LineOverlay)];
		pipeline = std::make_unique<ReflectionGraphicsPipelineState>();
		pipeline->CreateBlob(kPackagesShaderDirectory / "render/debug/Line/PrimitiveLine.vs.hlsl", GraphicsShaderType::vs);
		pipeline->CreateBlob(kPackagesShaderDirectory / "render/debug/Line/PrimitiveLine.gs.hlsl", GraphicsShaderType::gs);
		pipeline->CreateBlob(kPackagesShaderDirectory / "render/debug/Line/PrimitiveLine.ps.hlsl", GraphicsShaderType::ps);

		pipeline->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

		GraphicsPipelineDesc desc = {};
		desc.CreateDefaultDesc();

		desc.elements.clear();
		desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		desc.SetElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		desc.SetElement("THICKNESS", 0, DXGI_FORMAT_R32_FLOAT);

		desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
		desc.SetPrimitive(PrimitiveType::LineList);

		desc.SetDepthStencil(false);

		desc.SetRTVFormat(0, DXGI_FORMAT_R16G16B16A16_FLOAT);
		desc.SetBlendMode(0, BlendMode::kBlendModeNormal_AlphaMax);

		pipeline->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);
	}

	{
		auto& pipeline = pipelines_[static_cast<uint32_t>(PipelineType::Point)];
		pipeline = std::make_unique<ReflectionGraphicsPipelineState>();
		pipeline->CreateBlob(kPackagesShaderDirectory / "render/debug/Point/PrimitivePoint.vs.hlsl", GraphicsShaderType::vs);
		pipeline->CreateBlob(kPackagesShaderDirectory / "render/debug/Point/PrimitivePoint.gs.hlsl", GraphicsShaderType::gs);
		pipeline->CreateBlob(kPackagesShaderDirectory / "render/debug/Point/PrimitivePoint.ps.hlsl", GraphicsShaderType::ps);

		pipeline->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

		GraphicsPipelineDesc desc = {};
		desc.CreateDefaultDesc();

		desc.elements.clear();
		desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		desc.SetElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		desc.SetElement("THICKNESS", 0, DXGI_FORMAT_R32_FLOAT);

		desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
		desc.SetPrimitive(PrimitiveType::PointList);

		desc.SetDepthStencil(true);

		desc.SetRTVFormat(0, DXGI_FORMAT_R16G16B16A16_FLOAT);
		desc.SetBlendMode(0, BlendMode::kBlendModeNormal_AlphaMax);

		pipeline->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);
	}

	{
		auto& pipeline = pipelines_[static_cast<uint32_t>(PipelineType::PointOverlay)];
		pipeline = std::make_unique<ReflectionGraphicsPipelineState>();
		pipeline->CreateBlob(kPackagesShaderDirectory / "render/debug/Point/PrimitivePoint.vs.hlsl", GraphicsShaderType::vs);
		pipeline->CreateBlob(kPackagesShaderDirectory / "render/debug/Point/PrimitivePoint.gs.hlsl", GraphicsShaderType::gs);
		pipeline->CreateBlob(kPackagesShaderDirectory / "render/debug/Point/PrimitivePoint.ps.hlsl", GraphicsShaderType::ps);

		pipeline->ReflectionRootSignature(SxavengerSystem::GetDxDevice());

		GraphicsPipelineDesc desc = {};
		desc.CreateDefaultDesc();

		desc.elements.clear();
		desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		desc.SetElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		desc.SetElement("THICKNESS", 0, DXGI_FORMAT_R32_FLOAT);

		desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
		desc.SetPrimitive(PrimitiveType::PointList);

		desc.SetDepthStencil(false);

		desc.SetRTVFormat(0, DXGI_FORMAT_R16G16B16A16_FLOAT);
		desc.SetBlendMode(0, BlendMode::kBlendModeNormal_AlphaMax);

		pipeline->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);

	}

}
