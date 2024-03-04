#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include "MyEngine.h" //!< textureの取り出し

// c++
#include <memory>

// ObjectStructure
#include <ObjectStructure.h>

// DirectX12
#include <DxBufferResource.h>

// Lib
#include <DirectionalLight.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Object class
////////////////////////////////////////////////////////////////////////////////////////////
class Object {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	Object() { InitResource(); }

	//! @brief デストラクタ
	virtual ~Object() { TermResource(); };

	//! @brief 初期化処理
	virtual void Init() = 0;

	//! @brief ImGui
	virtual void EditorImGui(int identifier) = 0;

	//! @brief 描画処理
	virtual void Draw(ID3D12GraphicsCommandList* commandList, Directional* directionalLight) = 0;

	//! @brief 終了処理
	virtual void Term() = 0;

	bool GetIsDelete() const { return isDelete_; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	// parameter
	// matrix
	Transform transform_;
	Matrix4x4 worldMatrix_ = Matrix4x4::MakeIdentity();

	// material
	Material material_;
	Transform uvTransform_;
	Matrix4x4 uvMatrix_ = Matrix4x4::MakeIdentity();

	// resource
	std::unique_ptr<DxObject::BufferResource<TransformationMatrix>> matrixResource_;
	std::unique_ptr<DxObject::BufferPtrResource<Material>>          materialResource_;

	// bool
	bool isDelete_ = false;

	static const char* lambertItem_[kLambertTypeCount];

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void InitResource();

	void TermResource();

	void SetImGui(const std::string& title);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	bool isEnableBlinnPhong_ = false;

};