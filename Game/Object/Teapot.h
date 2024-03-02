#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <Object.h>

// Model
#include <Model.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Teapot class
////////////////////////////////////////////////////////////////////////////////////////////
class Teapot
	: public Object {
public:

	//! @brief コンストラクタ
	Teapot() { Init(); }

	//! @brief デストラクタ
	~Teapot() { Term(); }

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

};