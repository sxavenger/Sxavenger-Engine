#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxObject
#include <DxShaderBlob.h>
#include <DxShaderReflection.h>
#include <DxRootSignatureDesc.h>
#include <DxGraphicsPipeline.h>

//* c++
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsRender class
////////////////////////////////////////////////////////////////////////////////////////////
class GraphicsRender {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GraphicsRender() = default;
	~GraphicsRender() = default;

	void Init();

	//* create *//

	void CreateBlob(const std::wstring& filename, DxObject::GraphicShaderType type);

	void CreateTable();

	void BindBuffer(const std::string& bufferName, const DxObject::ShaderReflectionTable::BindBuffer& buffer);

	void CreatePipeline(BlendMode mode = kBlendModeNormal);

	//* use *//

	void SetPipeline(ID3D12GraphicsCommandList* commandList);

	void BindGraphicsParameter(ID3D12GraphicsCommandList* commandList);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::unique_ptr<DxObject::GraphicsBlob>          blob_;
	std::unique_ptr<DxObject::ShaderReflectionTable> table_;
	std::unique_ptr<DxObject::GraphicsPipeline>      pipeline_;

};