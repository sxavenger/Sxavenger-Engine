#include "SubobjectManager.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// subobject
#include <Cube.h>

// c++
#include <format>

//=========================================================================================
// static variables
//=========================================================================================

const std::string SubobjectManager::objectName_[SubobjectType::kCountOfSubobjectType] = {
	"cube", "teapot", "plane", "sphere", "squarePyramid"
};

////////////////////////////////////////////////////////////////////////////////////////////
// SubobjectManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SubobjectManager::Init() {
	// modelの生成
	for (int i = 0; i < SubobjectType::kCountOfSubobjectType; ++i) {
		models_[i] = std::make_unique<Model>("./Resources/model", objectName_[i] + ".obj");
		meshStructuredBuffers_[i].Create(models_[i].get());
	}

	SetThisAttribute("subobjectManager");
}

void SubobjectManager::Term() {
	subobjects_.clear();
}

void SubobjectManager::Update() {
	// deleteされたsubobjectの削除
	for (auto it = subobjects_.begin(); it != subobjects_.end();) {

		if (it->get()->IsDelete()) {
			EraseAttributeNode(it->get());
			it = subobjects_.erase(it);
			continue;
		}

		it++;
	}
}

void SubobjectManager::SetBlases(DxrObject::TopLevelAS* tlas) {
	for (auto& subobject : subobjects_) {
		subobject->SetOnTLAS(tlas);
	}
}

void SubobjectManager::SetAttributeImGui() {

	if (ImGui::BeginCombo("subobject", objectName_[selectedType_].c_str())) {
		for (int i = 0; i < kCountOfSubobjectType; ++i) {
			bool isSelect = (i == selectedType_);

			if (ImGui::Selectable(objectName_[i].c_str(), isSelect)) {
				selectedType_ = static_cast<SubobjectType>(i);
			}
		}

		ImGui::EndCombo();
	}

	if (ImGui::Button("Create")) {

		std::unique_ptr<Subobject> newObject = std::make_unique<Subobject>();
		newObject->Init(
			models_[selectedType_]->GetMesh(0).vertexResource.get(), models_[selectedType_]->GetMesh(0).indexResource.get(),
			meshStructuredBuffers_[selectedType_].vertex.get(), meshStructuredBuffers_[selectedType_].index.get()
		);

		newObject->SetAttributeName(objectName_[selectedType_].c_str());

		SetAttributeNode(newObject.get());
		subobjects_.push_back(std::move(newObject));
	}

	ImGui::Spacing();

	std::string text = std::format("subobject num: {}", subobjects_.size());
	ImGui::Text(text.c_str());

	if (ImGui::Button("DeleteAll")) {
		for (auto& subobject : subobjects_) {
			subobject->SetIsDelete(true);
		}
	}
}

void SubobjectManager::GetBlasModelData(DxrObject::BottomLevelAS* dst, const std::wstring& hitgroupName, SubobjectType type) {
	dst->Create(
		models_[type]->GetMesh(0).vertexResource.get(), models_[type]->GetMesh(0).indexResource.get(),
		meshStructuredBuffers_[type].vertex.get(), meshStructuredBuffers_[type].index.get(),
		hitgroupName
	);

	return;
}
