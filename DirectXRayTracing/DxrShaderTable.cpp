#include "DxrShaderTable.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cassert>
#include <DxrAccelerationStructure.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxrMethod;

//=========================================================================================
// static variables
//=========================================================================================

const UINT DxrObject::ShaderTable::kShaderRecordAlignment_     = D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT;
const UINT DxrObject::ShaderTable::kDefaultShaderRecordSize_   = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
const size_t DxrObject::ShaderTable::kDescriptorGPUHandleSize_ = sizeof(D3D12_GPU_DESCRIPTOR_HANDLE);
const UINT DxrObject::ShaderTable::kTableAlignment_            = D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT;

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderTable class method
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::ShaderTable::Init(
	int32_t clientWidth, int32_t clientHeight,
	const StateObjectDesc& descs,
	TopLevelAS* tlas, StateObject* stateObject, ResultBuffer* resultBuffer) {
	clientWidth; clientHeight;

	// TODO: descから取れるようにしておくこと

	// raygeneration
	UINT raygenerationRecordSize = kDefaultShaderRecordSize_;
	raygenerationRecordSize += kDescriptorGPUHandleSize_; //!< u0
	raygenerationRecordSize = Alignment(raygenerationRecordSize, kShaderRecordAlignment_);

	// hitgroup
	UINT hitgroupRecordSize = kDefaultShaderRecordSize_;
	hitgroupRecordSize += kDescriptorGPUHandleSize_ * 2; //!< t1, t2
	hitgroupRecordSize = Alignment(hitgroupRecordSize, kShaderRecordAlignment_);

	// miss
	UINT missRecordSize = kDefaultShaderRecordSize_;
	missRecordSize = Alignment(missRecordSize, kShaderRecordAlignment_);

	// 使用する各シェーダーの個数より、シェーダーテーブルのサイズを求める.
	UINT raygenerationSize = 1 * raygenerationRecordSize;
	UINT missSize          = 1 * missRecordSize;
	UINT hitgroupSize      = 2 * hitgroupRecordSize;

	// 各テーブルの開始位置にアライメント調整
	UINT raygenerationRegion = Alignment(raygenerationSize, kTableAlignment_);
	UINT missRegion          = Alignment(missSize, kTableAlignment_);
	UINT hitgroupRegion      = Alignment(hitgroupSize, kTableAlignment_);

	// resourceの生成
	UINT tableSize = raygenerationRegion + missRegion + hitgroupRegion;
	
	table_ = CreateBuffer(
		D3D12_HEAP_TYPE_UPLOAD,
		tableSize,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ
	);

	// クエリ
	ComPtr<ID3D12StateObjectProperties> properties;
	stateObject->GetComPtrStateObject().As(&properties);

	// resourceにshaderRecordを書き込み
	void* mapped = nullptr;
	table_->Map(0, nullptr, &mapped);
	uint8_t* pStart = static_cast<uint8_t*>(mapped);

	// raygenerationのシェーダーレコード書き込み
	{
		auto reygenerationStart = pStart;
		uint8_t* p = reygenerationStart;

		auto id = properties->GetShaderIdentifier(
			descs.blob->GetMainFunctionName(RAYGENERATION_SHAEAR).c_str()
		);

		if (id == nullptr) {
			// Not found raygeneration identifier
			assert(false);
		}

		p += WriteShaderIdentifier(p, id);

		// ローカルルートシグネイチャで u0 (出力先) を設定してるため, 対応してるDescriptorに書き込み
		p += WriteGPUDescriptor(p, resultBuffer->GetDescriptorUAV());
	}

	// missのシェーダーレコード書き込み
	{
		auto missStart = pStart + raygenerationRegion;
		uint8_t* p = missStart;

		auto id = properties->GetShaderIdentifier(
			descs.blob->GetMainFunctionName(MISS_SHADER).c_str()
		);

		if (id == nullptr) {
			// Not found miss identifier
			assert(false);
		}

		p += WriteShaderIdentifier(p, id);
	}

	// hitgroupのシェーダーレコード書き込み
	{
		// XXX: hitgroupが一つしかない
		auto hitgroupStart = pStart + raygenerationRegion + missRegion;
		uint8_t* pRecord = hitgroupStart;

		for (const auto& blas : tlas->GetBLASPtrArray()) {
			pRecord = WriteShaderRecord(pRecord, blas, hitgroupSize, properties.Get());
		}
	}

	table_->Unmap(0, nullptr);

	auto startAddress = table_->GetGPUVirtualAddress();

	// raygeneration desc
	dispatchRayDesc_.RayGenerationShaderRecord.StartAddress = startAddress;
	dispatchRayDesc_.RayGenerationShaderRecord.SizeInBytes  = raygenerationSize;
	startAddress += raygenerationRegion;

	// miss desc
	dispatchRayDesc_.MissShaderTable.StartAddress  = startAddress;
	dispatchRayDesc_.MissShaderTable.SizeInBytes   = missSize;
	dispatchRayDesc_.MissShaderTable.StrideInBytes = missRecordSize;
	startAddress += missRegion;

	// hitgroup desc
	dispatchRayDesc_.HitGroupTable.StartAddress  = startAddress;
	dispatchRayDesc_.HitGroupTable.SizeInBytes   = hitgroupSize;
	dispatchRayDesc_.HitGroupTable.StrideInBytes = hitgroupRecordSize;
	startAddress += hitgroupRegion;

	dispatchRayDesc_.Width  = clientWidth;
	dispatchRayDesc_.Height = clientHeight;
	dispatchRayDesc_.Depth  = 1;

}

void DxrObject::ShaderTable::Term() {
}

////////////////////////////////////////////////////////////////////////////////////////////
// DxrMethod namespace method
////////////////////////////////////////////////////////////////////////////////////////////

UINT DxrMethod::Alignment(size_t size, UINT align) {
	return UINT(size + align - 1) & ~(align - 1);
}

UINT DxrMethod::WriteShaderIdentifier(void* dst, const void* shaderId) {
	memcpy(dst, shaderId, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	return D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;
}

UINT DxrMethod::WriteGPUDescriptor(void* dst, const DxObject::Descriptor& descriptor) {
	auto handle = descriptor.handleGPU;
	memcpy(dst, &handle, sizeof(handle));
	return UINT(sizeof(handle));
}

uint8_t* DxrMethod::WriteShaderRecord(
	uint8_t* dst,
	DxrObject::BottomLevelAS* blas, UINT recordSize,
	ID3D12StateObjectProperties* prop) {

	auto entry = dst;
	auto id = prop->GetShaderIdentifier(blas->GetHitgruop().c_str());

	if (id == nullptr) {
		// Not found raygeneration identifier
		assert(false);
	}

	dst += WriteShaderIdentifier(dst, id);
	dst += WriteGPUDescriptor(dst, blas->GetIndicesDescriptor()); // t1
	dst += WriteGPUDescriptor(dst, blas->GetVerticesDescriptor()); // t2

	dst = entry + recordSize;
	return dst;
}
