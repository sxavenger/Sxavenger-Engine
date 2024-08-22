#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxObjectCommon
#include <DxObjectCommon.h>

//* DxObject
#include <DxShaderBlob.h>
#include <DxRootSignatureDesc.h>

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// CSPipeline class
////////////////////////////////////////////////////////////////////////////////////////////
class CSPipeline {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CSPipeline() = default;
	~CSPipeline() = default;

	//* create pipleine *//

	void CreatePipeline(const CSRootSignatureDesc& desc, CSBlob* csBlob);

	//* setter *//

	void SetPipeline();

	void Dispatch(UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* ID3D12 *//

	ComPtr<ID3D12RootSignature> rootSignature_;
	ComPtr<ID3D12PipelineState> pipeline_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreatePipeline(CSBlob* csBlob);

};

_DXOBJECT_NAMESPACE_END