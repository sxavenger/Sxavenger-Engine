#include "DxShaderTable.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <fstream>
#include <sstream>

// DxObject
#include <DxCompilers.h>
#include <DxShaderBlob.h>

#include <Logger.h>

//=========================================================================================
// anonymous variables
//=========================================================================================

static const wchar_t* shaderTypeStr[kCountOfShaderType] = {
	L"ms_6_5", L"ps_6_5"
};

static D3D12_SHADER_VISIBILITY shaderTypeVisibility[kCountOfShaderType] = {
	D3D12_SHADER_VISIBILITY_MESH,
	D3D12_SHADER_VISIBILITY_PIXEL
};

//=========================================================================================
// static variables
//=========================================================================================
DxObject::Compilers* DxObject::ShaderTable::compilers_ = nullptr;
const std::wstring DxObject::ShaderTable::directory_ = L"./resources/hlsl/";

////////////////////////////////////////////////////////////////////////////////////////////
// class ShaderTable methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::ShaderTable::Init() {
	ShaderBlob::SetShaderTable(this);
}

void DxObject::ShaderTable::Term() {
	shaderBlobs_.clear();
}

void DxObject::ShaderTable::CreateShaderBlob(const std::wstring& filePath, ShaderType type) {

	ComPtr<IDxcBlob> blob;

	// blobの生成
	blob = DxObjectMethod::CompileShader(
		directory_ + filePath, shaderTypeStr[type],
		compilers_->GetDxcUtils(), compilers_->GetDxcCompilder(), compilers_->GetIncluderHandler()
	);

	assert(blob != nullptr);

	// mapにblobを追加
	shaderBlobs_[filePath] = blob;

	// logの出力
	std::wstring logstr = L"[DxObject::ShaderTable]: " + filePath + L" << Complete Create";
	Log(logstr);

}