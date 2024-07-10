#include "Bloom.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>
#include <Environment.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// Bloom class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Bloom::Init() {

	SetThisAttribute("Bloom");

	renderTargetTexture_ = std::make_unique<RenderTexture>();
	renderTargetTexture_->Create(MyEngine::GetDxCommon(), kWindowWidth, kWindowHeight, { 0.0f, 0.0f, 0.0f, 1.0f }, DXGI_FORMAT_R32G32B32A32_FLOAT);

	luminnceTexture_ = std::make_unique<RenderTexture>();
	luminnceTexture_->Create(MyEngine::GetDxCommon(), kWindowWidth, kWindowHeight, { 0.0f, 0.0f, 0.0f, 1.0f }, DXGI_FORMAT_R32G32B32A32_FLOAT);

	luminnceBlurTexture_ = std::make_unique<RenderTexture>();
	luminnceBlurTexture_->Create(MyEngine::GetDxCommon(), kWindowWidth, kWindowHeight, { 0.0f, 0.0f, 0.0f, 1.0f }, DXGI_FORMAT_R32G32B32A32_FLOAT);

	sprite_ = DrawMethods::Plane2D();

	//!< grahics
	GraphicsRootSignatureDesc rootDesc;
	rootDesc.Resize(1, 1);
	rootDesc.SetSRV(0, VISIBILITY_PIXEL, 0);
	rootDesc.SetSampler(0, MODE_CLAMP, VISIBILITY_PIXEL, 0);

	GraphicsPipelineDesc pipelineDesc;
	pipelineDesc.CreateDefaultDesc();

	pipelineDesc.rtvFormats.clear();
	pipelineDesc.SetRTVFormat(DXGI_FORMAT_R32G32B32A32_FLOAT);

	{ //!< 輝度抽出Texture用
		blob_[LUMINNCE] = std::make_unique<GraphicsBlob>();
		blob_[LUMINNCE]->Create(L"Sprite.VS.hlsl", GRAPHICS_VERTEX);
		blob_[LUMINNCE]->Create(L"bloom/Luminnce.PS.hlsl", GRAPHICS_PIXEL);

		pipeline_[LUMINNCE] = std::make_unique<GraphicsPipeline>();
		pipeline_[LUMINNCE]->CreateRootSignature(MyEngine::GetDevicesObj(), rootDesc);
		pipeline_[LUMINNCE]->CreatePipeline(MyEngine::GetDevicesObj(), blob_[LUMINNCE].get(), pipelineDesc);
	}

	//{ //!< ブラー用 todo: blurクラスを使ってblurする
	//	blob_[BLUR] = std::make_unique<GraphicsBlob>();
	//	blob_[BLUR]->Create(L"Sprite.VS.hlsl", GRAPHICS_VERTEX);
	//	blob_[BLUR]->Create(L"bloom/LuminnceBlur.PS.hlsl", GRAPHICS_PIXEL);

	//	pipeline_[BLUR] = std::make_unique<GraphicsPipeline>();
	//	pipeline_[BLUR]->CreateRootSignature(MyEngine::GetDevicesObj(), rootDesc);
	//	pipeline_[BLUR]->CreatePipeline(MyEngine::GetDevicesObj(), blob_[BLUR].get(), pipelineDesc);
	//}

	blur_ = std::make_unique<GaussianBlur>(DXGI_FORMAT_R32G32B32A32_FLOAT);

	{
		blob_[COMPOSITE] = std::make_unique<GraphicsBlob>();
		blob_[COMPOSITE]->Create(L"Sprite.VS.hlsl", GRAPHICS_VERTEX);
		blob_[COMPOSITE]->Create(L"bloom/Composite.PS.hlsl", GRAPHICS_PIXEL);

		pipeline_[COMPOSITE] = std::make_unique<GraphicsPipeline>();
		pipeline_[COMPOSITE]->CreateRootSignature(MyEngine::GetDevicesObj(), rootDesc);
		pipeline_[COMPOSITE]->CreatePipeline(MyEngine::GetDevicesObj(), blob_[COMPOSITE].get(), pipelineDesc);
		
		pipelineDesc.SetBlendMode(kBlendModeAdd);

		pipeline_[COMPOSITE_ADD] = std::make_unique<GraphicsPipeline>();
		pipeline_[COMPOSITE_ADD]->CreateRootSignature(MyEngine::GetDevicesObj(), rootDesc);
		pipeline_[COMPOSITE_ADD]->CreatePipeline(MyEngine::GetDevicesObj(), blob_[COMPOSITE].get(), pipelineDesc);

		pipelineDesc.SetBlendMode(kBlendModeNormal);

		// HACK: このくらいspriteのクラス作れ。

		pipelineDesc.rtvFormats.clear();
		pipelineDesc.SetRTVFormat(defaultFormat);

		pipeline_[RESULT] = std::make_unique<GraphicsPipeline>();
		pipeline_[RESULT]->CreateRootSignature(MyEngine::GetDevicesObj(), rootDesc);
		pipeline_[RESULT]->CreatePipeline(MyEngine::GetDevicesObj(), blob_[COMPOSITE].get(), pipelineDesc);
	}
}

