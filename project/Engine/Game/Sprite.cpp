#include "Sprite.h"
_DXOBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>
#include <Engine/Game/Transform.h>

////////////////////////////////////////////////////////////////////////////////////////////
// SpriteManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SpriteCommon::Init() {
	CreatePipeline();
	CreateBuffer();
}

void SpriteCommon::Term() {
}

void SpriteCommon::Reset() {
	elementIndex_ = 0;
}

void SpriteCommon::DrawSprite(
	const Vector2f& pos, const Vector2f& size, const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Color4f& color) {

	SetVertex(elementIndex_);
	SetTexcoord(elementIndex_);
	SetColor(elementIndex_, color);
	SetTransform2d(elementIndex_, size, 0.0f, pos);
	SetHandle(elementIndex_, handle);

	DrawCall(elementIndex_);
	elementIndex_++;
}

void SpriteCommon::DrawSprite(
	const Vector2f& pos, const Vector2f& size, float rotate, const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Color4f& color) {

	SetVertex(elementIndex_);
	SetTexcoord(elementIndex_);
	SetColor(elementIndex_, color);
	SetTransform2d(elementIndex_, size, rotate, pos);
	SetHandle(elementIndex_, handle);

	DrawCall(elementIndex_);
	elementIndex_++;
}

void SpriteCommon::DrawSpriteClip(
	const Vector2f& pos, const Vector2f& size, const Vector2f& leftTopUV, const Vector2f& sizeUV, const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Color4f& color) {

	SetVertex(elementIndex_);
	SetTexcoordClip(elementIndex_, leftTopUV, sizeUV);
	SetColor(elementIndex_, color);
	SetTransform2d(elementIndex_, size, 0.0f, pos);
	SetHandle(elementIndex_, handle);

	DrawCall(elementIndex_);
	elementIndex_++;
}

