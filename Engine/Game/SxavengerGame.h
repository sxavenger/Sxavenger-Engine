#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* origin
#include "Model.h"
#include "DebugPrimitive.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SxavengerGame class
////////////////////////////////////////////////////////////////////////////////////////////
class SxavengerGameEngine {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief 初期化処理
	static void Init();

	//! @brief 終了処理
	static void Term();

	//-----------------------------------------------------------------------------------------
	// model option
	//-----------------------------------------------------------------------------------------

	static Model* LoadModel(const std::string& directoryPath, const std::string& filename, bool smooth = false);

	static void DeleteModel(const std::string& directoryPath, const std::string& filename);

	static ModelManager* GetModelManager();

	//-----------------------------------------------------------------------------------------
	// debug primitive option
	//-----------------------------------------------------------------------------------------

	static void DrawToScene(const Camera3D* camera);

	static void CountPrimitiveBufferOffset();

	static void ResetPrimitive();

	static void DrawLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color);

	static DebugPrimitive* GetDebugPrimitive();

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////

using SxavengerGame = SxavengerGameEngine;