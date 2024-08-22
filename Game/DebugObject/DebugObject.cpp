#include "DebugObject.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>
#include <Console.h>
#include <format>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

////////////////////////////////////////////////////////////////////////////////////////////
// DebugObject class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DebugObjects::Sphere::Init() {
	ia_ = DrawMethods::Sphere(1.0f, 10); //!< test value

	transform_.Init();

	material_ = std::make_unique<BufferResource<Material>>(MyEngine::GetDevicesObj(), 1);
	(*material_)[0].Init();

	SetAttributeName("Sphere");

	Collider::SetColliderBoundingSphere();

	Collider::typeId_       = 1;
	Collider::targetTypeId_ = 1;
}

void DebugObjects::Sphere::Term() {
}

void DebugObjects::Sphere::Update() {
	transform_.UpdateMatrix();
}

void DebugObjects::Sphere::Draw() {

	auto commandList = MyEngine::GetCommandList();

	ia_.SetBuffer(commandList);

	commandList->SetGraphicsRootConstantBufferView(0, MyEngine::camera3D->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, transform_.GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(2, material_->GetGPUVirtualAddress());

	ia_.DrawCall(commandList);

}

void DebugObjects::Sphere::SetAttributeImGui() {
	if (ImGui::TreeNode("transform")) {
		transform_.transform.SetImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("collider")) {
		SetColliderImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}
}

void DebugObjects::Sphere::OnCollisionEnter(_MAYBE_UNUSED Collider* const other) {
	(*material_)[0].color = { 1.0f, 0.0f, 0.0f, 0.5f };

	std::string log = std::format("Sphere OnCollision Enter ptr: {}", reinterpret_cast<void*>(this));
	console->Log(log.c_str());
}

void DebugObjects::Sphere::OnCollisionExit(_MAYBE_UNUSED Collider* const other) {
	(*material_)[0].color = { 1.0f, 1.0f, 1.0f, 0.5f };

	std::string log = std::format("Sphere OnCollision Exit ptr: {}", reinterpret_cast<void*>(this));
	console->Log(log.c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////
// Box(collider: AABB) class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DebugObjects::Box::Init() {
	ia_ = DrawMethods::SkyBox({1.0f, 1.0f, 1.0f}); //!< test value

	transform_.Init();
	qt_.Init();

	material_ = std::make_unique<BufferResource<Material>>(MyEngine::GetDevicesObj(), 1);
	(*material_)[0].Init();

	SetAttributeName("Box");

	Collider::SetColliderBoundingAABB();

	Collider::typeId_ = 1;
	Collider::targetTypeId_ = 1;

	qt_.transform.rotate = MakeRotateAxisAngleQuaternion(v, angle);
}

void DebugObjects::Box::Term() {
}

void DebugObjects::Box::Update() {
	transform_.UpdateMatrix();
	qt_.UpdateMatrix();
}

void DebugObjects::Box::Draw() {

	auto commandList = MyEngine::GetCommandList();

	ia_.SetBuffer(commandList);

	commandList->SetGraphicsRootConstantBufferView(0, MyEngine::camera3D->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, qt_.GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(2, material_->GetGPUVirtualAddress());

	ia_.DrawCall(commandList);
}

void DebugObjects::Box::SetAttributeImGui() {
	if (ImGui::TreeNode("transform")) {
		/*transform_.transform.SetImGuiCommand();*/

		ImGui::DragFloat3("transform", &qt_.transform.translate.x, 0.01f);
		ImGui::DragFloat3("direction", &v.x, 0.01f);
		v = Normalize(v);

		ImGui::SliderAngle("angle", &angle);

		qt_.transform.rotate = MakeRotateAxisAngleQuaternion(v, angle);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("collider")) {
		SetColliderImGuiCommand();
		ImGui::TreePop();
	}

	if (ImGui::Button("delete")) {
		isDelete_ = true;
	}
}

void DebugObjects::Box::OnCollisionEnter(_MAYBE_UNUSED Collider* const other) {
	(*material_)[0].color = { 1.0f, 0.0f, 0.0f, 0.5f };

	std::string log = std::format("Box OnCollision Enter ptr : {}", reinterpret_cast<void*>(this));
	console->Log(log.c_str());
}

void DebugObjects::Box::OnCollisionExit(_MAYBE_UNUSED Collider* const other) {
	(*material_)[0].color = { 1.0f, 1.0f, 1.0f, 0.5f };

	std::string log = std::format("Box OnCollision Exit ptr: {}", reinterpret_cast<void*>(this));
	console->Log(log.c_str());
}
