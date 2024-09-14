#include "Sprite.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include <Sxavenger.h>

// enviormant
#include <Environment.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// Sprite class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Sprite::Init(const std::string& texturefilePath, const Vector2i& pos, const Vector2i& size) {
	
	//* Graphics
	CreatePipeline();

	//* data
	pos_  = pos;
	size_ = size;

	//* ia
	ia_.Create();
	ia_.CalculateVertex(pos_, size_);

	//* buffer
	material_ = std::make_unique<BufferResource<SpriteMaterial>>(Sxavenger::GetDevicesObj(), 1);
	(*material_)[0].Init();

	//* texture
	filePath_ = texturefilePath;
	texture_ = Sxavenger::LoadTexture(filePath_);

	//* attribute
	SetAttributeName("sprite");

}

void Sprite::Term() {
	Sxavenger::ReleaseTexture(filePath_);
}

void Sprite::Draw() {

	// commandListの取り出し
	auto commandList = Sxavenger::GetCommandList();

	pipeline_->SetPipeline(commandList);
	ia_.SetIABuffer();

	commandList->SetGraphicsRootConstantBufferView(0, material_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(1, texture_->GetGPUHandleSRV());

	ia_.DrawCall();

}

void Sprite::SetAttributeImGui() {

	if (ImGui::TreeNode("IA")) {

		ImGui::DragInt2("position", &pos_.x);
		ImGui::DragInt2("size",     &size_.x);

		ia_.CalculateVertex(pos_, size_);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Material")) {

		ImGui::ColorEdit4("color", &(*material_)[0].color.r);
		ImGui::DragFloat2("pivot", &(*material_)[0].pivot.x, 0.01f);
		
		ImGui::Spacing();
		ImGui::Text("uvTransform");
		uvTransform_.SetImGuiCommand();

		(*material_)[0].uvTransform = uvTransform_.ToMatrix();

		ImGui::TreePop();
	}
}

void Sprite::CreatePipeline() {

	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"sprite/sprite.VS.hlsl", GRAPHICS_VERTEX);
	blob_->Create(L"sprite/sprite.PS.hlsl", GRAPHICS_PIXEL);

	pipeline_ = std::make_unique<GraphicsPipeline>();

	{
		GraphicsRootSignatureDesc desc;
		desc.SetCBV(0, VISIBILITY_PIXEL, 0); //!< material

		desc.SetSRV(1, VISIBILITY_PIXEL, 0); //!< texture
		desc.SetSampler(0, MODE_WRAP, VISIBILITY_PIXEL, 0);

		pipeline_->CreateRootSignature(Sxavenger::GetDevicesObj(), desc);
	}

	{
		pipeline_->CreatePipeline(Sxavenger::GetDevicesObj(), blob_.get(), kBlendModeNormal);
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// SpriteIA structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void Sprite::SpriteIA::Create() {

	vertex = std::make_unique<BufferResource<VertexData>>(Sxavenger::GetDevicesObj(), 4/*vertexNum*/);
	index  = std::make_unique<IndexBufferResource>(Sxavenger::GetDevicesObj(), 3/* triangleVertexNum */ * 2/* triangleCount */);

	// indexは固定なので設定
	(*index)[0] = LEFTBOTTOM;
	(*index)[1] = LEFTTOP;
	(*index)[2] = RIGHTBOTTOM;

	(*index)[3] = LEFTTOP;
	(*index)[4] = RIGHTTOP;
	(*index)[5] = RIGHTBOTTOM;
}

void Sprite::SpriteIA::CalculateVertex(const Vector2i& pos, const Vector2i& size) const {
	
	Vector2f posf  = static_cast<Vector2f>(pos);
	Vector2f sizef = static_cast<Vector2f>(size);

	(*vertex)[LEFTTOP].position = ToProjection(posf);
	(*vertex)[LEFTTOP].texcoord = { 0.0f, 0.0f };

	(*vertex)[LEFTBOTTOM].position = ToProjection(posf + Vector2f{ 0.0f, sizef.y });
	(*vertex)[LEFTBOTTOM].texcoord = { 0.0f, 1.0f };

	(*vertex)[RIGHTTOP].position = ToProjection(posf + Vector2f{ sizef.x, 0.0f });
	(*vertex)[RIGHTTOP].texcoord = { 1.0f, 0.0f };

	(*vertex)[RIGHTBOTTOM].position = ToProjection(posf + sizef);
	(*vertex)[RIGHTBOTTOM].texcoord = { 1.0f, 1.0f };

}

void Sprite::SpriteIA::SetIABuffer() {

	D3D12_VERTEX_BUFFER_VIEW vertexBuffer = vertex->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW indexBuffer   = index->GetIndexBufferView();

	auto commandList = Sxavenger::GetCommandList();

	commandList->IASetVertexBuffers(0, 1, &vertexBuffer);
	commandList->IASetIndexBuffer(&indexBuffer);
}

void Sprite::SpriteIA::DrawCall() {
	auto commandList = Sxavenger::GetCommandList();
	commandList->DrawIndexedInstanced(index->GetIndexSize(), 1, 0, 0, 0);
}

Vector4f Sprite::SpriteIA::ToProjection(const Vector2f& screenPostion) {

	Vector4f result
		= { (screenPostion.x / kWindowWidth) * 2.0f - 1.0f, (screenPostion.y / kWindowHeight) * 2.0f - 1.0f, 0.0f, 1.0f };

	// screen座標からprojection(左上: -1, 1)に変換
	result.y *= -1.0f;

	return result;
}
