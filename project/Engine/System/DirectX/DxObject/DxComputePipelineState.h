#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxCommandContext.h"
#include "DxBlendState.h"
#include "DxShaderBlob.h"
#include "DxRootSignatureDesc.h"
#include "DxBindBuffer.h"

//* lib
#include <Lib/Geometry/Vector3.h>

//* c++
#include <filesystem>
#include <optional>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ComputePipelineState class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief DirectXのComputePipelineを作成, 設定するクラス
class ComputePipelineState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ComputePipelineState() = default;
	~ComputePipelineState() { Term(); }

	void Term();

	//* create methods *//

	void SetBlob(const ShaderBlob& blob);
	void CreateBlob(const std::filesystem::path& filepath);

	void CreatePipeline(Device* device, ComputeRootSignatureDesc&& desc); //!< rootSignatureDescはmove前提

	//* setting pipeline *//

	void SetPipeline(CommandContext* context) const;

	void Dispatch(CommandContext* context, const Vector3ui& threadGroupCount) const;

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::optional<ShaderBlob> blob_;

	//* rootSignature *//

	ComputeRootSignatureDesc    rootSignatureDesc_;
	ComPtr<ID3D12RootSignature> rootSignature_;

	//* pipeline *//

	ComPtr<ID3D12PipelineState> pipeline_;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* option *//

	D3D12_SHADER_BYTECODE GetBytecode();

	//* methods *//

	void CreateDirectXRootSignature(Device* device);

	void CreateDirectXPipeline(Device* device);

};

////////////////////////////////////////////////////////////////////////////////////////////
// ReflectionComputePipelineState class
////////////////////////////////////////////////////////////////////////////////////////////
class ReflectionComputePipelineState
	: public ComputePipelineState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ReflectionComputePipelineState()  = default;
	~ReflectionComputePipelineState() = default;

	//* reflection methods *//

	void ReflectionPipeline(Device* device);

	void ReflectionPipeline(Device* device, const SamplerBindDesc& desc);

	void BindComputeBuffer(CommandContext* context, const BindBufferDesc& desc);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	BindBufferTable table_;

	//=========================================================================================
	// private variables
	//=========================================================================================

	void SetBlobToTable();
};


_DXOBJECT_NAMESPACE_END
