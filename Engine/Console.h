#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <deque>
#include <string>
#include <list>

// geometry
#include <Vector4.h>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Hierarchy;

////////////////////////////////////////////////////////////////////////////////////////////
// Console class
////////////////////////////////////////////////////////////////////////////////////////////
class Console {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static Console* GetInstance();

	Console() {}

	~Console() { Term(); }

	//! @brief 初期化処理
	void Init();

	//! @brief 更新処理
	void Update();

	//! @brief ログをセット
	//! 
	//! @param[in] log   出力したいログ
	//! @param[in] color 文字の色
	void SetLog(const std::string& log, const Vector4f& color = {1.0f, 1.0f, 1.0f, 1.0f});

	void SetHierarchy(Hierarchy* obj) {
		hierarchys_.push_back(obj);
	}

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// LogData structture
	////////////////////////////////////////////////////////////////////////////////////////////
	struct LogData {
		std::string log;
		Vector4f color;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	// scenes
	bool isOpenDebugScene_ = true;
	bool isOpenGameScene_  = true;

	// logs
	static const int32_t kMaxLogData_ = 10;
	std::deque<LogData> logDatas_;

	// hierarchys
	std::list<Hierarchy*> hierarchys_;
	Hierarchy* selectedHierarchy_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//! @brief 終了処理
	void Term();

	void OutputConsole();
	void OutputLog();
	void OutputSpeed();
	void OutputHierarchy();

};