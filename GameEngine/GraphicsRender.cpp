#include "GraphicsRender.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <MyEngine.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsRender class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GraphicsRender::Init() {
	blob_     = std::make_unique<GraphicsBlob>();
	table_    = std::make_unique<ShaderReflectionTable>();
	pipeline_ = std::make_unique<GraphicsPipeline>();
}

void GraphicsRender::CreateBlob(const std::wstring& filename, GraphicShaderType type) {
	blob_->Create(filename, type);
}

void GraphicsRender::CreateTable() {

	bool isUseMeshPipeline = blob_->IsUseMeshPipeline();

	const auto& blobs = blob_->GetGraphicsBlobs();

	// reflectionの生成
	if (isUseMeshPipeline) {
		// ms
		table_->Create(blobs[GraphicShaderType::GRAPHICS_MESH], VISIBILITY_MESH);

		// as
		if (blobs[GraphicShaderType::GRAPHICS_AMPLIFICATION] != nullptr) {
			table_->Create(blobs[GraphicShaderType::GRAPHICS_AMPLIFICATION], VISIBILITY_AMPLIFICATION);
		}

	} else {
		// vs
		table_->Create(blobs[GraphicShaderType::GRAPHICS_VERTEX], VISIBILITY_VERTEX);

		// gs
		if (blobs[GraphicShaderType::GRAPHICS_GEOMETRY] != nullptr) {
			table_->Create(blobs[GraphicShaderType::GRAPHICS_GEOMETRY], VISIBILITY_GEOMETRY);
		}
	}

	table_->Create(blobs[GraphicShaderType::GRAPHICS_PIXEL], VISIBILITY_PIXEL);

}

void GraphicsRender::BindBuffer(const std::string& bufferName, const ShaderReflectionTable::BindBuffer& buffer) {
	table_->Bind(bufferName, buffer);
}

void GraphicsRender::CreatePipeline(BlendMode mode) {

	pipeline_->CreateRootSignature(MyEngine::GetDevicesObj(), table_->CreateRootSignatureDesc());

	GraphicsPipelineDesc desc = {};
	desc.CreateDefaultDesc();

	if (!blob_->IsUseMeshPipeline()) { //!< vsを使う場合
		desc.elements.clear();
		desc.elements = table_->GetElement();
	}

	desc.blendMode = mode;

	pipeline_->CreatePipeline(MyEngine::GetDevicesObj(), blob_.get(), desc);
}

void GraphicsRender::SetPipeline(ID3D12GraphicsCommandList* commandList) {
	pipeline_->SetPipeline(commandList);
}

void GraphicsRender::BindGraphicsParameter(ID3D12GraphicsCommandList* commandList) {
	table_->BindGraphicsParameter(commandList);
}
