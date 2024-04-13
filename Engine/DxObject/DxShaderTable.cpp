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
	L"vs_6_0", L"gs_6_0", L"ps_6_0"
};

static D3D12_SHADER_VISIBILITY shaderTypeVisibility[kCountOfShaderType] = {
	D3D12_SHADER_VISIBILITY_VERTEX,
	D3D12_SHADER_VISIBILITY_GEOMETRY,
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
	std::wstring logstr = L"[DxObject::ShaderTable]: " + filePath + L" << Complete Create \n";
	Log(logstr);

}

////////////////////////////////////////////////////////////////////////////////////////////
// namespace - anonymous
////////////////////////////////////////////////////////////////////////////////////////////
namespace {

	//=========================================================================================
	// RegisterData structure
	//=========================================================================================
	struct RegisterData {
		std::string structName;
		int registerValue;
	};

	//=========================================================================================
	// anonymous methods
	//=========================================================================================

	bool SearchTargetStartString(const std::string& target, const std::string& str) {
		if (target.size() > str.size()) {
			return false;
		}

		std::string begin = str.substr(0, target.size());

		if (begin.find(target) != std::string::npos) {
			return true;
		}

		return false;
	}

	std::string GetStrBetween(const std::string& str, const char first, const char last) {
		assert(first != last); //!< asciiで違いがない場合, find(..)で同じ場所をさしてしまうため.

		std::string result;

		auto begin = str.find(first) + 1;
		result = str.substr(begin, str.find(last) - begin);

		return result;
	}

	RegisterData GetRegisterData(const std::string& line) {

		RegisterData result;

		// ConstantBufferのstructNameの取り出し
		result.structName = GetStrBetween(line, '<', '>');

		// register番号の取り出し
		std::string registerInfo;

		registerInfo = GetStrBetween(line, '(', ')');
		registerInfo = registerInfo.substr(1); //!< register文字't'の破棄

		result.registerValue = std::stoi(registerInfo);

		return result;
	}

	const D3D12_DESCRIPTOR_RANGE* GetSampleRange(int baseRegister) {
		// rangeの設定
		D3D12_DESCRIPTOR_RANGE range = {};
		range.BaseShaderRegister                = baseRegister;
		range.NumDescriptors                    = 1;
		range.RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		return &range;
	}

}

//=========================================================================================
// static variables
//=========================================================================================
DxObject::Compilers* DxObject::ShaderTable_test::compilers_ = nullptr;
const std::wstring DxObject::ShaderTable_test::directory_ = L"./resources/hlsl/";

////////////////////////////////////////////////////////////////////////////////////////////
// class ShaderTable_test methods
////////////////////////////////////////////////////////////////////////////////////////////

void DxObject::ShaderTable_test::Init() {
}

void DxObject::ShaderTable_test::Term() {
	shaderDatas_.clear();
}

void DxObject::ShaderTable_test::CreateShaderElement(const std::wstring& key, ShaderType type) {

	ShaderElement element;

	// shaderBlobの生成
	element.blob = DxObjectMethod::CompileShader(
		directory_ + key, shaderTypeStr[type],
		compilers_->GetDxcUtils(), compilers_->GetDxcCompilder(), compilers_->GetIncluderHandler()
	);

	assert(element.blob != nullptr);

	element.descs = GetRootDescElementOfShader(directory_ + key, type);
	
	// mapにelementを構築
	shaderDatas_[key] = element;

	std::wstring logstr = L"[DxObject::ShaderTable_test]: " + key + L" << Complete Create \n";
	Log(logstr);
}

DxObject::RootDescElement DxObject::ShaderTable_test::GetRootDescElementOfShader(const std::wstring& filePath, ShaderType type) {
	return GetRootDescElementOfShader(filePath, shaderTypeVisibility[type]);
}

DxObject::RootDescElement DxObject::ShaderTable_test::GetRootDescElementOfShader(const std::wstring& filePath, D3D12_SHADER_VISIBILITY visibility) {
	// todo: samplerとrangeがサンプル
	RootDescElement result;

	std::string line;

	std::ifstream ifs(filePath);
	assert(ifs.is_open());

	while (std::getline(ifs, line)) {

		std::string identifire;
		std::istringstream s(line);
		s >> identifire;

		if (identifire.empty()) {
			continue;
		}

		if (identifire == "#include") {
			// include先のfilePathの取得
			std::string includeStr;
			s >> includeStr;

			std::string includeFilePath;
			includeFilePath = includeStr.substr(1, includeStr.size() - 2);

			// include先のdescとの統合
			RootDescElement includeDescs = GetRootDescElementOfShader(directory_ + ToWstring(includeFilePath), D3D12_SHADER_VISIBILITY_ALL);
			result.Intergrate(includeDescs);

		} else if (SearchTargetStartString("ConstantBuffer", identifire)) {

			RegisterData data = GetRegisterData(line);

			// paramの生成
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
			param.ShaderVisibility          = visibility;
			param.Descriptor.ShaderRegister = data.registerValue;

			result.param.push_back(param);

		} else if (SearchTargetStartString("StructuredBuffer", identifire)) {

			RegisterData data = GetRegisterData(line);

			// sampleRange
			// rangeの設定
			D3D12_DESCRIPTOR_RANGE sampleRange[1] = {};
			sampleRange[0].BaseShaderRegister                = data.registerValue;
			sampleRange[0].NumDescriptors                    = 1;
			sampleRange[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			sampleRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			// paramの生成
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility                    = visibility;
			param.DescriptorTable.pDescriptorRanges   = sampleRange;
			param.DescriptorTable.NumDescriptorRanges = _countof(sampleRange);

			result.param.push_back(param);

		} else if (SearchTargetStartString("Texture2D", identifire)) {

			RegisterData data = GetRegisterData(line);
			data.structName = "Texture2D"; // float4がstructNameになってしまうので書き換え

			// sampleRange
			// rangeの設定
			D3D12_DESCRIPTOR_RANGE sampleRange[1] = {};
			sampleRange[0].BaseShaderRegister                = data.registerValue;
			sampleRange[0].NumDescriptors                    = 1;
			sampleRange[0].RangeType                         = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			sampleRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			// paramの生成
			D3D12_ROOT_PARAMETER param = {};
			param.ParameterType                       = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			param.ShaderVisibility                    = visibility;
			param.DescriptorTable.pDescriptorRanges   = sampleRange;
			param.DescriptorTable.NumDescriptorRanges = _countof(sampleRange);

			result.param.push_back(param);

		} else if (identifire == "SamplerState") {

			RegisterData data = GetRegisterData(line);

			// samplerの設定
			D3D12_STATIC_SAMPLER_DESC sampler = {};
			sampler.Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			sampler.AddressU         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler.AddressV         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler.AddressW         = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			sampler.ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
			sampler.MaxLOD           = D3D12_FLOAT32_MAX;
			sampler.ShaderRegister   = data.registerValue;
			sampler.ShaderVisibility = visibility;

			result.sampler.push_back(sampler);

		}

	}

	ifs.close();

	return result;

}
