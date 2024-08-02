#include "Subobject.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <SubobjectManager.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// Sprite class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Subobjects::SpriteSubobject::Init(const SubobjectManager* manager) {

	// managerの保存
	manager_ = manager;

	sprite_ = std::make_unique<Sprite>();
	sprite_->Init("resources/sxavengerEngine.png", { 0, 0 }, { 200, 200 });
}

void Subobjects::SpriteSubobject::Update() {
}

void Subobjects::SpriteSubobject::Draw(Camera3D* mainCamera) {
	mainCamera; //!< meshではないので使わない
	// todo: maybe_useの追加

	sprite_->Draw();
}

void Subobjects::SpriteSubobject::SetAttributeImGui() {
	sprite_->SetAttributeImGui();

	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// Plane class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Subobjects::Plane::Init(const SubobjectManager* manager) {

	// managerの保存
	manager_ = manager;

	//* IA
	model_ = std::make_unique<Model>(manager_->GetModelDirectory(), "Plane.obj");
	mesh_  = std::make_unique<Mesh>(model_->GetMesh(0).vertexResource.get(), model_->GetMesh(0).indexResource.get());

	//* buffer
	matBuffer_ = std::make_unique<BufferResource<TransformationMatrix>>(MyEngine::GetDevicesObj(), 1);
	material_  = std::make_unique<BufferResource<Material>>(MyEngine::GetDevicesObj(), 1);

	(*material_)[0].Init();
}

void Subobjects::Plane::Update() {

	(*matBuffer_)[0].world      = transform_.ToMatrix();
	(*material_)[0].uvTransform = uvTransform_.ToMatrix();

}

