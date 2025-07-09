#include "DxBindBuffer.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// BindBufferDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void BindBufferDesc::Clear() {
	container_.clear();
}

void BindBufferDesc::SetAddress(const std::string& name, const D3D12_GPU_VIRTUAL_ADDRESS& address) {
	container_[name] = address;
}

void BindBufferDesc::SetHandle(const std::string& name, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {
	container_[name] = handle;
}

void BindBufferDesc::Set32bitConstants(const std::string& name, UINT num32bit, const void* data) {
	container_[name] = Constant32bits{ num32bit, data };
}

void BindBufferDesc::Merge(const BindBufferDesc& desc) {
	for (const auto& [name, buffer] : desc.container_) {
		container_[name] = buffer;
	}
}

bool BindBufferDesc::Contains(const std::string& name) const {
	return container_.contains(name);
}

const BindBufferDesc::Constant32bits& BindBufferDesc::Get32bitConstants(const std::string& name) const {
	Exception::Assert(std::holds_alternative<Constant32bits>(container_.at(name)), "type is not same.");
	return std::get<Constant32bits>(container_.at(name));
}

const D3D12_GPU_VIRTUAL_ADDRESS& BindBufferDesc::GetAddress(const std::string& name) const {
	Exception::Assert(std::holds_alternative<D3D12_GPU_VIRTUAL_ADDRESS>(container_.at(name)), "type is not same.");
	return std::get<D3D12_GPU_VIRTUAL_ADDRESS>(container_.at(name));
}

const D3D12_GPU_DESCRIPTOR_HANDLE& BindBufferDesc::GetHandle(const std::string& name) const {
	Exception::Assert(std::holds_alternative<D3D12_GPU_DESCRIPTOR_HANDLE>(container_.at(name)), "type is not same.");
	return std::get<D3D12_GPU_DESCRIPTOR_HANDLE>(container_.at(name));
}

////////////////////////////////////////////////////////////////////////////////////////////
// SamplerBindDesc structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void SamplerBindDesc::SetSamplerLinear(const std::string& name, SamplerMode mode) {
	samplers_[name].Filter           = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplers_[name].AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers_[name].AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers_[name].AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers_[name].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	samplers_[name].MaxLOD           = D3D12_FLOAT32_MAX;
}

void SamplerBindDesc::SetSamplerAnisotropic(const std::string& name, SamplerMode mode, uint32_t anisotropic) {
	samplers_[name].Filter           = D3D12_FILTER_ANISOTROPIC;
	samplers_[name].MaxAnisotropy    = anisotropic; //!< 異方性フィルタリングパラメーター
	samplers_[name].AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers_[name].AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers_[name].AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers_[name].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	samplers_[name].MaxLOD           = D3D12_FLOAT32_MAX;
}

void SamplerBindDesc::SetSamplerPoint(const std::string& name, SamplerMode mode) {
	samplers_[name].Filter           = D3D12_FILTER_MIN_MAG_MIP_POINT;
	samplers_[name].AddressU         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers_[name].AddressV         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers_[name].AddressW         = static_cast<D3D12_TEXTURE_ADDRESS_MODE>(mode);
	samplers_[name].ComparisonFunc   = D3D12_COMPARISON_FUNC_NEVER;
	samplers_[name].MaxLOD           = D3D12_FLOAT32_MAX;
}

void SamplerBindDesc::SetSamplerDesc(const std::string& name, const D3D12_STATIC_SAMPLER_DESC& desc) {
	samplers_[name] = desc;
}

bool SamplerBindDesc::Contains(const std::string& name) const {
	return samplers_.contains(name);
}

D3D12_STATIC_SAMPLER_DESC SamplerBindDesc::GetSampler(const std::string& name, ShaderVisibility stage, UINT shaderRegister, UINT registerSpace) const {
	Exception::Assert(Contains(name), "sampler is not found. sampler name: " + name);

	D3D12_STATIC_SAMPLER_DESC desc = samplers_.at(name);
	desc.ShaderRegister   = shaderRegister;
	desc.RegisterSpace    = registerSpace;
	desc.ShaderVisibility = static_cast<D3D12_SHADER_VISIBILITY>(stage);

	return desc;
}

////////////////////////////////////////////////////////////////////////////////////////////
// BindBufferInfo structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void BindBufferTable::BindBufferInfo::Create(const D3D12_SHADER_INPUT_BIND_DESC& _desc, ShaderVisibility _visibility) {
	rootParam      = std::nullopt;
	visibility     = _visibility;
	registerNum    = _desc.BindPoint;
	registerSpace  = _desc.Space;
	type           = _desc.Type;
	bindBufferType = ToBindBufferType(_desc.Type);

	// todo: 32bitconstantsの設定
}

////////////////////////////////////////////////////////////////////////////////////////////
// BindBufferTable class methods
////////////////////////////////////////////////////////////////////////////////////////////

