#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Console/RenderingConsole.h>
#include <Engine/Console/ProcessConsole.h>
#include <Engine/Game/Camera/CineCamera.h>
#include <Engine/Game/Camera/DebugCamera3D.h>
#include <Engine/Game/SxavengerPipeline/SxavengerFrame.h>

//* lib
#include <Lib/Geometry/Vector4.h>

//* external
#include <imgui.h>

//* c++
#include <optional>
#include <string>
#include <deque>

////////////////////////////////////////////////////////////////////////////////////////////
// SystemConsole class
////////////////////////////////////////////////////////////////////////////////////////////
class SystemConsole final
	: public RenderingConsole, public ProcessConsole {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init();

	void Term();

	void UpdateConsole();

	void Draw();

	//* console docking *//

	void DockingConsole() const;

	//* console option *//

	void Log(const std::string& log, const std::optional<Color4f>& color = std::nullopt);

	void PresentToScreen();

	//* config getter *//

	const ImGuiWindowFlags GetWindowFlag() const { return windowFlag_; } //!< HACK

	bool IsUpdateRequired() const { return isUpdateRequired_; }

	CineCamera* GetGameCamera() const { return gameCamera_.get(); }

	//!< singleton
	// TODO: SxavengerEngineの関数として入れてもいいかも
	static SystemConsole* const GetInstance();
	SystemConsole()  = default;
	~SystemConsole() = default;

	//=========================================================================================
	// public variables
	//=========================================================================================

	static const Color4f kCommentOutColor;
	static const Color4f kErrorColor;
	static const Color4f kWarningColor;

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// PresentToScreenType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class PresentToScreenType : uint32_t {
		kPresent_None,
		kPresent_Game,
		kPresent_Scene,
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* config *//
	//* console
	bool isDisplayConsole_           = true;
	PresentToScreenType presentType_ = PresentToScreenType::kPresent_Game;

	//* imgui
	ImGuiID dockingId_           = NULL;
	ImGuiWindowFlags windowFlag_ = NULL;

	//* update process *//

	bool isUpdateRequired_ = true; //!< 最終的なプロセス管理

	//! [optional]
	//! nullopt:   通常状態(制限なく更新処理をする)
	//! has_value: 何回更新処理をするか<frame>(0の場合は更新処理を止める)
	std::optional<uint32_t> updateLimit_ = std::nullopt;

	//* parameter *//

	static const std::string kConsoleName_;
	static const std::string kImGuiIniFilepath_;

	//* log *//

	//! [pair]
	//! first: ログ内容
	//!  second: [optional]
	//!  std::nullopt: 通常text
	//!  has_value:    colorText
	std::deque<std::pair<std::string, std::optional<Color4f>>> logs_;
	uint32_t limitLog_ = 32;

	//* sxavenger frame *//

	std::unique_ptr<SxavengerFrame> gameFrame_;
	std::unique_ptr<CineCamera> gameCamera_;

	std::unique_ptr<SxavengerFrame> sceneFrame_;
	std::unique_ptr<DebugCamera3D> sceneCamera_;

	//* checker board render target *//


	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitImGuiConfig();

	void InitConsole();
	void TermConsole();

	void InitFrame();
	void TermFrame();

	void InitCheckerBoard();
	void TermCheckerBoard();
	void DrawCheckerBoard();

	void UpdateConsoleShortcut();

	//* display console methods *//

	void DisplayMainMenu();
	void DisplayPerformance();
	void DisplayLog();
	void DisplayGame();
	void DisplayScene();

	void DisplaySystemMenu();

	//* assistance methods *//

	void DisplayTextureImGuiFullWindow(const MultiViewTexture* texture) const;
	void DisplayTextureImGuiFullWindow(const BaseTexture* texture, const ImVec4& boarderColor = {}) const;
};

SystemConsole* const sSystemConsole = SystemConsole::GetInstance();