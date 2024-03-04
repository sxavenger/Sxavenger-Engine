#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Object.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sprite class
////////////////////////////////////////////////////////////////////////////////////////////
class Sprite
	: public Object {
public:

	//! @brief コンストラクタ
	Sprite() { Init(); }

	//! @brief デストラクタ
	~Sprite() { Term(); }

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

	std::unique_ptr<DxObject::BufferResource<VertexData>> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW                              vertexBufferView_;
	std::unique_ptr<DxObject::IndexBufferResource>        indexResource_;
	D3D12_INDEX_BUFFER_VIEW                               indexBufferView_;

};