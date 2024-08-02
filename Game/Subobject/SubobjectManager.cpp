#include "SubobjectManager.h"

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxObject;

//=========================================================================================
// static variables
//=========================================================================================

const std::string SubobjectManager::kSubobjectNames_[kCountOfSubobjectType] = {
	"Sprite", "Plane", "Sphere", "Teapot", "Bunny", "MultiMesh", "MultiMaterial", "Suzanne"

	/*
	kSprite,
	kPlane,
	kSphere,
	kTeapot,
	kBunny,
	kMultiMesh,
	kMultiMaterial,
	kSuzanne,

	kCountOfSubobjectType
	*/
};

////////////////////////////////////////////////////////////////////////////////////////////
// SubobjectManager class methods
////////////////////////////////////////////////////////////////////////////////////////////

void SubobjectManager::Init(Camera3D* mainCamera) {

	CreatePipeline();


	mainCamera_ = mainCamera;

	light_ = std::make_unique<Light>(MyEngine::GetDevicesObj());

	SetThisAttribute("subobjectManager");

}

void SubobjectManager::Update() {
	// deleteされたsubobjectの削除
	for (auto it = subobjects_.begin(); it != subobjects_.end();) {

		if ((*it)->IsDelete()) {
			EraseAttributeNode(it->get());
			it = subobjects_.erase(it);
			continue;
		}

		it++;
	}

	// Attributeの更新処理
	for (auto& it : subobjects_) {
		it->Update();
	}
}

void SubobjectManager::Draw() {
	// Attributeの描画処理
	for (auto& it : subobjects_) {
		it->Draw(mainCamera_);
	}

	if (MyEngine::camera3D != mainCamera_) { //!< debugCameraかどうかの確認
		mainCamera_->DrawFrustum({1.0f, 1.0f, 0.0f, 1.0f});
	}
}

void SubobjectManager::SetAttributeImGui() {

	if (ImGui::BeginCombo("subobject", kSubobjectNames_[selectedSubobject_].c_str())) {
		for (int i = 0; i < SubobjectType::kCountOfSubobjectType; ++i) {
			bool isSelect = (i == selectedSubobject_);

			if (ImGui::Selectable(kSubobjectNames_[i].c_str(), isSelect)) {
				selectedSubobject_ = static_cast<SubobjectType>(i);
			}
		}

		ImGui::EndCombo();
	}

	if (ImGui::Button("Create")) {
		switch (selectedSubobject_) {
			case kSprite:
				{
					auto newSprite = std::make_unique<Subobjects::SpriteSubobject>();
					newSprite->Init(this);

					newSprite->SetAttributeName(kSubobjectNames_[selectedSubobject_]);
					SetAttributeNode(newSprite.get());
					subobjects_.push_back(std::move(newSprite));
				}
				break;

			case kPlane:
				{
					auto newPlane = std::make_unique<Subobjects::Plane>();
					newPlane->Init(this);

					newPlane->SetAttributeName(kSubobjectNames_[selectedSubobject_]);
					SetAttributeNode(newPlane.get());
					subobjects_.push_back(std::move(newPlane));
				}
				break;

			case kSphere:
				{
					auto newSphere = std::make_unique<Subobjects::Sphere>();
					newSphere->Init(this);

					newSphere->SetAttributeName(kSubobjectNames_[selectedSubobject_]);
					SetAttributeNode(newSphere.get());
					subobjects_.push_back(std::move(newSphere));
				}
				break;

			case kTeapot:
				{
					auto newTeapot = std::make_unique<Subobjects::Teapot>();
					newTeapot->Init(this);

					newTeapot->SetAttributeName(kSubobjectNames_[selectedSubobject_]);
					SetAttributeNode(newTeapot.get());
					subobjects_.push_back(std::move(newTeapot));
				}
				break;

			case kBunny:
				{
					auto newBunny = std::make_unique<Subobjects::Bunny>();
					newBunny->Init(this);

					newBunny->SetAttributeName(kSubobjectNames_[selectedSubobject_]);
					SetAttributeNode(newBunny.get());
					subobjects_.push_back(std::move(newBunny));
				}
				break;

			case kMultiMesh:
				{
					auto newMultiMesh = std::make_unique<Subobjects::MultiMesh>();
					newMultiMesh->Init(this);

					newMultiMesh->SetAttributeName(kSubobjectNames_[selectedSubobject_]);
					SetAttributeNode(newMultiMesh.get());
					subobjects_.push_back(std::move(newMultiMesh));
				}
				break;

			case kMultiMaterial:
				{
					auto newMultiMaterial = std::make_unique<Subobjects::MultiMaterial>();
					newMultiMaterial->Init(this);

					newMultiMaterial->SetAttributeName(kSubobjectNames_[selectedSubobject_]);
					SetAttributeNode(newMultiMaterial.get());
					subobjects_.push_back(std::move(newMultiMaterial));
				}
				break;

			case kSuzanne:
				{
					auto newSuzanne = std::make_unique<Subobjects::Suzanne>();
					newSuzanne->Init(this);

					newSuzanne->SetAttributeName(kSubobjectNames_[selectedSubobject_]);
					SetAttributeNode(newSuzanne.get());
					subobjects_.push_back(std::move(newSuzanne));
				}
				break;

			default:
				assert(false);
				break;
		}
	}

	ImGui::Spacing();

	if (ImGui::Button("DeleteAll")) {
		for (auto& it : subobjects_) {
			it->SetIsDelete(true);
		}
	}

}

