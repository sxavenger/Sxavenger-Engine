#include "DebugPrimitive.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/SxavengerSystem.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BaseDebugPrimitive base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BaseDebugPrimitive::Term() {
	input_.reset();
}

void BaseDebugPrimitive::Draw(const DirectXThreadContext* context, const Camera3d* camera) {

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
	inputCount_ = 0;
}

void BaseDebugPrimitive::CreateInputBuffer(uint32_t size) {
	input_ = std::make_unique<VertexDimensionBuffer<PrimitiveInput>>();
	input_->Create(SxavengerSystem::GetDxDevice(), size);
}

void BaseDebugPrimitive::SetVertexBuffer(const Vector3f& position, const Color4f& color) {
	Assert(inputOffset_ + inputCount_ < input_->GetSize()); //!< indexの超過

	(*input_)[inputOffset_ + inputCount_].position = { position.x, position.y, position.z };
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

void DebugPrimitiveLine::Term() {
}

void DebugPrimitiveLine::DrawLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color) {
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

void DebugPrimitive::DrawToScene(const DirectXThreadContext* context, const Camera3d* camera) {

	auto commandList = context->GetCommandList();

	{
		pipeline_->SetPipeline(commandList);
		line_->Draw(context, camera);
	}
	
}

void DebugPrimitive::ResetPrimitive() {
	line_->Reset();
}

void DebugPrimitive::DrawLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color) {
	line_->DrawLine(v1, v2, color);
}

void DebugPrimitive::DrawGrid(const Vector3f& center, float size) {

	static const float cell    = 1.0f;
	const Color4f defaultColor = ToColor4f(0x909090FF);
	const Color4f centerColor  = ToColor4f(0xF2630DFF);

	float half = size * 0.5f;


	for (float w = -half; w <= half; w += cell) {

		Color4f color = defaultColor;

		if (w == 0.0f) {
			color = centerColor;
		}

		DrawLine(
			{ center.x + w, center.y, center.z - half },
			{ center.x + w, center.y, center.z + half },
			color
		);

		DrawLine(
			{ center.x - half, center.y, center.z + w },
			{ center.x + half, center.y, center.z + w },
			color
		);
	}
}

void DebugPrimitive::DrawAxis(const Vector3f& center, float length) {
	DrawLine(center, center + Vector3f(length, 0.0f, 0.0f), ToColor4f(0xFA0000FF)); //!< x軸
	DrawLine(center, center + Vector3f(0.0f, length, 0.0f), ToColor4f(0x00FA00FF)); //!< y軸
	DrawLine(center, center + Vector3f(0.0f, 0.0f, length), ToColor4f(0x0000FAFF)); //!< z軸
}

void DebugPrimitive::DrawBox(const Vector3f& min, const Vector3f& max, const Color4f& color) {
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
	DrawLine(v0, v1, color);
	DrawLine(v1, v2, color);
	DrawLine(v2, v3, color);
	DrawLine(v3, v0, color);

	// 上部の四角形
	DrawLine(v4, v5, color);
	DrawLine(v5, v6, color);
	DrawLine(v6, v7, color);
	DrawLine(v7, v4, color);

	// 側面の線
	DrawLine(v0, v4, color);
	DrawLine(v1, v5, color);
	DrawLine(v2, v6, color);
	DrawLine(v3, v7, color);
}

void DebugPrimitive::CreatePrimitive() {
	line_ = std::make_unique<DebugPrimitiveLine>();
}

void DebugPrimitive::CreatePipeline() {

	pipeline_ = std::make_unique<GraphicsPipelineState>();
	pipeline_->CreateBlob("primitive/debugPrimitive.vs.hlsl", GraphicsShaderType::vs);
	pipeline_->CreateBlob("primitive/debugPrimitive.ps.hlsl", GraphicsShaderType::ps);

	GraphicsRootSignatureDesc rootDesc;
	rootDesc.SetVirtualCBV(0, ShaderVisibility::VISIBILITY_ALL, 0); //!< camera

	pipeline_->CreateRootSignature(SxavengerSystem::GetDxDevice(), rootDesc);

	GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();

	desc.elements.clear();
	desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	desc.SetElement("COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);
	desc.SetPrimitive(PrimitiveType::kLine);

	pipeline_->CreatePipeline(SxavengerSystem::GetDxDevice(), desc);

}
