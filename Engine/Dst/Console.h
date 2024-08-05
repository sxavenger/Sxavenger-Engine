#pragma once

//*****************************************************************************************
// todo: imguiのスタイル変更, gameSceneのwindowの追加
//*****************************************************************************************

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// D3D12
#include <d3d12.h>

// c++
#include <deque>
#include <string>
#include <list>
#include <memory>
#include <unordered_map>

// geometry
#include <Vector4.h>

#include <Camera3D.h>
#include <DebugCamera3D.h>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Attribute;
class DirectXCommon;
class Texture;

////////////////////////////////////////////////////////////////////////////////////////////
// Console class
////////////////////////////////////////////////////////////////////////////////////////////
class Console {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static Console* GetInstance();

	//! @brief 初期化処理
	void Init();

	//! @brief 終了処理
	void Term();

	//! @brief 更新処理
	void Update();

	//! @brief ログをセット
	//! 
	//! @param[in] log   出力したいログ
	//! @param[in] color 文字の色
	void Log(const std::string& log, const Color4f& color = {1.0f, 1.0f, 1.0f, 1.0f});

	void SetAttribute(Attribute* obj) {
		attributes_.push_back(obj);
	}

	Texture* GetSceneTexture() const { return sceneTexture_; }

	Camera3D* GetDebugCamera() const { return debugCamera_.get(); }

	void CheckEraseAttribute(Attribute* obj);

	bool IsUpdateRequired() const { return isUpdateRequired_; }

	//
	// test function
	//

	void OutputTexture(const std::string& name, const D3D12_GPU_DESCRIPTOR_HANDLE& srvHandleGPU);

	void OutputDefferedTextures(const std::string& name, uint32_t indexSize, const D3D12_GPU_DESCRIPTOR_HANDLE textureHandles[8]);

	//=========================================================================================
	// public variables
	//=========================================================================================

	static const Color4f commentOutColor;
	static const Color4f errorColor;
	static const Color4f warningColor;
	static const Color4f defaultColor;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// LogData structture
	////////////////////////////////////////////////////////////////////////////////////////////
	struct LogData {
		std::string log;
		Color4f color;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	DirectXCommon* dxCommon_;

	// settings
	bool isOutputConsole_ = true;
	bool isFix_ = true;

	ImGuiWindowFlags windowFlags_ = 0;

	// scenes
	bool isFocusDebugScene_ = false;
	bool isFocusGameScene_  = false;

	std::unique_ptr<DebugCamera3D> debugCamera_;
	Texture* sceneTexture_;

	// logs
	static const int32_t kMaxLogData_ = 30;
	std::deque<LogData> logDatas_;

	// Attribute
	std::list<Attribute*> attributes_;
	Attribute* selectedAttribute_;

	//* process *//

	bool isUpdateRequired_ = true;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void OutputMenu();

	void OutputScene();
	void OutputGame();

	void OutputLog();

	void OutputPerformance();

	void OutputOutliner();

	void OutputSystem();

	void OutputAssets();

	void SetTextureImGui(const D3D12_GPU_DESCRIPTOR_HANDLE& texture);

	// methods 

	void OutlinerAttribute(Attribute* attribute);

};

static Console* console = Console::GetInstance();