void SubobjectManager::CreatePipeline() {

	for (int i = 0; i < SubobjectPipeline::kCountOfSubobjectPipeline; ++i) {
		pipelines_[i].Create();
	}

	{
		pipelines_[SubobjectPipeline::kTextureMesh].blob->Create(L"Subobject/Subobject.AS.hlsl", GRAPHICS_AMPLIFICATION);
		pipelines_[SubobjectPipeline::kTextureMesh].blob->Create(L"Subobject/Subobject.MS.hlsl", GRAPHICS_MESH);
		pipelines_[SubobjectPipeline::kTextureMesh].blob->Create(L"Subobject/SubobjectTexture.PS.hlsl", GRAPHICS_PIXEL);
		
		GraphicsRootSignatureDesc desc;
		desc.Resize(12, 1);
		//* meshlet
		desc.SetVirtualSRV(0, VISIBILITY_ALL, 0); //!< vertices
		desc.SetVirtualSRV(1, VISIBILITY_ALL, 1); //!< uniqueVertexIndices
		desc.SetVirtualSRV(2, VISIBILITY_ALL, 2); //!< meshlets
		desc.SetVirtualSRV(3, VISIBILITY_ALL, 3); //!< primitiveIndices
		desc.SetVirtualSRV(4, VISIBILITY_ALL, 4); //!< cullData
		desc.SetCBV(5, VISIBILITY_ALL, 0); //!< meshInfo
		//* debug
		desc.SetCBV(6, VISIBILITY_ALL, 3); //!< cullingCamera

		//* parameter
		desc.SetCBV(7, VISIBILITY_ALL, 1); //!< camera
		desc.SetCBV(8, VISIBILITY_ALL, 2); //!< transfrom

		//* pixel
		desc.SetSRV(9, VISIBILITY_PIXEL, 5); //!< Texture
		desc.SetSampler(0, MODE_WRAP, VISIBILITY_PIXEL, 0);

		desc.SetCBV(10, VISIBILITY_PIXEL, 4); //!< material
		desc.SetCBV(11, VISIBILITY_PIXEL, 5); //!< light

		pipelines_[SubobjectPipeline::kTextureMesh].pipeline->CreateRootSignature(MyEngine::GetDevicesObj(), desc);
		pipelines_[SubobjectPipeline::kTextureMesh].CreatePipeline();
		

	}

	
	{
		pipelines_[SubobjectPipeline::kNotTextureMesh].blob->Create(L"Subobject/Subobject.AS.hlsl", GRAPHICS_AMPLIFICATION);
		pipelines_[SubobjectPipeline::kNotTextureMesh].blob->Create(L"Subobject/Subobject.MS.hlsl", GRAPHICS_MESH);
		pipelines_[SubobjectPipeline::kNotTextureMesh].blob->Create(L"Subobject/SubobjectPolygon.PS.hlsl", GRAPHICS_PIXEL);

		GraphicsRootSignatureDesc desc;
		desc.Resize(11, 0);
		//* meshlet
		desc.SetVirtualSRV(0, VISIBILITY_ALL, 0); //!< vertices
		desc.SetVirtualSRV(1, VISIBILITY_ALL, 1); //!< uniqueVertexIndices
		desc.SetVirtualSRV(2, VISIBILITY_ALL, 2); //!< meshlets
		desc.SetVirtualSRV(3, VISIBILITY_ALL, 3); //!< primitiveIndices
		desc.SetVirtualSRV(4, VISIBILITY_ALL, 4); //!< cullData
		desc.SetCBV(5, VISIBILITY_ALL, 0); //!< meshInfo
		//* debug
		desc.SetCBV(6, VISIBILITY_ALL, 3); //!< cullingCamera

		//* parameter
		desc.SetCBV(7, VISIBILITY_ALL, 1); //!< camera
		desc.SetCBV(8, VISIBILITY_ALL, 2); //!< transfrom

		//* pixel
		desc.SetCBV(9, VISIBILITY_PIXEL, 4); //!< material
		desc.SetCBV(10, VISIBILITY_PIXEL, 5); //!< light

		pipelines_[SubobjectPipeline::kNotTextureMesh].pipeline->CreateRootSignature(MyEngine::GetDevicesObj(), desc);
		pipelines_[SubobjectPipeline::kNotTextureMesh].CreatePipeline();

	}
	
}