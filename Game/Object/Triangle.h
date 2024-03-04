#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Object.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Triangle class
////////////////////////////////////////////////////////////////////////////////////////////
class Triangle
	: public Object {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	Triangle() { Init(); }

	//! @brief デストラクタ
	~Triangle() { Term(); }

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

	static const uint32_t kVertexNum_ = 3;

	// resource
	std::unique_ptr<DxObject::BufferResource<VertexData>> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW                              vertexBufferView_;

};