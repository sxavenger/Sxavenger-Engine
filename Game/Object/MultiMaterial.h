#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Object.h>

#include <Model.h>

////////////////////////////////////////////////////////////////////////////////////////////
// MultiMaterial class
////////////////////////////////////////////////////////////////////////////////////////////
class MultiMaterial
	: public Object {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @breif コンストラクタ
	MultiMaterial() { Init(); }

	//! @brief デストラクタ
	~MultiMaterial() { Term(); }

	//! @brief 初期化処理
	void Init() override;

	//! @brief ImGui
	void EditorImGui(int identifier);

	//! @brief 描画処理
	//! 
	//! @param[in] commandList
	void Draw(ID3D12GraphicsCommandList* commandList, Directional* directionalLight) override;

	//! @brief 終了処理
	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Object_Ptr<Model> model_;

	// multimaterailなのでmaterialが2つ必要...
	static const uint32_t kMaterialNum_ = 2; // materail分用意...

	// material
	Material materials_[kMaterialNum_];
	Transform uvTransforms_[kMaterialNum_];
	Matrix4x4 uvMatrixs_[kMaterialNum_] = { Matrix4x4::MakeIdentity(), Matrix4x4::MakeIdentity() };

	bool isEnableBlinnPhongs_[kMaterialNum_];

	// resource
	Object_Ptr<DxObject::BufferPtrResource<Material>> materialResources_[kMaterialNum_];

};
