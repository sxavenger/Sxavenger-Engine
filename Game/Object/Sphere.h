#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Object.h>

// DrawMethod
#include <DrawMethod.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sphere class
////////////////////////////////////////////////////////////////////////////////////////////
class Sphere
	: public Object {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	Sphere() { Init(); }

	//! @brief デストラクタ
	~Sphere() { Term(); }

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

	Object_Ptr<DxObject::BufferResource<VertexData>> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW                         vertexBufferView_;
	Object_Ptr<DxObject::IndexBufferResource>        indexResource_;
	D3D12_INDEX_BUFFER_VIEW                          indexBufferView_;

};