void SpriteCommon::CreatePipeline() {

	std::unique_ptr<GraphicsBlob> blob = std::make_unique<GraphicsBlob>();
	blob->Create(L"sprite/sprite.vs.hlsl", GRAPHICS_VERTEX);
	blob->Create(L"sprite/sprite.ps.hlsl", GRAPHICS_PIXEL);

	pipeline_ = std::make_unique<GraphicsPipeline>();

	GraphicsRootSignatureDesc rootDesc = {};
	rootDesc.SetSRV(0, VISIBILITY_ALL, 10);   //!< Texture
	rootDesc.SetSampler(MODE_CLAMP, VISIBILITY_ALL, 10);
	rootDesc.SetCBV(1, VISIBILITY_ALL, 10);   //!< Info
	rootDesc.SetCBV(2, VISIBILITY_VERTEX, 0); //!< Projection

	pipeline_->CreateRootSignature(Sxavenger::GetDevicesObj(), rootDesc);

	GraphicsPipelineDesc desc;
	desc.CreateDefaultDesc();

	desc.elements.clear();
	desc.SetElement("POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT);
	desc.SetElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
	desc.SetElement("COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT);

	desc.SetRasterizer(D3D12_CULL_MODE_NONE, D3D12_FILL_MODE_SOLID);

	pipeline_->CreatePipeline(Sxavenger::GetDevicesObj(), blob.get(), desc);
}

void SpriteCommon::CreateBuffer() {

	for (auto& element : elements_) {
		element.vertex = std::make_unique<BufferResource<SpriteInputAssembler>>(Sxavenger::GetDevicesObj(), kCountOfSpriteIAType);
		element.info   = std::make_unique<BufferResource<SpriteInfo>>(Sxavenger::GetDevicesObj(), 1);
	}

	index_ = std::make_unique<DxObject::IndexBufferResource>(Sxavenger::GetDevicesObj(), 2 * 3);
	// indexは固定なので設定
	(*index_)[0] = LEFTTOP;
	(*index_)[2] = LEFTBOTTOM;
	(*index_)[1] = RIGHTTOP;

	(*index_)[3] = RIGHTTOP;
	(*index_)[4] = RIGHTBOTTOM;
	(*index_)[5] = LEFTBOTTOM;

	projection_ = std::make_unique<BufferResource<Matrix4x4>>(Sxavenger::GetDevicesObj(), 1);
	(*projection_)[0] = Matrix::MakeOrthographic(0.0f, 0.0f, static_cast<float>(kWindowSize.x), static_cast<float>(kWindowSize.y), 0.0f, 100.0f);

}

void SpriteCommon::SetVertex(uint32_t index, const Vector2f& anchor, const Vector2<bool>& isFlip) {

	float left   = 0.0f - anchor.x;
	float right  = 1.0f - anchor.x;
	float top    = 0.0f - anchor.y;
	float bottom = 1.0f - anchor.y;

	if (isFlip.x) {
		left  = -left;
		right = -right;
	}

	if (isFlip.y) {
		top    = -top;
		bottom = -bottom;
	}

	(*elements_[index].vertex)[LEFTTOP].position     = { left, top, 1.0f, 1.0f };
	(*elements_[index].vertex)[LEFTBOTTOM].position  = { left, bottom, 1.0f, 1.0f };
	(*elements_[index].vertex)[RIGHTTOP].position    = { right, top, 1.0f, 1.0f };
	(*elements_[index].vertex)[RIGHTBOTTOM].position = { right, bottom, 1.0f, 1.0f };

}

void SpriteCommon::SetTexcoord(uint32_t index, const Vector2f& pivot) {

	float left   = 0.0f - pivot.x;
	float right  = 1.0f - pivot.x;
	float top    = 0.0f - pivot.y;
	float bottom = 1.0f - pivot.y;

	(*elements_[index].vertex)[LEFTTOP].texcoord     = { left, top };
	(*elements_[index].vertex)[LEFTBOTTOM].texcoord  = { left, bottom };
	(*elements_[index].vertex)[RIGHTTOP].texcoord    = { right, top };
	(*elements_[index].vertex)[RIGHTBOTTOM].texcoord = { right, bottom };
}

void SpriteCommon::SetColor(uint32_t index, const Color4f& color) {
	(*elements_[index].vertex)[LEFTTOP].color     = color;
	(*elements_[index].vertex)[LEFTBOTTOM].color  = color;
	(*elements_[index].vertex)[RIGHTTOP].color    = color;
	(*elements_[index].vertex)[RIGHTBOTTOM].color = color;
}

void SpriteCommon::SetTransform2d(uint32_t index, const Vector2f& size, float rotate, const Vector2f& pos) {

	UVTransform transform2d = {};
	transform2d.scale     = size;
	transform2d.rotate    = rotate;
	transform2d.translate = pos;

	(*elements_[index].info)[0].transform2d = transform2d.ToMatrix();
}

void SpriteCommon::SetHandle(uint32_t index, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {
	elements_[index].texture = handle;
}

void SpriteCommon::SetTexcoordClip(int32_t index, const Vector2f& leftTopUV, const Vector2f& sizeUV, const Vector2f& pivot) {

	float left   = 0.0f - pivot.x + leftTopUV.x;
	float right  = 1.0f - pivot.x + leftTopUV.x + sizeUV.x;
	float top    = 0.0f - pivot.y + leftTopUV.y;
	float bottom = 1.0f - pivot.y + leftTopUV.y + sizeUV.y;

	(*elements_[index].vertex)[LEFTTOP].texcoord     = { left, top };
	(*elements_[index].vertex)[LEFTBOTTOM].texcoord  = { left, bottom };
	(*elements_[index].vertex)[RIGHTTOP].texcoord    = { right, top };
	(*elements_[index].vertex)[RIGHTBOTTOM].texcoord = { right, bottom };
}

void SpriteCommon::DrawCall(uint32_t index) {

	auto commandList = Sxavenger::GetCommandList();

	pipeline_->SetPipeline(commandList);
	D3D12_VERTEX_BUFFER_VIEW vbv = elements_[index].vertex->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW ibv  = index_->GetIndexBufferView();

	commandList->IASetVertexBuffers(0, 1, &vbv);
	commandList->IASetIndexBuffer(&ibv);

	commandList->SetGraphicsRootDescriptorTable(0, elements_[index].texture);
	commandList->SetGraphicsRootConstantBufferView(1, elements_[index].info->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(2, projection_->GetGPUVirtualAddress());

	commandList->DrawIndexedInstanced(index_->GetIndexSize(), 1, 0, 0, 0);
}

