#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT origin
#include "DxObject/DxObjectCommon.h"
#include "DxObject/DxCommandContext.h"

//* engine
#include <Engine/Foundation.h>

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// DirectXPixEvent class
////////////////////////////////////////////////////////////////////////////////////////////
class DirectXPixEvent {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	static void BeginEvent(ID3D12GraphicsCommandList* commandList, const std::wstring& name, uint8_t indent);

	static void EndEvent(ID3D12GraphicsCommandList* commandList);

	static void CaptureNextFrames(const std::filesystem::path& filepath, uint32_t frames);

};

SXAVENGER_ENGINE_NAMESPACE_END
