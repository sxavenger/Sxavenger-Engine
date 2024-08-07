#include "DebugObjectManager.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include <MyEngine.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// DebugObject base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DebugObjectManager::Init() {
	CreatePipeline();
}

void DebugObjectManager::Update() {
	// deleteされたsubobjectの削除
	for (auto it = debugObjects_.begin(); it != debugObjects_.end();) {

		if ((*it)->IsDelete()) {
			EraseAttributeNode(it->get());
			it = debugObjects_.erase(it);
			continue;
		}

		it++;
	}
}

void DebugObjectManager::Draw() {

	auto commandList = MyEngine::GetCommandList();

	pipeline_->SetPipeline(commandList);

	for (auto& object : debugObjects_) {
		object->Draw();
	}
}

void DebugObjectManager::SetAttributeImGui() {
}

void DebugObjectManager::CreatePipeline() {

	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"DebugObject/DebugObject.VS.hlsl", GRAPHICS_VERTEX);
	blob_->Create(L"DebugObject/DebugObject.PS.hlsl", GRAPHICS_PIXEL);

	pipeline_ = std::make_unique<GraphicsPipeline>();

	{
		GraphicsRootSignatureDesc desc;
		desc.Resize(2, 0);
		desc.SetCBV(0, VISIBILITY_ALL, 0);
		desc.SetCBV(1, VISIBILITY_VERTEX, 1);

		pipeline_->CreateRootSignature(MyEngine::GetDevicesObj(), desc);
	}

	pipeline_->CreatePipeline(MyEngine::GetDevicesObj(), blob_.get(), kBlendModeNormal);

}
