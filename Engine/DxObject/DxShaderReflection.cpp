#include "DxShaderReflection.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <DxShaderManager.h>

#include "MyEngine.h" //!< hack include

////////////////////////////////////////////////////////////////////////////////////////////
// VoidBufferResource class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::VoidBufferResouce::Init(uint32_t indexSize, uint32_t bufferSize) {

	indexSize_ = indexSize;
	
	// deviceの取り出し
	auto device = MyEngine::GetDevice();

	// 配列分のBufferResourceを生成
	resource_ = DxObjectMethod::CreateBufferResource(
		device,
		indexSize * bufferSize
	);

	// resourceをマッピング
	resource_->Map(0, nullptr, map_);
}

void DxObject::VoidBufferResouce::Term() {
}

//=========================================================================================
// static variables
//=========================================================================================

DxObject::ShaderManager* DxObject::ShaderReflection::manager_;

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderReflection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::ShaderReflection::Init(IDxcBlob* blob) { // TODO

	DxcBuffer buffer = {
		.Ptr  = blob->GetBufferPointer(),
		.Size = blob->GetBufferSize()
	};

	auto hr = manager_->GetUtils()->CreateReflection(
		&buffer,
		IID_PPV_ARGS(&reflection_)
	);

	assert(SUCCEEDED(hr));

	D3D12_SHADER_DESC desc;
	reflection_->GetDesc(&desc);

	std::vector<D3D12_SIGNATURE_PARAMETER_DESC> paramDescs;

	for (UINT i = 0; i < desc.InputParameters; ++i) {
		D3D12_SIGNATURE_PARAMETER_DESC paramDesc;
		reflection_->GetInputParameterDesc(i, &paramDesc);

		paramDescs.push_back(paramDesc);
	}

	std::vector<D3D12_SHADER_BUFFER_DESC> bufferDescs;
	std::vector<D3D12_SHADER_VARIABLE_DESC> variableDescs;
	std::vector<D3D12_SHADER_TYPE_DESC> typeDescs;
	std::vector<D3D12_SHADER_TYPE_DESC> menberTypeDescs;


	for (UINT i = 0; i < desc.ConstantBuffers; ++i) {
		ID3D12ShaderReflectionConstantBuffer* constantBuffer = reflection_->GetConstantBufferByIndex(i);
		D3D12_SHADER_BUFFER_DESC bufferDesc;
		constantBuffer->GetDesc(&bufferDesc);

		bufferDescs.push_back(bufferDesc);

		for (UINT j = 0; j < bufferDesc.Variables; ++j) {

			ID3D12ShaderReflectionVariable* variable = constantBuffer->GetVariableByIndex(j);
			D3D12_SHADER_VARIABLE_DESC variableDesc;
			variable->GetDesc(&variableDesc);

			variableDescs.push_back(variableDesc);

			ID3D12ShaderReflectionType* type = variable->GetType();
			D3D12_SHADER_TYPE_DESC typeDesc;
			type->GetDesc(&typeDesc);

			typeDescs.push_back(typeDesc);

			for (UINT k = 0; k < typeDesc.Members; ++k) {
				ID3D12ShaderReflectionType* menberType = type->GetMemberTypeByIndex(k);
				D3D12_SHADER_TYPE_DESC memberType;
				menberType->GetDesc(&memberType);

				menberTypeDescs.push_back(memberType);
			}
		}
	}

}

void DxObject::ShaderReflection::Term() {
}