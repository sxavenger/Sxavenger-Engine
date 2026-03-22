#include "FProcessTexture.h"
SXAVENGER_ENGINE_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* render
#include "../Core/FRenderCore.h"
#include "../Core/FRenderCoreTransition.h"

//* engine
#include <Engine/System/Utility/StreamLogger.h>
#include <Engine/System/System.h>

//* lib
#include <Lib/Geometry/VectorComparison.h>
#include <Lib/Adapter/String/EncodedString.h>

//* c++
#include <bit>

////////////////////////////////////////////////////////////////////////////////////////////
// Argument structure methods
////////////////////////////////////////////////////////////////////////////////////////////

bool FProcessTexture::Argument::Compatible(const Argument& other) const {
	bool compatible = true;
	compatible &= Comparison::All(resolution == other.resolution);
	compatible &= format == other.format;
	compatible &= maxMiplevel == other.maxMiplevel;

	return compatible;
}

uint16_t FProcessTexture::Argument::GetMiplevel() const {
	uint32_t maxDimension = std::max(resolution.x, resolution.y);
	uint16_t mipLevel = static_cast<uint16_t>(std::bit_width(maxDimension)); //!< std::log2(maxDimension) + 1と同等.

	return std::min(mipLevel, maxMiplevel.value_or(std::numeric_limits<uint16_t>::max()));
	//!< maxMipLevelが指定されている場合はその値を、そうでない場合は計算されたmipLevelを返す.
}

////////////////////////////////////////////////////////////////////////////////////////////
// FProcessTexture class methods
////////////////////////////////////////////////////////////////////////////////////////////

void FProcessTexture::Create(const Argument& argument) {

	if (argument_.Compatible(argument)) {
		return; //!< 互換性のあるオプションがすでに保存されている場合は何もしない
	}

	CreateResource(argument);
	CreateDescriptor(argument);

	// 引数の保存
	argument_ = argument;

}

std::optional<D3D12_RESOURCE_BARRIER> FProcessTexture::GetTransitionState(D3D12_RESOURCE_STATES state) {
	return resource_.GetTransition(state);
}

void FProcessTexture::SetTransitionState(std::vector<D3D12_RESOURCE_BARRIER>& barriers, D3D12_RESOURCE_STATES state) {
	resource_.SetTransition(barriers, state);
}

void FProcessTexture::TransitionState(const DirectXQueueContext* context, D3D12_RESOURCE_STATES state) {
	resource_.Transition(context->GetDxCommand(), state);
}

NODISCARD std::optional<D3D12_RESOURCE_BARRIER> FProcessTexture::GetTransitionDefaultState() {
	return GetTransitionState(kDefaultState);
}

void FProcessTexture::SetTransitionDefaultState(std::vector<D3D12_RESOURCE_BARRIER>& barriers) {
	SetTransitionState(barriers, kDefaultState);
}

void FProcessTexture::TransitionDefaultState(const DirectXQueueContext* context) {
	TransitionState(context, kDefaultState);
}

