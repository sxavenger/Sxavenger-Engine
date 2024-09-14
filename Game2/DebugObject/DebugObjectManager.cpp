#include "DebugObjectManager.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// engine
#include <Sxavenger.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

//=========================================================================================
// static variables
//=========================================================================================

const LPCSTR DebugObjectManager::kObjectNames_[DebugObjectType::kCountOfDebugObjectType] = {
	"Sphere", "Box"
};

////////////////////////////////////////////////////////////////////////////////////////////
// DebugObject base class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DebugObjectManager::Init() {
	CreatePipeline();

	SetThisAttribute("debugObject");
}

void DebugObjectManager::Update() {
	// deleteされたsubobjectの削除
	for (auto it = debugObjects_.begin(); it != debugObjects_.end();) {

		if ((*it)->IsDelete()) {
			EraseAttributeNode(it->get());
			it = debugObjects_.erase(it);
			continue;
		}

		(*it)->Update();
		it++;
	}
}

void DebugObjectManager::Draw() {

	auto commandList = Sxavenger::GetCommandList();

	pipeline_->SetPipeline(commandList);

	for (auto& object : debugObjects_) {
		object->Draw();
	}
}

void DebugObjectManager::SetAttributeImGui() {

	if (ImGui::BeginCombo("DebugObject", kObjectNames_[selectedObjectType_])) {
		for (uint32_t i = 0; i < DebugObjectType::kCountOfDebugObjectType; ++i) {

			bool isSelected = (i == selectedObjectType_);

			if (ImGui::Selectable(kObjectNames_[i], isSelected)) {
				selectedObjectType_ = static_cast<DebugObjectType>(i);
			}
		}

		ImGui::EndCombo();
	}

	if (ImGui::Button("Create")) {
		CreateObject();
	}
}

void DebugObjectManager::CreatePipeline() {

	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"DebugObject/DebugObject.VS.hlsl", GRAPHICS_VERTEX);
	blob_->Create(L"DebugObject/DebugObject.PS.hlsl", GRAPHICS_PIXEL);

	pipeline_ = std::make_unique<GraphicsPipeline>();

	{
		GraphicsRootSignatureDesc desc;
		desc.SetCBV(0, VISIBILITY_ALL, 0);
		desc.SetCBV(1, VISIBILITY_VERTEX, 1);
		desc.SetCBV(2, VISIBILITY_PIXEL, 1);

		pipeline_->CreateRootSignature(Sxavenger::GetDevicesObj(), desc);
	}

	pipeline_->CreatePipeline(Sxavenger::GetDevicesObj(), blob_.get(), kBlendModeNormal);

}

void DebugObjectManager::CreateObject() {
	
	switch (selectedObjectType_) {
		case DebugObjectType::kSphere: {

				std::unique_ptr<DebugObject> newObject = std::make_unique<DebugObjects::Sphere>();
				newObject->Init();
				
				SetAttributeNode(newObject.get());
				debugObjects_.emplace_back(std::move(newObject));
			}
			break;


		case DebugObjectType::kBox: {

				std::unique_ptr<DebugObject> newObject = std::make_unique<DebugObjects::Box>();
				newObject->Init();

				SetAttributeNode(newObject.get());
				debugObjects_.emplace_back(std::move(newObject));
			}
			break;

		default:
			break;
	}

}
