#include "DxShaderReflection.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <DxShaderBlobManager.h>
#include <DxShaderBlob.h>

//=========================================================================================
// static variables
//=========================================================================================

DxObject::ShaderBlobManager* DxObject::ShaderReflection::manager_ = nullptr;

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderReflection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::ShaderReflection::Init(IDxcBlob* blob) { // TODO

	CreateReflection(blob);
	Reflection();

}

void DxObject::ShaderReflection::Term() {
}

void DxObject::ShaderReflection::CreateReflection(IDxcBlob* blob) {

	DxcBuffer buffer = {
		.Ptr      = blob->GetBufferPointer(),
		.Size     = blob->GetBufferSize(),
		//!< encordingは設定しない
	};

	assert(manager_ != nullptr); //!< managerが設定されてない

	auto hr = manager_->GetUtils()->CreateReflection(
		&buffer,
		IID_PPV_ARGS(&reflection_)
	);

	assert(SUCCEEDED(hr));

}

void DxObject::ShaderReflection::Reflection() {

	D3D12_SHADER_DESC mainDesc = {}; //!< shaderの外見情報
	reflection_->GetDesc(&mainDesc);

	// 外部からBindされている情報の取得
	for (UINT i = 0; i < mainDesc.BoundResources; ++i) {
		D3D12_SHADER_INPUT_BIND_DESC bindDesc = {};

		reflection_->GetResourceBindingDesc(i, &bindDesc);

		// bind情報からrootSignature設定用のmapの生成
		bindBuffers_.emplace(bindDesc.Name, bindDesc.Type);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// ReflectionPipeline class
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::ReflectionPipeline::CreatePipeline(GraphicsBlob* graphicsBlob) {

	auto& blobs = graphicsBlob->GetGraphicsBlobs();

	if (graphicsBlob->IsUseMeshPipeline()) {
		assert(false); //!< TODO: mesh shader pipelineの実装

	} else {

		if (blobs[GRAPHICS_VERTEX] != nullptr) {
			auto reflection = std::make_unique<ShaderReflection>(blobs[GRAPHICS_VERTEX]);
			reflections_.emplace_back(std::move(reflection));
		}
		
		if (blobs[GRAPHICS_GEOMETRY] != nullptr) {
			auto reflection = std::make_unique<ShaderReflection>(blobs[GRAPHICS_GEOMETRY]);
			reflections_.emplace_back(std::move(reflection));
		}

		if (blobs[GRAPHICS_PIXEL] != nullptr) {
			auto reflection = std::make_unique<ShaderReflection>(blobs[GRAPHICS_PIXEL]);
			reflections_.emplace_back(std::move(reflection));
		}

	}

}
