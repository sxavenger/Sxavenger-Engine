#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <string>
#include <list>
#include <optional>
#include <format>
#include <memory>
#include <deque>

//* imgui
#include <imgui.h>

//* DxObject
#include <Engine/System/DxObject/DxShaderBlob.h>
#include <Engine/System/DxObject/DxGraphicsPipeline.h>

//* engine
#include <Engine/System/Texture.h>
#include <Engine/Game/Camera3D.h>
#include <Engine/Game/DebugCamera3D.h>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class MonoBehavior;

////////////////////////////////////////////////////////////////////////////////////////////
// BetaConsolePipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class BetaConsolePipeline {
public:

	enum PipelineType {
		kConsoleLocal,
		kDefault_Diffuse,

		kCountOfPipelineType
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void SetPipeline(PipelineType type);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::GraphicsBlob>     blobs_[kCountOfPipelineType];
	std::unique_ptr<DxObject::GraphicsPipeline> pipelines_[kCountOfPipelineType];

	//=========================================================================================
	// private variables
	//=========================================================================================

};

////////////////////////////////////////////////////////////////////////////////////////////
// BetaConsole class
////////////////////////////////////////////////////////////////////////////////////////////
class BetaConsole final {
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

	//! @brief 描画処理
	void Draw();

	//* console docking *//

	//! @brief 現在のwindowをconsoleにdockingする
	void DockingConsole() const;

	//* MonoBehavior option *//

	void SetMonoBehavior(MonoBehavior* behavior);

	void RemoveMonoBehavior(MonoBehavior* behavior);

	//* console option *//

	void Log(const std::string& log, const Color4f& color = kDefaultColor);

	//* console accsessor *//

	bool IsUpdateRequired() const { return isUpdateRequired_; }

	RenderTexture* GetLocalRenderTexture() const { return localRenderTexture_.get(); }

	//* singleton *//

	static BetaConsole* GetInstance();
	BetaConsole()  = default;
	~BetaConsole() = default;

	//=========================================================================================
	// public variables
	//=========================================================================================

	static const Color4f kCommentOutColor;
	static const Color4f kErrorColor;
	static const Color4f kWarningColor;
	static const Color4f kDefaultColor;


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

	//* MonoBehavior *//

	std::list<MonoBehavior*>                          behaviors_;
	std::optional<std::list<MonoBehavior*>::iterator> selectedBehavior_ = std::nullopt;

	//* config *//
	//* Console config
	bool isDisplayConsole_ = _DEBUG;
	bool isOutputImGuiIni_ = false;

	//* ImGui config
	ImGuiID          dockingID_   = 0;
	ImGuiWindowFlags windowFlags_ = 0;

	//* Rendering *//
	//* screen
	std::unique_ptr<RenderTexture> gameRenderTexture_;
	std::unique_ptr<Camera3D>      gameCamera_;

	std::unique_ptr<RenderTexture> sceneRenderTexture_;
	std::unique_ptr<DebugCamera3D> sceneCamera_;

	//* local
	std::unique_ptr<RenderTexture> localRenderTexture_;
	std::unique_ptr<DebugCamera3D> localCamera_;

	// TODO: cameraの実装

	BetaConsolePipeline pipeline_;

	//* log *//

	std::deque<LogData> logDatas_;
	const uint32_t      kMaxLog_ = 30;

	//* process *//

	bool isUpdateRequired_ = true; //!< 最終的なプロセス管理

	//! [optional]
	//! nullopt:   通常状態(制限なく更新処理をする)
	//! has_value: 何回更新処理をするか<frame>(0の場合は更新処理を止める)
	std::optional<uint32_t> updateLimit_ = std::nullopt;

	//* parameter *//

	static const std::string kConsoleName_;
	static const std::string kImGuiIniFilepath_;


	//=========================================================================================
	// private methods
	//=========================================================================================

	//* displays methods *//

	void DisplayMainMenu();

	void DisplayGame();
	void DisplayScene();

	void DisplayOutliner();  //!< selectable MonoBehavior
	void DisplayAttribute(); //!< selected MonoBehavior

	void DisplayLog();
	void DisplaySystem();

	void DisplayPerformance();

	//* console option methods *//

	void InitConfig();

	void InitRenderTarget();
	void TermRenderTarget();

	//* assistance methods *//

	void DisplayTextureImGuiFullWindow(const BaseTexture* texture);

	bool IsSelectedBehavior(MonoBehavior* behavior);
	void SelectableMonoBehavior(const std::list<MonoBehavior*>::iterator& behaviorIt);

};

static BetaConsole* const sBetaConsole = BetaConsole::GetInstance();