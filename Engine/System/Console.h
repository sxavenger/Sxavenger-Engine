#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// DirectX12
#include <d3d12.h>

// c++
#include <list>
#include <memory>
#include <string>
#include <deque>
#include <optional>

// imgui
#include <imgui.h>

// camera
#include <Engine/Game/DebugCamera3D.h>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Attribute;

////////////////////////////////////////////////////////////////////////////////////////////
// Console class
////////////////////////////////////////////////////////////////////////////////////////////
class Console {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 初期化処理
	void Init();

	//! @brief 終了処理
	void Term();

	//! @brief 更新処理
	void Update();

	//* console item *//

	//! @brief 現在のwindowをconsoleにdockingする
	void DockingConsole() const;

	//* attribute methods *//
	
	void SetAttribute(Attribute* attribute);

	void CheckEraseAttribute(Attribute* obj);

	//* console methods *//

	void Log(const std::string& log, const Color4f& color = defaultColor);

	void BeginPrintf(); //!< TODO: 安全性
	void EndPrintf();

	void BreakPoint(const std::source_location& location = std::source_location::current());

	//* getter *//

	RenderTexture* GetSceneTexture() const { return sceneTexture_.get(); }

	Camera3D* GetDebugCamera() const { return debugCamera_.get(); }

	bool IsUpdateRequired() const { return isUpdateRequired_; }

	//* singleton *//

	static Console* GetInstance();

	Console() = default;
	~Console() = default;

	// test //

	void DisplayTextureForConsole(const std::string& title, const D3D12_GPU_DESCRIPTOR_HANDLE handle);

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

	//* parameter *//

	static const std::string kConsoleName_;

	//* ImGui configs *//

	ImGuiID          dockingID_   = 0;
	ImGuiWindowFlags windowFlags_ = 0;

	//* attribute *//

	std::list<Attribute*> attributes_;
	Attribute*            selectedAttribute_ = nullptr;

	//* log *//

	const uint32_t      kMaxLog_ = 30;
	std::deque<LogData> logs_;

	//* process & config *//
	//* config
	bool isDisplayConsole_  = true;
	bool isOutputImGuiFile_ = false;

	//* process
	bool isUpdateRequired_ = true; //!< 最終的なプロセス管理

	//! [optional]
	//! nullopt:  通常状態(制限なく更新処理をする)
	//! in_value: 何回更新処理をするか<frame>(0の場合は更新処理を止める)
	std::optional<uint32_t> updateLimit_ = std::nullopt;

	//* scene *//

	std::unique_ptr<RenderTexture> sceneTexture_;
	std::unique_ptr<DebugCamera3D> debugCamera_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* display console methods *//

	void DisplayMenu();

	void DisplayScene();
	void DisplayGame();

	void DisplayOutliner();
	void DisplayAttribute();

	void DisplayLogs();

	void DisplaySystem();
	void DisplayPerformance();

	//* methods *//

	void DisplayTextureImGuiWindow(const D3D12_GPU_DESCRIPTOR_HANDLE texture);
	void SelectableAttribute(Attribute* attribute);

};

static Console* const console = Console::GetInstance();