NODISCARD std::optional<D3D12_RESOURCE_BARRIER> FProcessTexture::GetTransitionUnorderedAccess() {
	return GetTransitionState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

void FProcessTexture::SetTransitionUnorderedAccess(std::vector<D3D12_RESOURCE_BARRIER>& barriers) {
	SetTransitionState(barriers, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

void FProcessTexture::TransitionUnorderedAccess(const DirectXQueueContext* context) {
	TransitionState(context, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

void FProcessTexture::BarrierUnorderedAccess(const DirectXQueueContext* context) {
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type          = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = resource_.Get();

	context->GetCommandList()->ResourceBarrier(1, &barrier);
}

void FProcessTexture::GenerateMipmap(const DirectXQueueContext* context, const std::optional<uint16_t>& miplevel) {

	//!< stateの遷移.
	TransitionUnorderedAccess(context);

	//!< Pipelineの設定.
	auto core = FRenderCore::GetInstance()->EnsureRenderCore<FRenderCoreTransition>();
	core->SetPipeline(FRenderCoreTransition::Transition::MipmapTransition, context);

	//!< mipmapの生成.
	context->BeginEvent(L"[FProcessTexture] GenerateMipmap");

	uint16_t m = std::min(argument_.GetMiplevel(), miplevel.value_or(std::numeric_limits<uint16_t>::max()));
	//!< 生成するmiplevelの数を決定.

	for (uint16_t i = 1; i < m; ++i) {

		Vector2ui dimension = {
			std::max<uint32_t>(argument_.resolution.x >> i, 1),
			std::max<uint32_t>(argument_.resolution.y >> i, 1)
		};

		//!< parameterの設定.
		DxObject::BindBufferDesc desc = {};
		desc.Set32bitConstants("Dimension", 2, &dimension);
		desc.SetHandle("gOutput", descriptorsUAV_[i].GetGPUHandle());
		desc.SetHandle("gInput",  descriptorsUAV_[i - 1].GetGPUHandle());

		core->BindComputeBuffer(FRenderCoreTransition::Transition::MipmapTransition, context, desc);
		core->Dispatch(context, dimension);

		//!< UAVのバリア.
		BarrierUnorderedAccess(context);
	}

	context->EndEvent();
}

void FProcessTexture::ImportFromRenderTexture(const DirectXQueueContext* context, FRenderTexture* texture) {

	{ //!< barrierの設定 (default -> copy)

		std::vector<D3D12_RESOURCE_BARRIER> barriers;
		texture->SetTransitionState(barriers, D3D12_RESOURCE_STATE_COPY_SOURCE);
		SetTransitionState(barriers, D3D12_RESOURCE_STATE_COPY_DEST);

		context->GetDxCommand()->ResourceBarrier(barriers);
	}

	{ //!< copyの実行(miplevel0のみ)

		D3D12_TEXTURE_COPY_LOCATION source = {};
		source.pResource        = texture->GetResource();
		source.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		source.SubresourceIndex = 0;

		D3D12_TEXTURE_COPY_LOCATION destination = {};
		destination.pResource        = resource_.Get();
		destination.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		destination.SubresourceIndex = 0;

		context->GetCommandList()->CopyTextureRegion(&destination, 0, 0, 0, &source, nullptr);
	}

	{ //!< barrierの設定 (copy -> default)
		std::vector<D3D12_RESOURCE_BARRIER> barriers;
		texture->SetTransitionDefaultState(barriers);
		SetTransitionDefaultState(barriers);

		context->GetDxCommand()->ResourceBarrier(barriers);
	}
	
}

void FProcessTexture::ExportToRenderTexture(const DirectXQueueContext* context, FRenderTexture* texture) {

	{ //!< barrierの設定 (default -> copy)

		std::vector<D3D12_RESOURCE_BARRIER> barriers;
		SetTransitionState(barriers, D3D12_RESOURCE_STATE_COPY_SOURCE);
		texture->SetTransitionState(barriers, D3D12_RESOURCE_STATE_COPY_DEST);

		context->GetDxCommand()->ResourceBarrier(barriers);
	}

	{ //!< copyの実行(miplevel0のみ)

		D3D12_TEXTURE_COPY_LOCATION source = {};
		source.pResource        = resource_.Get();
		source.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		source.SubresourceIndex = 0;

		D3D12_TEXTURE_COPY_LOCATION destination = {};
		destination.pResource        = texture->GetResource();
		destination.Type             = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		destination.SubresourceIndex = 0;

		context->GetCommandList()->CopyTextureRegion(&destination, 0, 0, 0, &source, nullptr);
	}

	{ //!< barrierの設定 (copy -> default)
		std::vector<D3D12_RESOURCE_BARRIER> barriers;
		SetTransitionDefaultState(barriers);
		texture->SetTransitionDefaultState(barriers);

		context->GetDxCommand()->ResourceBarrier(barriers);
	}
}

const DxObject::Descriptor& FProcessTexture::GetDescriptorSRV() const {
	StreamLogger::AssertA(descriptorSRV_.HasHandle(), "ShaderResource View is not created.");
	return descriptorSRV_;
}

const D3D12_GPU_DESCRIPTOR_HANDLE& FProcessTexture::GetGPUHandleSRV() const {
	return GetDescriptorSRV().GetGPUHandle();
}

const DxObject::Descriptor& FProcessTexture::GetDescriptorUAV(uint16_t miplevel) const {
	StreamLogger::AssertA(miplevel < descriptorsUAV_.size(), "Invalid miplevel is specified.");
	StreamLogger::AssertA(descriptorsUAV_[miplevel].HasHandle(), "UnorderedAccess View is not created.");
	return descriptorsUAV_[miplevel];
}

const D3D12_GPU_DESCRIPTOR_HANDLE& FProcessTexture::GetGPUHandleUAV(uint16_t miplevel) const {
	return GetDescriptorUAV(miplevel).GetGPUHandle();
}

void FProcessTexture::CreateResource(const Argument& argument) {

	// propの設定
	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;

	// descの設定
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width            = argument.resolution.x;
	desc.Height           = argument.resolution.y;
	desc.DepthOrArraySize = 1;
	desc.MipLevels        = argument.GetMiplevel();
	desc.Format           = argument.format;
	desc.SampleDesc.Count = 1;
	desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	// resourceの生成
	resource_.CreateCommitted(
		System::GetDxDevice(),
		prop,
		desc,
		kDefaultState,
		std::nullopt
	);

	resource_.SetName(L"FProcessTexture");
}

void FProcessTexture::CreateDescriptor(const Argument& argument) {

	auto device = System::GetDxDevice()->GetDevice(); //!< deviceの取り出し

	{ //!< SRVの生成

		// handleの取得
		if (!descriptorSRV_.HasHandle()) {
			descriptorSRV_ = System::GetDescriptor(DxObject::kDescriptor_SRV);
		}

		// descの設定
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format                  = argument.format;
		desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Texture2D.MipLevels     = 1;

		// SRVの生成
		device->CreateShaderResourceView(
			resource_.Get(),
			&desc,
			descriptorSRV_.GetCPUHandle()
		);
	}

	{ //!< UAVの生成

		uint16_t miplevel = argument.GetMiplevel(); //!< 生成するmipLevelの数.

		descriptorsUAV_.resize(miplevel);
		for (uint32_t i = 0; i < miplevel; ++i) {

			// handleの取得
			if (!descriptorsUAV_[i].HasHandle()) {
				descriptorsUAV_[i] = System::GetDescriptor(DxObject::kDescriptor_UAV);
			}

			// descの設定
			D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
			desc.Format             = argument.format;
			desc.ViewDimension      = D3D12_UAV_DIMENSION_TEXTURE2D;
			desc.Texture2D.MipSlice = i;

			// UAVの生成
			device->CreateUnorderedAccessView(
				resource_.Get(),
				nullptr,
				&desc,
				descriptorsUAV_[i].GetCPUHandle()
			);
		}
	}

}