void Subobjects::Plane::Draw(Camera3D* mainCamera) {

	// commandListの取り出し
	auto commandList = MyEngine::GetCommandList();

	// textureを使うpipelineに設定
	manager_->GetPipeline(kTextureMesh)->SetPipeline(commandList);

	// debug
	commandList->SetGraphicsRootConstantBufferView(6, mainCamera->GetGPUVirtualAddress());

	// ParamBuffers
	commandList->SetGraphicsRootConstantBufferView(7, MyEngine::camera3D->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(8, matBuffer_->GetGPUVirtualAddress());
	model_->SetGraphicsTextureHandle(commandList, 0, 9);
	commandList->SetGraphicsRootConstantBufferView(10, material_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(11, manager_->GetLight()->GetGPUVirtualAddress());

	mesh_->Dispatch(0, 1, 2, 3, 4, 5);

}

void Subobjects::Plane::SetAttributeImGui() {

	if (ImGui::TreeNode("transform")) {
		transform_.SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("material")) {
		(*material_)[0].SetImGuiCommand();

		ImGui::Spacing();
		ImGui::Text("UVTransform");
		uvTransform_.SetImGuiCommand();

		ImGui::TreePop();
	}

	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// Sphere class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Subobjects::Sphere::Init(const SubobjectManager* manager) {

	// managerの保存
	manager_ = manager;

	//* IA
	model_ = std::make_unique<Model>(manager_->GetModelDirectory(), "Sphere.obj");
	mesh_ = std::make_unique<Mesh>(model_->GetMesh(0).vertexResource.get(), model_->GetMesh(0).indexResource.get());

	//* buffer
	matBuffer_ = std::make_unique<BufferResource<TransformationMatrix>>(MyEngine::GetDevicesObj(), 1);
	material_ = std::make_unique<BufferResource<Material>>(MyEngine::GetDevicesObj(), 1);

	(*material_)[0].Init();

}

void Subobjects::Sphere::Update() {

	(*matBuffer_)[0].world = transform_.ToMatrix();
	(*material_)[0].uvTransform = uvTransform_.ToMatrix();

}

void Subobjects::Sphere::Draw(Camera3D* mainCamera) {

	// commandListの取り出し
	auto commandList = MyEngine::GetCommandList();

	// textureを使うpipelineに設定
	manager_->GetPipeline(kTextureMesh)->SetPipeline(commandList);

	// debug
	commandList->SetGraphicsRootConstantBufferView(6, mainCamera->GetGPUVirtualAddress());

	// ParamBuffers
	commandList->SetGraphicsRootConstantBufferView(7, MyEngine::camera3D->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(8, matBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(9, MyEngine::GetTextureHandleGPU("resources/uvChecker.png"));
	commandList->SetGraphicsRootConstantBufferView(10, material_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(11, manager_->GetLight()->GetGPUVirtualAddress());

	mesh_->Dispatch(0, 1, 2, 3, 4, 5);

}

void Subobjects::Sphere::SetAttributeImGui() {

	if (ImGui::TreeNode("transform")) {
		transform_.SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("material")) {
		(*material_)[0].SetImGuiCommand();

		ImGui::Spacing();
		ImGui::Text("UVTransform");
		uvTransform_.SetImGuiCommand();

		ImGui::TreePop();
	}

	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// Teapot class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Subobjects::Teapot::Init(const SubobjectManager* manager) {

	// managerの保存
	manager_ = manager;

	//* IA
	model_ = std::make_unique<Model>(manager_->GetModelDirectory(), "teapot.obj");
	mesh_ = std::make_unique<Mesh>(model_->GetMesh(0).vertexResource.get(), model_->GetMesh(0).indexResource.get());

	//* buffer
	matBuffer_ = std::make_unique<BufferResource<TransformationMatrix>>(MyEngine::GetDevicesObj(), 1);
	material_ = std::make_unique<BufferResource<Material>>(MyEngine::GetDevicesObj(), 1);

	(*material_)[0].Init();

}

void Subobjects::Teapot::Update() {

	(*matBuffer_)[0].world = transform_.ToMatrix();
	(*material_)[0].uvTransform = uvTransform_.ToMatrix();

}

void Subobjects::Teapot::Draw(Camera3D* mainCamera) {

	// commandListの取り出し
	auto commandList = MyEngine::GetCommandList();

	// textureを使うpipelineに設定
	manager_->GetPipeline(kTextureMesh)->SetPipeline(commandList);

	// debug
	commandList->SetGraphicsRootConstantBufferView(6, mainCamera->GetGPUVirtualAddress());

	// ParamBuffers
	commandList->SetGraphicsRootConstantBufferView(7, MyEngine::camera3D->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(8, matBuffer_->GetGPUVirtualAddress());
	model_->SetGraphicsTextureHandle(commandList, 0, 9);
	commandList->SetGraphicsRootConstantBufferView(10, material_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(11, manager_->GetLight()->GetGPUVirtualAddress());

	mesh_->Dispatch(0, 1, 2, 3, 4, 5);

}

void Subobjects::Teapot::SetAttributeImGui() {

	if (ImGui::TreeNode("transform")) {
		transform_.SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("material")) {
		(*material_)[0].SetImGuiCommand();

		ImGui::Spacing();
		ImGui::Text("UVTransform");
		uvTransform_.SetImGuiCommand();

		ImGui::TreePop();
	}

	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// Bunny class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Subobjects::Bunny::Init(const SubobjectManager* manager) {

	// managerの保存
	manager_ = manager;

	//* IA
	model_ = std::make_unique<Model>(manager_->GetModelDirectory(), "bunny.obj");
	mesh_ = std::make_unique<Mesh>(model_->GetMesh(0).vertexResource.get(), model_->GetMesh(0).indexResource.get());

	//* buffer
	matBuffer_ = std::make_unique<BufferResource<TransformationMatrix>>(MyEngine::GetDevicesObj(), 1);
	material_ = std::make_unique<BufferResource<Material>>(MyEngine::GetDevicesObj(), 1);

	(*material_)[0].Init();

}

void Subobjects::Bunny::Update() {

	(*matBuffer_)[0].world = transform_.ToMatrix();
	(*material_)[0].uvTransform = uvTransform_.ToMatrix();

}

void Subobjects::Bunny::Draw(Camera3D* mainCamera) {

	// commandListの取り出し
	auto commandList = MyEngine::GetCommandList();

	// textureを使うpipelineに設定
	manager_->GetPipeline(kTextureMesh)->SetPipeline(commandList);

	// debug
	commandList->SetGraphicsRootConstantBufferView(6, mainCamera->GetGPUVirtualAddress());

	// ParamBuffers
	commandList->SetGraphicsRootConstantBufferView(7, MyEngine::camera3D->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(8, matBuffer_->GetGPUVirtualAddress());
	model_->SetGraphicsTextureHandle(commandList, 0, 9);
	commandList->SetGraphicsRootConstantBufferView(10, material_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(11, manager_->GetLight()->GetGPUVirtualAddress());

	mesh_->Dispatch(0, 1, 2, 3, 4, 5);

}

void Subobjects::Bunny::SetAttributeImGui() {

	if (ImGui::TreeNode("transform")) {
		transform_.SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("material")) {
		(*material_)[0].SetImGuiCommand();

		ImGui::Spacing();
		ImGui::Text("UVTransform");
		uvTransform_.SetImGuiCommand();

		ImGui::TreePop();
	}

	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}

}


////////////////////////////////////////////////////////////////////////////////////////////
// MultiMesh class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Subobjects::MultiMesh::Init(const SubobjectManager* manager) {

	// managerの保存
	manager_ = manager;

	//* IA
	model_ = std::make_unique<Model>(manager_->GetModelDirectory(), "multiMesh.obj");

	// modelのmesh分の容量確保
	meshes_.resize(model_->GetModelIndexSize());

	for (uint32_t i = 0; i < model_->GetModelIndexSize(); ++i) {
		meshes_.at(i)
			= std::make_unique<Mesh>(model_->GetMesh(i).vertexResource.get(), model_->GetMesh(i).indexResource.get());
	}

	//* buffer
	matBuffer_ = std::make_unique<BufferResource<TransformationMatrix>>(MyEngine::GetDevicesObj(), 1);
	material_  = std::make_unique<BufferResource<Material>>(MyEngine::GetDevicesObj(), 2);

	(*material_)[0].Init();

}

void Subobjects::MultiMesh::Update() {

	(*matBuffer_)[0].world = transform_.ToMatrix();
	(*material_)[0].uvTransform = uvTransform_.ToMatrix();

}

void Subobjects::MultiMesh::Draw(Camera3D* mainCamera) {

	// commandListの取り出し
	auto commandList = MyEngine::GetCommandList();

	// textureを使うpipelineに設定
	manager_->GetPipeline(kTextureMesh)->SetPipeline(commandList);

	for (uint32_t i = 0; i < model_->GetModelIndexSize(); ++i) {
		// debug
		commandList->SetGraphicsRootConstantBufferView(6, mainCamera->GetGPUVirtualAddress());

		// ParamBuffers
		commandList->SetGraphicsRootConstantBufferView(7, MyEngine::camera3D->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(8, matBuffer_->GetGPUVirtualAddress());
		model_->SetGraphicsTextureHandle(commandList, 0, 9);
		commandList->SetGraphicsRootConstantBufferView(10, material_->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(11, manager_->GetLight()->GetGPUVirtualAddress());

		meshes_.at(i)->Dispatch(0, 1, 2, 3, 4, 5);
	}

	

}

void Subobjects::MultiMesh::SetAttributeImGui() {

	if (ImGui::TreeNode("transform")) {
		transform_.SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("material")) {
		(*material_)[0].SetImGuiCommand();

		ImGui::Spacing();
		ImGui::Text("UVTransform");
		uvTransform_.SetImGuiCommand();

		ImGui::TreePop();
	}

	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// MultiMaterial class
////////////////////////////////////////////////////////////////////////////////////////////

void Subobjects::MultiMaterial::Init(const SubobjectManager* manager) {

	// managerの保存
	manager_ = manager;

	//* IA
	model_ = std::make_unique<Model>(manager_->GetModelDirectory(), "multiMaterial.obj");

	//* buffer
	matBuffer_ = std::make_unique<BufferResource<TransformationMatrix>>(MyEngine::GetDevicesObj(), 1);

	//* meshs & materials
	// mesh数の取得
	meshNum_ = model_->GetModelIndexSize();

	// modelのmesh分の容量確保
	meshes_.resize(meshNum_);

	// mesh分のmaterialの確保
	materials_.resize(meshNum_);
	uvTransforms_.resize(meshNum_);

	// 初期化処理
	for (uint32_t i = 0; i < meshNum_; ++i) {

		// mesh
		meshes_.at(i)
			= std::make_unique<Mesh>(model_->GetMesh(i).vertexResource.get(), model_->GetMesh(i).indexResource.get());

		// material
		materials_[i]
			= std::make_unique<BufferResource<Material>>(MyEngine::GetDevicesObj(), 1);

		(*materials_[i])[0].Init();
	}
}

void Subobjects::MultiMaterial::Update() {

	(*matBuffer_)[0].world = transform_.ToMatrix();

	for (uint32_t i = 0; i < meshNum_; ++i) {
		(*materials_[i])[0].uvTransform = uvTransforms_.at(i).ToMatrix();
	}

}

void Subobjects::MultiMaterial::Draw(Camera3D* mainCamera) {

	// commandListの取り出し
	auto commandList = MyEngine::GetCommandList();

	// textureを使うpipelineに設定
	manager_->GetPipeline(kTextureMesh)->SetPipeline(commandList);

	for (uint32_t i = 0; i < model_->GetModelIndexSize(); ++i) {
		// debug
		commandList->SetGraphicsRootConstantBufferView(6, mainCamera->GetGPUVirtualAddress());

		// ParamBuffers
		commandList->SetGraphicsRootConstantBufferView(7, MyEngine::camera3D->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(8, matBuffer_->GetGPUVirtualAddress());
		model_->SetGraphicsTextureHandle(commandList, i, 9);
		commandList->SetGraphicsRootConstantBufferView(10, materials_[i]->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(11, manager_->GetLight()->GetGPUVirtualAddress());

		meshes_.at(i)->Dispatch(0, 1, 2, 3, 4, 5);
	}

}

void Subobjects::MultiMaterial::SetAttributeImGui() {

	if (ImGui::TreeNode("transform")) {
		transform_.SetImGuiCommand();
		ImGui::TreePop();
	}

	for (uint32_t i = 0; i < meshNum_; ++i) {

		std::string label = "material##" + std::to_string(i);

		if (ImGui::TreeNode(label.c_str())) {

			(*materials_[i])[0].SetImGuiCommand();

			ImGui::Spacing();
			ImGui::Text("UVTransform");
			uvTransforms_.at(i).SetImGuiCommand();

			ImGui::TreePop();

		}
	}

	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}

}

////////////////////////////////////////////////////////////////////////////////////////////
// Suzanne class
////////////////////////////////////////////////////////////////////////////////////////////

void Subobjects::Suzanne::Init(const SubobjectManager* manager) {

	// managerの保存
	manager_ = manager;

	//* IA
	model_ = std::make_unique<Model>(manager_->GetModelDirectory(), "Suzanne.obj");
	mesh_ = std::make_unique<Mesh>(model_->GetMesh(0).vertexResource.get(), model_->GetMesh(0).indexResource.get());

	//* buffer
	matBuffer_ = std::make_unique<BufferResource<TransformationMatrix>>(MyEngine::GetDevicesObj(), 1);
	material_ = std::make_unique<BufferResource<Material>>(MyEngine::GetDevicesObj(), 1);

	(*material_)[0].Init();
}

void Subobjects::Suzanne::Update() {

	(*matBuffer_)[0].world = transform_.ToMatrix();

}

void Subobjects::Suzanne::Draw(Camera3D* mainCamera) {

	// commandListの取り出し
	auto commandList = MyEngine::GetCommandList();

	// textureを使うpipelineに設定
	manager_->GetPipeline(kNotTextureMesh)->SetPipeline(commandList);

	// debug
	commandList->SetGraphicsRootConstantBufferView(6, mainCamera->GetGPUVirtualAddress());

	// ParamBuffers
	commandList->SetGraphicsRootConstantBufferView(7, MyEngine::camera3D->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(8, matBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(9, material_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(10, manager_->GetLight()->GetGPUVirtualAddress());

	mesh_->Dispatch(0, 1, 2, 3, 4, 5);

}

void Subobjects::Suzanne::SetAttributeImGui() {
	if (ImGui::TreeNode("transform")) {
		transform_.SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("material")) {
		(*material_)[0].SetImGuiCommand();

		ImGui::TreePop();
	}

	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}
}
