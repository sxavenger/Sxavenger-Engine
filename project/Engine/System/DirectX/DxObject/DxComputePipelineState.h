#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxDevice.h"
#include "DxCommandContext.h"
#include "DxCompileBlobCollection.h"
#include "DxRootSignatureDesc.h"
#include "DxBindBuffer.h"

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ComputePipelineState class
////////////////////////////////////////////////////////////////////////////////////////////
class ComputePipelineState {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ComputePipelineState()  = default;
	~ComputePipelineState() { Term(); }

	void Term();

	//* create methods *//

	void CreateBlob(const std::wstring& filename);

	void CreatePipeline(Device* device, const ComputeRootSignatureDesc& desc);

	//* update methods *//

	void ReloadShader();

	virtual void CheckAndUpdatePipeline();

	//* setting pipeline *//

	void SetPipeline(ID3D12GraphicsCommandList* commandList) const;
	void SetPipeline(CommandContext* context) const;

	void ReloadAndSetPipeline(ID3D12GraphicsCommandList* commandList);
	void ReloadAndSetPipeline(CommandContext* context);

	void Dispatch(ID3D12GraphicsCommandList* commandList, UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ) const;
	void Dispatch(CommandContext* context, UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ) const;

	//* external methods *//

	static void SetExternal(CompileBlobCollection* collection);

protected:

	////////////////////////////////////////////////////////////////////////////////////////////
	// ComputeBlob structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ComputeBlob {
		std::optional<std::weak_ptr<ComPtr<IDxcBlob>>> blob;
		std::wstring                                   filename;
	};

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* external *//

	static CompileBlobCollection* collection_;
	Device* device_ = nullptr;

	//* blob *//

	ComputeBlob blob_;

	//* rootSignature *//

	ComPtr<ID3D12RootSignature> rootSignature_;
	ComputeRootSignatureDesc    rootSignatureDesc_;

	//* pipeline *//

	ComPtr<ID3D12PipelineState> pipeline_;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void CreateRootSignature();
	void CreatePipeline();

	//* option *//

	IDxcBlob* GetBlob();
	D3D12_SHADER_BYTECODE GetBytecode();

	bool CheckShaderReloadStatus();

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

	void ReflectionRootSignature(Device* device);

	void BindComputeBuffer(CommandContext* context, const BindBufferDesc& desc);

	//* update methods *//

	void CheckAndUpdatePipeline() override;

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

