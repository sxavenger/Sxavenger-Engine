#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* graphics
#include "Animation/SkinningCompute.h"
#include "DebugPrimitive/DebugPrimitive.h"
#include "TextureBuffer/OffscreenTextureCollection.h"

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// Graphics class
////////////////////////////////////////////////////////////////////////////////////////////
class Graphics {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void Init();

	static void Term();

	//-----------------------------------------------------------------------------------------
	// skinning compute option
	//-----------------------------------------------------------------------------------------

	//! @brief スキニングPipelineの設定
	static void SetSkinningPipeline(const DirectXQueueContext* context);

	//! @brief スキニングのDispatch
	static void DispatchSkinning(const DirectXQueueContext* context, const DxObject::BindBufferDesc& desc, uint32_t vertexSize);

	//-----------------------------------------------------------------------------------------
	// texture collection option
	//-----------------------------------------------------------------------------------------

	//! @brief Textureの登録
	//! @param[in] name 名前
	//! @param texture offscreen texture
	static void RegisterTexture(const std::string& name, std::unique_ptr<BaseOffscreenTexture>&& texture);

	//! @brief Textureの取得
	//! @param[in] name 名前
	//! @return Descriptor
	static const DxObject::Descriptor& GetDescriptorSRV(const std::string& name);

	//! @brief TextureのGPUハンドルの取得
	//! @param[in] name 名前
	//! @return Handle
	static const D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHandleSRV(const std::string& name);

	//-----------------------------------------------------------------------------------------
	// debug primitive option
	//-----------------------------------------------------------------------------------------

		//! @brief デバッグPrimitiveのリセット
	static void ResetPrimitive();

	//! @brief LinePrimitiveの追加
	//! @param[in] v1 座標
	//! @param[in] v2 座標
	//! @param[in] color 色
	//! @param[in] thickness 線の太さ
	static void PushLine(const Vector3f& v1, const Vector3f& v2, const Color4f& color, float thickness = 0.0f);

	//! @brief LinePrimitiveの追加(Overlay)
	//! @param[in] v1 座標
	//! @param[in] v2 座標
	//! @param[in] color 色
	//! @param[in] thickness 線の太さ
	static void PushLineOverlay(const Vector3f& v1, const Vector3f& v2, const Color4f& color, float thickness = 0.0f);

	//! @brief PointPrimitiveの追加
	//! @param[in] v 座標
	//! @param[in] color 色
	//! @param[in] thickness 点の大きさ
	static void PushPoint(const Vector3f& v, const Color4f& color, float thickness = 0.0f);

	//! @brief PointPrimitiveの追加(Overlay)
	//! @param[in] v 座標
	//! @param[in] color 色
	//! @param[in] thickness 点の大きさ
	static void PushPointOverlay(const Vector3f& v, const Color4f& color, float thickness = 0.0f);

	//! @brief LinePrimitiveの軸描画
	//! @param[in] center 中心座標
	//! @param[in] length 軸の長さ
	static void PushAxis(const Vector3f& center, float length);

	//! @brief LinePrimitiveのBox描画
	//! @param[in] min 最小座標
	//! @param[in] max 最大座標
	//! @param[in] color 色
	//! @param[in] thickness 線の太さ
	static void PushBox(const Vector3f& min, const Vector3f& max, const Color4f& color, float thickness = 0.0f);

	//! @brief LinePrimitiveのBox描画
	//! @param[in] position 位置座標
	//! @param[in] min 最小座標
	//! @param[in] max 最大座標
	//! @param[in] color 色
	//! @param[in] thickness 線の太さ
	static void PushBox(const Vector3f& position, const Vector3f& min, const Vector3f& max, const Color4f& color, float thickness = 0.0f);

	//! @brief LinePrimitiveのCube描画
	//! @param[in] center 中心座標
	//! @param[in] size サイズ
	//! @param[in] color 色
	static void PushSphere(const Vector3f& center, float radius, const Color4f& color);

	//! @brief LinePrimitiveのGrid描画
	static void PushGrid(const Matrix4x4& viewInv, const Matrix4x4& projInv, const Vector2f& size, float radius);

	//! @brief LinePrimitiveのCone描画
	//! @param[in] center 中心座標
	//! @param[in] direction 方向ベクトル
	//! @param[in] radius 底面の半径
	//! @param[in] angle 角度(ラジアン)
	//! @param[in] color 色
	static void PushCone(const Vector3f& center, const Vector3f& direction, float radius, float angle, const Color4f& color);

	static DebugPrimitive* GetDebugPrimitive() { return debugPrimitive_.get(); }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static inline std::unique_ptr<SkinningCompute> skinning_;
	static inline std::unique_ptr<DebugPrimitive> debugPrimitive_;
	static inline std::unique_ptr<OffscreenTextureCollection> collection_;

};

SXAVENGER_ENGINE_NAMESPACE_END
