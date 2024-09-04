#include "PrimitiveDrawer.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Sxavenger.h>
#include <SxavengerGraphics.h>

#include <Environment.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BasePrimitive3D base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BasePrimitive::CreateVertexBuffer(uint32_t vertexCount) {
	vertex_ = std::make_unique<BufferResource<PrimitiveInput>>(Sxavenger::GetDevicesObj(), vertexCount);
}

void BasePrimitive::ResetObjectCount() {
	objectCount_ = 0;
}

Vector4f BasePrimitive::ToProjection(const Vector2f& v) {
	Vector2f projection = { v.x / kWindowWidth * 2.0f - 1.0f, v.y / kWindowHeight * 2.0f - 1.0f };
	return { projection.x, projection.y * -1.0f, 0.0f };
}

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveLine class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveLine::Init(PrimitiveDimensionType dimension) {

	// 引数の保存
	dimension_ = dimension;

	// vertexの生成
	CreateVertexBuffer(kMaxLineNum_ * kObjectVertexNum_);
}

void PrimitiveLine::Term() {
}

void PrimitiveLine::DrawLine(const Vector3f& v0, const Vector3f& v1, const Color4f& color) {
	Assert(objectCount_ < kMaxLineNum_, "object count is out of range.");

	uint32_t index = objectCount_ * kObjectVertexNum_;

	(*vertex_)[index + 0].position = { v0.x, v0.y, v0.z, };
	(*vertex_)[index + 0].color    = color;
	(*vertex_)[index + 1].position = { v1.x, v1.y, v1.z, };
	(*vertex_)[index + 1].color    = color;

	objectCount_++;
}

void PrimitiveLine::DrawLine(const Vector2f& v0, const Vector2f& v1, const Color4f& color) {
	Assert(objectCount_ < kMaxLineNum_, "object count is out of range.");

	uint32_t index = objectCount_ * kObjectVertexNum_;

	(*vertex_)[index + 0].position = ToProjection(v0);
	(*vertex_)[index + 0].color    = color;
	(*vertex_)[index + 1].position = ToProjection(v1);
	(*vertex_)[index + 1].color    = color;

	objectCount_++;
}

void PrimitiveLine::DrawAll(Texture* sampleTexture) {

	auto commandList = Sxavenger::GetCommandList();

	D3D12_VERTEX_BUFFER_VIEW vbv = vertex_->GetVertexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vbv);

	commandList->SetGraphicsRootDescriptorTable(0, sampleTexture->GetGPUHandleSRV());

	if (dimension_ == kPrimitive_3D) {
		commandList->SetGraphicsRootConstantBufferView(1, SxavengerGraphics::camera3D->GetGPUVirtualAddress());
	}

	commandList->DrawInstanced(objectCount_ * kObjectVertexNum_, 1, 0, 0);
	
	ResetObjectCount();
}

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveTriangle class methods
////////////////////////////////////////////////////////////////////////////////////////////

void PrimitiveTriangle::Init(PrimitiveDimensionType dimension) {

	// 引数の保存
	dimension_ = dimension;

	// vertexの生成
	CreateVertexBuffer(kMaxLineNum_ * kObjectVertexNum_);

}

void PrimitiveTriangle::Term() {
}

void PrimitiveTriangle::DrawTriangle(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Color4f& color) {
	Assert(objectCount_ < kMaxLineNum_, "object count is out of range.");

	uint32_t index = objectCount_ * kObjectVertexNum_;

	(*vertex_)[index + 0].position = { v0.x, v0.y, v0.z, };
	(*vertex_)[index + 0].color    = color;
	(*vertex_)[index + 1].position = { v1.x, v1.y, v1.z, };
	(*vertex_)[index + 1].color    = color;
	(*vertex_)[index + 2].position = { v2.x, v2.y, v2.z, };
	(*vertex_)[index + 2].color    = color;

	objectCount_++;
}

void PrimitiveTriangle::DrawTriangle(const Vector2f& v0, const Vector2f& v1, const Vector2f& v2, const Color4f& color) {
	Assert(objectCount_ < kMaxLineNum_, "object count is out of range.");

	uint32_t index = objectCount_ * kObjectVertexNum_;

	(*vertex_)[index + 0].position = ToProjection(v0);
	(*vertex_)[index + 0].color    = color;
	(*vertex_)[index + 1].position = ToProjection(v1);
	(*vertex_)[index + 1].color    = color;
	(*vertex_)[index + 2].position = ToProjection(v2);
	(*vertex_)[index + 2].color    = color;

	objectCount_++;
}

void PrimitiveTriangle::DrawAll(Texture* sampleTexture) {

	auto commandList = Sxavenger::GetCommandList();

	D3D12_VERTEX_BUFFER_VIEW vbv = vertex_->GetVertexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vbv);

	commandList->SetGraphicsRootDescriptorTable(0, sampleTexture->GetGPUHandleSRV());

	if (dimension_ == kPrimitive_3D) {
		commandList->SetGraphicsRootConstantBufferView(1, SxavengerGraphics::camera3D->GetGPUVirtualAddress());
	}

	commandList->DrawInstanced(objectCount_ * kObjectVertexNum_, 1, 0, 0);

	ResetObjectCount();
}

////////////////////////////////////////////////////////////////////////////////////////////
// PrimitiveDrawer class methods
////////////////////////////////////////////////////////////////////////////////////////////

const std::wstring PrimitiveDrawer::directory_ = L"Primitive/";

