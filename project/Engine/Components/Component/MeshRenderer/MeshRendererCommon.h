#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Foundation.h>

//* lib
#include <Lib/Sxl/Flag.h>

//* c++
#include <cstdint>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// MeshInstanceMask enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class MeshInstanceMask : uint8_t {
	None       = 0,
	Shadow     = 1 << 0,

	Default    = 0xFF
};

////////////////////////////////////////////////////////////////////////////////////////////
// MeshRendererCommon class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 各種メッシュレンダラーcomponentが共有する描画設定/機能をまとめた基底クラス
class MeshRendererCommon {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Mode enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Mode : uint8_t {
		Opaque,
		Translucent,
		Emissive,
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	uint8_t GetMask() const { return mask_.Get(); }

	uint8_t GetStencil() const { return stencil_; }

	void SetEnable(bool isEnable = true) { isEnable_ = isEnable; }

	Mode GetMode() const { return mode_; }

	void SetMode(Mode mode) { mode_ = mode; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isEnable_ = true;

	//* parameter *//

	Sxl::Flag<MeshInstanceMask> mask_ = MeshInstanceMask::Default;

	uint8_t stencil_ = 0;

	Mode mode_ = Mode::Opaque;

};

SXAVENGER_ENGINE_NAMESPACE_END
