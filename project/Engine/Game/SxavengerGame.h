#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* origin
#include <Engine/Game/Model.h>
#include <Engine/Game/DebugPrimitive.h>
#include <Engine/Game/Collider/ColliderManager.h>
#include <Engine/Game/Sprite.h>
#include <Engine/Game/Animation.h>

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

	static void Reset();

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

	static void ResetPrimitive();

	static void DrawLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color);

	static void DrawAxis(const Vector3f& center, float length);

	static DebugPrimitive* GetDebugPrimitive();

	//-----------------------------------------------------------------------------------------
	// collider manager option
	//-----------------------------------------------------------------------------------------

	static void SetCollider(Collider* collider);

	static void EraseCollider(Collider* collider);

	static void ClearColliders();

	static void UpdateColliders();

	static void DrawColliders(const Color4f& color = ToColor4f(0xFAFA00FF));

	static ColliderManager* GetColliderManager();

	//-----------------------------------------------------------------------------------------
	// sprite common option
	//-----------------------------------------------------------------------------------------

	static void DrawSprite(
		const Vector2f& pos, const Vector2f& size, const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Color4f& color = { 1.0f, 1.0f, 1.0f, 1.0f }
	);

	static void DrawSprite(
		const Vector2f& pos, const Vector2f& size, float rotate, const D3D12_GPU_DESCRIPTOR_HANDLE& handle, const Color4f& color = { 1.0f, 1.0f, 1.0f, 1.0f }
	);

	static SpriteCommon* GetSpriteCommon();

	//-----------------------------------------------------------------------------------------
	// skinning pipeline option
	//-----------------------------------------------------------------------------------------

	static void SetSkinningPipeline();

	static SkinningPipeline* GetSkinningPipeline();

private:
};

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////

using SxavengerGame = SxavengerGameEngine;