void PrimitiveDrawer::Init() {
	CreatePipeline();
	CreateSampleTexture();

	//* primitives *//

	for (int i = 0; i < kCountOfPrimitiveDimensionType; ++i) {
		lines_[i]     = std::make_unique<PrimitiveLine>(static_cast<PrimitiveDimensionType>(i));
		triangles_[i] = std::make_unique<PrimitiveTriangle>(static_cast<PrimitiveDimensionType>(i));
	}
}

void PrimitiveDrawer::Term() {
}

void PrimitiveDrawer::DrawLine(const Vector3f& v0, const Vector3f& v1, const Color4f& color) {
	lines_[kPrimitive_3D]->DrawLine(v0, v1, color);
}

void PrimitiveDrawer::DrawLine(const Vector2f& v0, const Vector2f& v1, const Color4f& color) {
	lines_[kPrimitive_2D]->DrawLine(v0, v1, color);
}

void PrimitiveDrawer::DrawTriangle(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2, const Color4f& color) {
	triangles_[kPrimitive_3D]->DrawTriangle(v0, v1, v2, color);
}

void PrimitiveDrawer::DrawTriangle(const Vector2f& v0, const Vector2f& v1, const Vector2f& v2, const Color4f& color) {
	triangles_[kPrimitive_2D]->DrawTriangle(v0, v1, v2, color);
}

void PrimitiveDrawer::DrawAll3D() {

	auto commandList = Sxavenger::GetCommandList();

	pipelines_[kPrimitive_3D][kPrimitive_Line]->SetPipeline(commandList);
	lines_[kPrimitive_3D]->DrawAll(texture_.get());

	pipelines_[kPrimitive_3D][kPrimitive_Triangle]->SetPipeline(commandList);
	triangles_[kPrimitive_3D]->DrawAll(texture_.get());
}

void PrimitiveDrawer::DrawAll2D() {

	auto commandList = Sxavenger::GetCommandList();

	pipelines_[kPrimitive_2D][kPrimitive_Line]->SetPipeline(commandList);
	lines_[kPrimitive_2D]->DrawAll(texture_.get());

	pipelines_[kPrimitive_2D][kPrimitive_Triangle]->SetPipeline(commandList);
	triangles_[kPrimitive_2D]->DrawAll(texture_.get());

}

void PrimitiveDrawer::Reset() {
	for (int di = 0; di < kCountOfPrimitiveDimensionType; ++di) {
		lines_[di]->ResetObjectCount();
		triangles_[di]->ResetObjectCount();
	}
}

PrimitiveDrawer* PrimitiveDrawer::GetInstance() {
	static PrimitiveDrawer instance;
	return &instance;
}

void PrimitiveDrawer::CreatePipeline() {

	// blobの生成
	{
		// 3D
		blobs_[kPrimitive_3D] = std::make_unique<GraphicsBlob>();
		blobs_[kPrimitive_3D]->Create(directory_ + L"Primitive3D.VS.hlsl", GRAPHICS_VERTEX);
		blobs_[kPrimitive_3D]->Create(directory_ + L"Primitive.PS.hlsl",   GRAPHICS_PIXEL);

		// 2D
		blobs_[kPrimitive_2D] = std::make_unique<GraphicsBlob>();
		blobs_[kPrimitive_2D]->Create(directory_ + L"Primitive2D.VS.hlsl", GRAPHICS_VERTEX);
		blobs_[kPrimitive_2D]->Create(directory_ + L"Primitive.PS.hlsl",   GRAPHICS_PIXEL);
	}

	// pipelineの生成
	for (int di = 0; di < kCountOfPrimitiveDimensionType; ++di) { //!< dimensionが同じ場合は同じblobを使う

		// RootSignatureの生成
		GraphicsRootSignatureDesc rootDesc;
		rootDesc.SetSRV(0, VISIBILITY_PIXEL, 0);                //!< Texture2D
		rootDesc.SetSampler(0, MODE_WRAP, VISIBILITY_PIXEL, 0); //!< SamplerState
		
		if (di == kPrimitive_3D) {
			rootDesc.SetCBV(1, VISIBILITY_VERTEX, 0); //!< Camera3D
		}

		// Triangle
		pipelines_[di][kPrimitive_Triangle] = std::make_unique<GraphicsPipeline>();
		pipelines_[di][kPrimitive_Triangle]->CreateRootSignature(Sxavenger::GetDevicesObj(), rootDesc);

		GraphicsPipelineDesc desc = {};
		desc.CreateDefaultDesc();

		desc.elements.clear(); //!< elementの代入し直し
		desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
		desc.SetElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
		desc.SetElement("COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT);

		pipelines_[di][kPrimitive_Triangle]->CreatePipeline(Sxavenger::GetDevicesObj(), blobs_[di].get(), desc);

		// Line
		pipelines_[di][kPrimitive_Line] = std::make_unique<GraphicsPipeline>();
		pipelines_[di][kPrimitive_Line]->CreateRootSignature(Sxavenger::GetDevicesObj(), rootDesc);

		desc.SetPrimitive(PRIMITIVE_LINE);

		pipelines_[di][kPrimitive_Line]->CreatePipeline(Sxavenger::GetDevicesObj(), blobs_[di].get(), desc);

	}
}

void PrimitiveDrawer::CreateSampleTexture() {
	texture_ = std::make_unique<Texture>();
	texture_->Load(Sxavenger::GetDxCommon(), "resources/white1x1.png");
}