void Bloom::Term() {
}

void Bloom::CreateBloom() { //!< 名前がなんか気に入らない

	// コマンドリストの取り出し
	auto commandList = MyEngine::GetCommandList();

	D3D12_VERTEX_BUFFER_VIEW vertexBuffer = sprite_.vertex->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW indexBuffer = sprite_.index->GetIndexBufferView();

	{ //!< 輝度の取得
		MyEngine::BeginOffscreen(luminnceTexture_.get());

		pipeline_[LUMINNCE]->SetPipeline(commandList);

		commandList->IASetVertexBuffers(0, 1, &vertexBuffer);
		commandList->IASetIndexBuffer(&indexBuffer);

		commandList->SetGraphicsRootDescriptorTable(0, renderTargetTexture_->GetGPUHandleSRV());

		commandList->DrawIndexedInstanced(sprite_.index->GetIndexSize(), 1, 0, 0, 0);

		MyEngine::EndOffscreen(luminnceTexture_.get());
	}

	{
		MyEngine::BeginOffscreen(luminnceBlurTexture_.get());

		/*pipeline_[BLUR]->SetPipeline(commandList);

		commandList->IASetVertexBuffers(0, 1, &vertexBuffer);
		commandList->IASetIndexBuffer(&indexBuffer);

		commandList->SetGraphicsRootDescriptorTable(0, luminnceTexture_->GetGPUHandleSRV());

		commandList->DrawIndexedInstanced(sprite_.index->GetIndexSize(), 1, 0, 0, 0);*/

		blur_->CreateBlurTexture(
			luminnceTexture_.get(), { kWindowWidth, kWindowHeight }, blurStrength_, sigma_
		);

		MyEngine::EndOffscreen(luminnceBlurTexture_.get());
	}

	{
		MyEngine::BeginOffscreen(luminnceTexture_.get()); //!< 輝度Textureを最終Textureに指定

		//!< 下レイヤー
		pipeline_[COMPOSITE]->SetPipeline(commandList);

		commandList->IASetVertexBuffers(0, 1, &vertexBuffer);
		commandList->IASetIndexBuffer(&indexBuffer);

		commandList->SetGraphicsRootDescriptorTable(0, renderTargetTexture_->GetGPUHandleSRV());

		commandList->DrawIndexedInstanced(sprite_.index->GetIndexSize(), 1, 0, 0, 0);

		//!< 加算合成
		pipeline_[COMPOSITE_ADD]->SetPipeline(commandList);

		commandList->IASetVertexBuffers(0, 1, &vertexBuffer);
		commandList->IASetIndexBuffer(&indexBuffer);

		commandList->SetGraphicsRootDescriptorTable(0, luminnceBlurTexture_->GetGPUHandleSRV());

		commandList->DrawIndexedInstanced(sprite_.index->GetIndexSize(), 1, 0, 0, 0);

		MyEngine::EndOffscreen(luminnceTexture_.get());
	}
}

void Bloom::Draw() {

	// コマンドリストの取り出し
	auto commandList = MyEngine::GetCommandList();

	D3D12_VERTEX_BUFFER_VIEW vertexBuffer = sprite_.vertex->GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW indexBuffer = sprite_.index->GetIndexBufferView();

	pipeline_[RESULT]->SetPipeline(commandList);

	commandList->IASetVertexBuffers(0, 1, &vertexBuffer);
	commandList->IASetIndexBuffer(&indexBuffer);

	commandList->SetGraphicsRootDescriptorTable(0, luminnceTexture_->GetGPUHandleSRV());

	commandList->DrawIndexedInstanced(sprite_.index->GetIndexSize(), 1, 0, 0, 0);

}

void Bloom::SetAttributeImGui() {

	static Vector2i strength = static_cast<Vector2i>(blurStrength_);

	ImGui::DragInt2("strength", &strength.x, 1, 0, 100);
	blurStrength_ = static_cast<Vector2ui>(strength);

	ImGui::DragFloat("sigma", &sigma_, 0.01f);

}
