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
#include <DxCSBlob.h>

#include <Logger.h>

//=========================================================================================
// anonymous variables
//=========================================================================================

static const wchar_t* shaderTypeStr[kCountOfShaderType] = {
	L"vs_6_5", // vs
	L"ps_6_5", // pixel
	L"ms_6_5", // ms

	L"cs_6_5", // cs
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
	CSBlob::SetShaderTable(this);
}

void DxObject::ShaderTable::Term() {
	shaderBlobs_.clear();
}

IDxcBlob* DxObject::ShaderTable::GetShaderBlob(const std::wstring& filePath, ShaderType type) {
	// mapにshaderDataがあるか確認
	if (shaderBlobs_.find(filePath) != shaderBlobs_.end()) {
		return shaderBlobs_.at(filePath).Get();
	
	} else {
		CreateShaderBlob(filePath, type);
		return shaderBlobs_.at(filePath).Get();
	}
}

void DxObject::ShaderTable::CreateShaderBlob(const std::wstring& filePath, ShaderType type) {

	ComPtr<IDxcBlob> blob;

	// blobの生成
	blob = DxObjectMethod::CompileShader(
		directory_ + filePath,
		shaderTypeStr[type],
		compilers_->GetDxcUtils(), compilers_->GetDxcCompilder(), compilers_->GetIncluderHandler()
	);

	assert(blob != nullptr);

	// mapにblobを追加
	shaderBlobs_[filePath] = blob;

	// logの出力
	std::wstring logstr = L"[DxObject::ShaderTable]: " + filePath + L" << Complete Create";
	Log(logstr);

}