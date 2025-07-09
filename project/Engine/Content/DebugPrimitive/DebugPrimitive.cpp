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

////////////////////////////////////////////////////////////////////////////////////////////
// BaseDebugPrimitive class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseDebugPrimitive::Draw(const DirectXThreadContext* context, const CameraComponent* camera) {
	// commandListの取得
	auto commandList = context->GetCommandList();

	// 登録されたinputの描画
	const D3D12_VERTEX_BUFFER_VIEW vbv = input_->GetVertexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->SetGraphicsRootConstantBufferView(0, camera->GetGPUVirtualAddress());

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

void BaseDebugPrimitive::SetVertexBuffer(const Vector3f& position, const Color4f& color) {
	Exception::Assert(inputOffset_ + inputCount_ < input_->GetSize()); //!< indexの超過

	(*input_)[inputOffset_ + inputCount_].position = { position.x, position.y, position.z, 1.0f };
	(*input_)[inputOffset_ + inputCount_].color    = color;

	// 登録したので
	inputCount_++;
}

////////////////////////////////////////////////////////////////////////////////////////////
// DebugPrimitiveLine class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DebugPrimitiveLine::Init() {
	BaseDebugPrimitive::CreateInputBuffer(kMaxLineNum_ * kVertexNum_);
}

void DebugPrimitiveLine::PushLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color) {
	BaseDebugPrimitive::SetVertexBuffer(v1, color);
	BaseDebugPrimitive::SetVertexBuffer(v2, color);
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

void DebugPrimitive::DrawToScene(const DirectXThreadContext* context, const CameraComponent* camera) {

	{
		pipelines_[PipelineType::kLine]->SetPipeline(context->GetDxCommand());
		line_->Draw(context, camera);
	}

	{
		pipelines_[PipelineType::kLineOverlay]->SetPipeline(context->GetDxCommand());
		lineOverlay_->Draw(context, camera);
	}
}

void DebugPrimitive::ResetPrimitive() {
	line_->Reset();
	lineOverlay_->Reset();
}

void DebugPrimitive::PushLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color) {
	line_->PushLine(v1, v2, color);
}

void DebugPrimitive::PushLineOverlay(const Vector3f& v1, const Vector3f& v2, const Color4f& color) {
	lineOverlay_->PushLine(v1, v2, color);
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

void DebugPrimitive::PushAxis(const Vector3f& center, float length) {
	PushLine(center, center + Vector3f(length, 0.0f, 0.0f), Color4f::Convert(0xFA0000FF)); //!< x軸
	PushLine(center, center + Vector3f(0.0f, length, 0.0f), Color4f::Convert(0x00FA00FF)); //!< y軸
	PushLine(center, center + Vector3f(0.0f, 0.0f, length), Color4f::Convert(0x0000FAFF)); //!< z軸
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
	PushLine(v0, v1, color);
	PushLine(v1, v2, color);
	PushLine(v2, v3, color);
	PushLine(v3, v0, color);

	// 上部の四角形
	PushLine(v4, v5, color);
	PushLine(v5, v6, color);
	PushLine(v6, v7, color);
	PushLine(v7, v4, color);

	// 側面の線
	PushLine(v0, v4, color);
	PushLine(v1, v5, color);
	PushLine(v2, v6, color);
	PushLine(v3, v7, color);
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
}

void DebugPrimitive::CreatePipeline() {

	{
		auto& pipeline = pipelines_[PipelineType::kLine];
		pipeline = std::make_unique<GraphicsPipelineState>();
		pipeline->CreateBlob(kPackagesShaderDirectory / "render/debug/debugPrimitive.vs.hlsl", GraphicsShaderType::vs);
		pipeline->CreateBlob(kPackagesShaderDirectory / "render/debug/debugPrimitive.ps.hlsl", GraphicsShaderType::ps);

		GraphicsRootSignatureDesc rootDesc;
		rootDesc.SetVirtualCBV(0, ShaderVisibility::VISIBILITY_ALL, 0); //!< camera

		pipeline->CreateRootSignature(SxavengerSystem::GetDxDevice(), std::move(rootDesc));

		GraphicsPipelineDesc desc = {};
		desc.CreateDefaultDesc();

		desc.elements.clear();
		desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		desc.SetElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

		desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
		desc.SetPrimitive(PrimitiveType::LineList);

		desc.SetDepthStencil(true);

		desc.SetRTVFormat(0, DXGI_FORMAT_R32G32B32A32_FLOAT);

		pipeline->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);
	}

	{
		auto& pipeline = pipelines_[PipelineType::kLineOverlay];
		pipeline = std::make_unique<GraphicsPipelineState>();
		pipeline->CreateBlob(kPackagesShaderDirectory / "render/debug/debugPrimitive.vs.hlsl", GraphicsShaderType::vs);
		pipeline->CreateBlob(kPackagesShaderDirectory / "render/debug/debugPrimitive.ps.hlsl", GraphicsShaderType::ps);

		GraphicsRootSignatureDesc rootDesc;
		rootDesc.SetVirtualCBV(0, ShaderVisibility::VISIBILITY_ALL, 0); //!< camera

		pipeline->CreateRootSignature(SxavengerSystem::GetDxDevice(), std::move(rootDesc));

		GraphicsPipelineDesc desc = {};
		desc.CreateDefaultDesc();

		desc.elements.clear();
		desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		desc.SetElement("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);

		desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
		desc.SetPrimitive(PrimitiveType::LineList);

		desc.SetDepthStencil(false);

		desc.SetRTVFormat(0, DXGI_FORMAT_R32G32B32A32_FLOAT);

		pipeline->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);
	}


}
