#include "NodeModel.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

//=========================================================================================
// static variables
//=========================================================================================

const std::string NodeModel::modelNames_[kCountOfNodeModelType] = {
	"Node_Attribute_01.gltf",
	"Node_Attribute_02.gltf",
	"Node_Attribute_03.gltf",
	"Node_Attribute_08.gltf",
};

////////////////////////////////////////////////////////////////////////////////////////////
// NodeModel class methods
////////////////////////////////////////////////////////////////////////////////////////////

void NodeModel::Init(DefferedRendering* deffered) {

	// Graphics
	blob_ = std::make_unique<GraphicsBlob>();
	blob_->Create(L"object3d/object3d.VS.hlsl", GRAPHICS_VERTEX);
	blob_->Create(L"object3d/object3dDeffered.PS.hlsl", GRAPHICS_PIXEL);

	GraphicRootSignatureDesc desc;
	desc.Resize(3, 1);
	desc.SetCBV(0, VISIBILITY_VERTEX, 0);
	desc.SetCBV(1, VISIBILITY_VERTEX, 1);
	desc.SetSRV(2, VISIBILITY_PIXEL, 0);
	desc.SetSampler(0, MODE_WRAP, VISIBILITY_PIXEL, 0);

	pipeline_ = std::make_unique<GraphicsPipeline>();
	pipeline_->CreateRootSiganture(MyEngine::GetDevicesObj(), desc);
	pipeline_->CreatePipeline(MyEngine::GetDevicesObj(), blob_.get(), kBlendModeNormal, DefferedRenderingType::kCountOfDefferedRenderingType, deffered->GetFormats());

	// IA
	model_ = std::make_unique<Model>("./Resources/model/node_model", modelNames_[type_]);

	// buffer
	matrixBuffers_.resize(model_->GetModelIndexSize());

	for (uint32_t i = 0; i < model_->GetModelIndexSize(); ++i) {
		matrixBuffers_[i] = std::make_unique<BufferResource<Matrix4x4>>(MyEngine::GetDevicesObj(), 1);
		matrixBuffers_[i]->operator[](0) = Matrix4x4::MakeIdentity();
	}

	SetThisAttribute("nodeModel");

}

void NodeModel::Term() {
}

void NodeModel::Update() {

	matrixBuffers_[0]->operator[](0) = model_->GetNode().localMatrix * worldMatrix_;
	matrixBuffers_[1]->operator[](0) = model_->GetNode().children[0].localMatrix * worldMatrix_;

}

void NodeModel::Draw() {

	// commandListの取得
	auto commandList = MyEngine::GetCommandList();

	pipeline_->SetPipeline(commandList);

	for (uint32_t i = 0; i < model_->GetModelIndexSize(); ++i) {

		model_->SetBuffers(commandList, i);

		commandList->SetGraphicsRootConstantBufferView(0, MyEngine::camera3D_->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(1, matrixBuffers_[i]->GetGPUVirtualAddress());
		model_->SetGraphicsTextureHandle(commandList, i, 2, TEXTURE_DIFFUSE);

		model_->DrawCall(commandList, i);

	}

}

void NodeModel::SetAttributeImGui() {
	
	transform_.SetImGuiCommand();
	worldMatrix_ = transform_.SetMatrix();

	if (ImGui::BeginCombo("modelType", modelNames_[type_].c_str())) {

		for (int i = 0; i < kCountOfNodeModelType; ++i) {
			bool isSelect = (i == type_);

			if (ImGui::Selectable(modelNames_[i].c_str(), isSelect)) {
				type_ = static_cast<NodeModelType>(i);

				if (!isSelect) { //!< model読み込みされていないので新しく読み込み
					model_ = std::make_unique<Model>("./Resources/model/node_model", modelNames_[type_]);
				}
			}
		}

		ImGui::EndCombo();
	}

}