void BindBufferTable::CreateTable(ID3D12ShaderReflection* reflection, ShaderVisibility visibility) {

	// shader全体の情報を取得
	D3D12_SHADER_DESC main = {};
	reflection->GetDesc(&main);

	for (UINT i = 0; i < main.BoundResources; ++i) {
		// bind情報
		D3D12_SHADER_INPUT_BIND_DESC desc = {};
		reflection->GetResourceBindingDesc(i, &desc);

		InsertBindBuffer(desc, visibility);
	}
}

GraphicsRootSignatureDesc BindBufferTable::CreateGraphicsRootSignatureDesc() {
	GraphicsRootSignatureDesc desc = {};

	UINT rootIndex = 0;

	for (auto& [name, info] : table_) {
		switch (info.bindBufferType) {
			case BindBufferType::kVirtual_CBV:
				desc.SetVirtualCBV(rootIndex, info.visibility, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kVirtual_SRV:
				desc.SetVirtualSRV(rootIndex, info.visibility, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kVirtual_UAV:
				desc.SetVirtualUAV(rootIndex, info.visibility, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kHandle_SRV:
				desc.SetHandleSRV(rootIndex, info.visibility, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kHandle_UAV:
				desc.SetHandleUAV(rootIndex, info.visibility, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kSampler:
				desc.SetSamplerLinear(MODE_WRAP, info.visibility, info.registerNum, info.registerSpace);
				continue;
				break;
		}

		info.rootParam = rootIndex;
		rootIndex++;
	}

	return desc;
}

GraphicsRootSignatureDesc BindBufferTable::CreateGraphicsRootSignatureDesc(const SamplerBindDesc& samplerDesc) {
	GraphicsRootSignatureDesc desc = {};

	UINT rootIndex = 0;

	for (auto& [name, info] : table_) {
		switch (info.bindBufferType) {
			case BindBufferType::kVirtual_CBV:
				desc.SetVirtualCBV(rootIndex, info.visibility, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kVirtual_SRV:
				desc.SetVirtualSRV(rootIndex, info.visibility, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kVirtual_UAV:
				desc.SetVirtualUAV(rootIndex, info.visibility, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kHandle_SRV:
				desc.SetHandleSRV(rootIndex, info.visibility, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kHandle_UAV:
				desc.SetHandleUAV(rootIndex, info.visibility, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kSampler:
				if (samplerDesc.Contains(name)) {
					//!< samplerがある場合はsamplerを設定
					desc.SetSamplerDesc(samplerDesc.GetSampler(name, info.visibility, info.registerNum, info.registerSpace));

				} else {
					//!< samplerがない場合はデフォルトのsamplerを設定
					desc.SetSamplerLinear(MODE_WRAP, info.visibility, info.registerNum, info.registerSpace);
				}
				continue;
				break;
		}

		info.rootParam = rootIndex;
		rootIndex++;
	}

	return desc;
}

ComPtr<ID3D12RootSignature> BindBufferTable::CreateGraphicsRootSignature(Device* device) {
	return CreateGraphicsRootSignatureDesc().CreateGraphicsRootSignature(device->GetDevice());
}

ComputeRootSignatureDesc BindBufferTable::CreateComputeRootSignatureDesc() {

	ComputeRootSignatureDesc desc = {};

	UINT rootIndex = 0;

	for (auto& [name, info] : table_) {

		Exception::Assert(info.visibility == ShaderVisibility::VISIBILITY_ALL, "buffer visibility is not VISIBILITY_ALL");

		switch (info.bindBufferType) {
			case BindBufferType::kVirtual_CBV:
				desc.SetVirtualCBV(rootIndex, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kVirtual_SRV:
				desc.SetVirtualSRV(rootIndex, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kVirtual_UAV:
				desc.SetVirtualUAV(rootIndex, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kHandle_SRV:
				desc.SetHandleSRV(rootIndex, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kHandle_UAV:
				desc.SetHandleUAV(rootIndex, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kSampler:
				desc.SetSamplerLinear(MODE_WRAP, info.visibility, info.registerNum, info.registerSpace);
				continue;
				break;
		}

		info.rootParam = rootIndex;
		rootIndex++;
	}

	return desc;
}

ComputeRootSignatureDesc BindBufferTable::CreateComputeRootSignatureDesc(const SamplerBindDesc& samplerDesc) {
	ComputeRootSignatureDesc desc = {};

	UINT rootIndex = 0;

	for (auto& [name, info] : table_) {

		Exception::Assert(info.visibility == ShaderVisibility::VISIBILITY_ALL, "buffer visibility is not VISIBILITY_ALL");

		switch (info.bindBufferType) {
			case BindBufferType::kVirtual_CBV:
				desc.SetVirtualCBV(rootIndex, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kVirtual_SRV:
				desc.SetVirtualSRV(rootIndex, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kVirtual_UAV:
				desc.SetVirtualUAV(rootIndex, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kHandle_SRV:
				desc.SetHandleSRV(rootIndex, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kHandle_UAV:
				desc.SetHandleUAV(rootIndex, info.registerNum, info.registerSpace);
				break;

			case BindBufferType::kSampler:
				if (samplerDesc.Contains(name)) {
					//!< samplerがある場合はsamplerを設定
					desc.SetSamplerDesc(samplerDesc.GetSampler(name, info.visibility, info.registerNum, info.registerSpace));

				} else {
					//!< samplerがない場合はデフォルトのsamplerを設定
					desc.SetSamplerLinear(MODE_WRAP, info.visibility, info.registerNum, info.registerSpace);
				}
				continue;
				break;
		}

		info.rootParam = rootIndex;
		rootIndex++;
	}

	return desc;
}

ComPtr<ID3D12RootSignature> BindBufferTable::CreateComputeRootSignature(Device* device) {
	return CreateComputeRootSignatureDesc().CreateComputeRootSignature(device->GetDevice());
}

void BindBufferTable::Reset() {
	table_.clear();
}

void BindBufferTable::BindGraphicsBuffer(CommandContext* context, const BindBufferDesc& desc) {

	auto commandList = context->GetCommandList();

	for (const auto& [name, info] : table_) {

		if (info.bindBufferType == BindBufferType::kSampler) {
			continue;
		}

		Exception::Assert(desc.Contains(name), "buffer is not found. buffer name: " + name);

		UINT index = info.rootParam.value();

		switch (info.bindBufferType) {
			case BindBufferType::k32bitConstants:
				{
					const auto& constant = desc.Get32bitConstants(name);
					commandList->SetGraphicsRoot32BitConstants(index, constant.num32bit, constant.data, 0);
				}
				break;

			case BindBufferType::kVirtual_CBV:
				commandList->SetGraphicsRootConstantBufferView(index, desc.GetAddress(name));
				break;

			case BindBufferType::kVirtual_SRV:
				commandList->SetGraphicsRootShaderResourceView(index, desc.GetAddress(name));
				break;

			case BindBufferType::kVirtual_UAV:
				commandList->SetGraphicsRootUnorderedAccessView(index, desc.GetAddress(name));
				break;

			case BindBufferType::kHandle_SRV:
			case BindBufferType::kHandle_UAV:
				commandList->SetGraphicsRootDescriptorTable(index, desc.GetHandle(name));
				break;
		}
	}
}

void BindBufferTable::BindComputeBuffer(CommandContext* context, const BindBufferDesc& desc) {

	auto commandList = context->GetCommandList();

	for (const auto& [name, info] : table_) {

		if (info.bindBufferType == BindBufferType::kSampler) {
			continue;
		}

		Exception::Assert(desc.Contains(name), "buffer is not found. buffer name: " + name);

		UINT index  = info.rootParam.value();

		switch (info.bindBufferType) {
			case BindBufferType::k32bitConstants:
				{
					const auto& constant = desc.Get32bitConstants(name);
					commandList->SetComputeRoot32BitConstants(index, constant.num32bit, constant.data, 0);
				}
				break;

			case BindBufferType::kVirtual_CBV:
				commandList->SetComputeRootConstantBufferView(index, desc.GetAddress(name));
				break;

			case BindBufferType::kVirtual_SRV:
				commandList->SetComputeRootShaderResourceView(index, desc.GetAddress(name));
				break;

			case BindBufferType::kVirtual_UAV:
				commandList->SetComputeRootUnorderedAccessView(index, desc.GetAddress(name));
				break;

			case BindBufferType::kHandle_SRV:
			case BindBufferType::kHandle_UAV:
				commandList->SetComputeRootDescriptorTable(index, desc.GetHandle(name));
				break;
		}
	}
}

BindBufferType BindBufferTable::ToBindBufferType(D3D_SHADER_INPUT_TYPE type) {
	switch (type) {
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER:
			return BindBufferType::kVirtual_CBV;
			break;

		case D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE:
			return BindBufferType::kHandle_SRV;
			break;

		case D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER:
			return BindBufferType::kSampler;
			break;

		case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWTYPED:
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER:
			return BindBufferType::kHandle_UAV;
			break;

		case D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED:
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_RTACCELERATIONSTRUCTURE:
			return BindBufferType::kVirtual_SRV;
			break;

		case D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED:
			return BindBufferType::kVirtual_UAV;
			break;

		default:
			Exception::Assert(false, "D3D_SHADER_INPUT_TYPE is undefine.");
			break;
	}

	return {};
}

void BindBufferTable::InsertBindBuffer(const D3D12_SHADER_INPUT_BIND_DESC& desc, ShaderVisibility visibility) {

	BindBufferInfo info = {};
	info.Create(desc, visibility);

	if (table_.contains(desc.Name)) { //!< buffer conflict.
		// conflictの対応
		//* visibility all になるかの確認
		BindBufferInfo& preInfo = table_.at(desc.Name);

		Exception::Assert(info.type == preInfo.type, "buffer is conflict.");                                                                       //!< bufferの型が違う.
		Exception::Assert(info.registerNum == preInfo.registerNum && info.registerSpace == preInfo.registerSpace, "buffer is not same register."); //!< register関係が違う

		// allに変更
		preInfo.visibility = ShaderVisibility::VISIBILITY_ALL;
		return;
	}

	// 新規追加
	table_.emplace(desc.Name, info);
}
