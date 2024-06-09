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

const std::string SubobjectManager::filePaths_[SubobjectType::kCountOfSubobjectType] = {
	"cube.obj"
};

////////////////////////////////////////////////////////////////////////////////////////////
// SubobjectManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SubobjectManager::Init() {
	// modelの生成
	for (int i = 0; i < SubobjectType::kCountOfSubobjectType; ++i) {
		models_[i] = std::make_unique<Model>("./Resources/model", filePaths_[i]);
		meshStructuredBuffers_[i].Create(models_[i].get());
	}

	SetThisAttribute("subobjectManager");
}

void SubobjectManager::Term() {
	subobjects_.clear();
}

void SubobjectManager::SetBlases(DxrObject::TopLevelAS* tlas) {
	for (auto& subobject : subobjects_) {
		subobject->SetOnTLAS(tlas);
	}
}

void SubobjectManager::SetAttributeImGui() {
	
	// todo: いろんなobjectが使えるように
	if (ImGui::Button("Create Cube")) {

		std::unique_ptr<Cube> newCube = std::make_unique<Cube>();
		newCube->Init(
			models_[TYPE_CUBE]->GetMeshData(0).vertexResource.get(), models_[TYPE_CUBE]->GetMeshData(0).indexResource.get(),
			meshStructuredBuffers_[TYPE_CUBE].vertex.get(), meshStructuredBuffers_[TYPE_CUBE].index.get()
		);

		subobjects_.push_back(std::move(newCube));
	}

	ImGui::Separator();
	ImGui::Spacing();

	// リストに登録されているsubobjectのImGui編集
	int id = 0; //!< stackの対策

	for (auto& subobject : subobjects_) {
		subobject->SetOnImGui(id);
		id++;
	}

	// deleteされたsubobjectの削除
	subobjects_.remove_if([](auto& subobject) {
		return subobject->IsDelete();
	});

	ImGui::Spacing();

	std::string text = std::format("subobject num: {}", subobjects_.size());
	ImGui::Text(text.c_str());
}
