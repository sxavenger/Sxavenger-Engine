#include "DxrShaderTable.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
#include <cassert>
#include <Logger.h>
#include <DxrAccelerationStructure.h>

//-----------------------------------------------------------------------------------------
// using
//-----------------------------------------------------------------------------------------
using namespace DxrMethod;

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderTable class method
////////////////////////////////////////////////////////////////////////////////////////////

void DxrObject::ShaderTable::Init(int32_t clientWidth, int32_t clientHeight) {

	dispatchRayDesc_.Width  = clientWidth;
	dispatchRayDesc_.Height = clientHeight;
	dispatchRayDesc_.Depth  = 1;
}

void DxrObject::ShaderTable::Record(
	TopLevelAS* tlas, StateObject* stateObject,
	RecordBuffer* raygenearation, RecordBuffer* miss) {

	// raygeneration
	UINT raygenerationRecordSize = kDefaultShaderRecordSize;
	raygenerationRecordSize     += kGPUDescriptorHandle; // todo: raygenerationの最大size
	raygenerationRecordSize      = Alignment(raygenerationRecordSize, kShaderRecordAlignment);

	// hitgroup
	UINT hitgroupRecordSize = kDefaultShaderRecordSize;
	hitgroupRecordSize     += static_cast<UINT>(tlas->GetTopRecordSize()); // 登録されている中での最大sizeをrecord
	hitgroupRecordSize      = Alignment(hitgroupRecordSize, kShaderRecordAlignment);

	// miss
	UINT missRecordSize = kDefaultShaderRecordSize;
	missRecordSize      = Alignment(missRecordSize, kShaderRecordAlignment);

	// 使用する各シェーダーの個数より、シェーダーテーブルのサイズを求める.
	UINT raygenerationSize = 1 * raygenerationRecordSize;
	UINT missSize          = 1 * missRecordSize;
	UINT hitgroupSize      = tlas->GetObjectSize() * hitgroupRecordSize; // hack

	// 各テーブルの開始位置にアライメント調整
	UINT raygenerationRegion = Alignment(raygenerationSize, kTableAlignment);
	UINT missRegion          = Alignment(missSize, kTableAlignment);
	UINT hitgroupRegion      = Alignment(hitgroupSize, kTableAlignment);

	// テーブル制限の確認
	UINT tableSize = raygenerationRegion + missRegion + hitgroupRegion;

	// resourceの生成
	table_ = CreateBuffer(
		D3D12_HEAP_TYPE_UPLOAD,
		tableSize,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ
	);

	// クエリ
	auto properties = stateObject->GetProperties();

	// resourceにshaderRecordを書き込み
	void* mapped = nullptr;
	table_->Map(0, nullptr, &mapped);
	uint8_t* pStart = static_cast<uint8_t*>(mapped);

	// raygenerationのシェーダーレコード書き込み
	{
		auto reygenerationStart = pStart;
		uint8_t* pRecord = reygenerationStart;

		// HACK: raygenerationが複数あってもいいように改良
		pRecord = WriteShaderRecord(pRecord, raygenearation, raygenerationRecordSize, properties);
	}

	// missのシェーダーレコード書き込み
	{
		auto missStart = pStart + raygenerationRegion;
		uint8_t* pRecord = missStart;

		pRecord = WriteShaderRecord(pRecord, miss, missRecordSize, properties);
	}

	// hitgroupのシェーダーレコード書き込み
	{
		auto hitgroupStart = pStart + raygenerationRegion + missRegion;
		uint8_t* pRecord = hitgroupStart;

		for (auto& blas : tlas->GetInstances()) { //!< HACK: hitgroupIndexを手動設定する場合, ここを変える必要があるかも
			pRecord = WriteShaderRecord(pRecord, blas.first->GetRecordBuffer(), hitgroupRecordSize, properties);
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
	memcpy(dst, shaderId, DxrObject::kDefaultShaderRecordSize);
	return DxrObject::kDefaultShaderRecordSize;
}

UINT DxrMethod::WriteGPUDescriptor(void* dst, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {
	memcpy(dst, &handle, sizeof(handle));
	return UINT(sizeof(handle));
}

UINT DxrMethod::WriteGPUVirtualAddress(void* dst, const D3D12_GPU_VIRTUAL_ADDRESS& address) {
	memcpy(dst, &address, sizeof(address));
	return UINT(sizeof(address));
}

uint8_t* DxrMethod::WriteShaderRecord(
	uint8_t* dst,
	DxrObject::RecordBuffer* buffer, UINT recordSize,
	ID3D12StateObjectProperties* prop) {

	auto entry = dst;

	auto id = prop->GetShaderIdentifier(buffer->GetExport().c_str());

	if (id == nullptr) {
		Log(L"WriteShaderRecord Error: not found export identifier. [export]: " + buffer->GetExport());
		assert(false);
	}

	// id分の書き込み
	dst += WriteShaderIdentifier(dst, id);

	if (buffer->GetRecordOrder().empty()) { //!< 書き込むBufferがないので早期return
		dst = entry + recordSize;
		return dst;
	}

	// recordOrderの順に書き込み
	for (const auto& record : buffer->GetRecordOrder()) {
		// 型の判別
		// TODO: localRootSignatureと整合性が取れているかの確認
		if (std::holds_alternative<D3D12_GPU_DESCRIPTOR_HANDLE>(record)) {
			dst += WriteGPUDescriptor(dst, std::get<D3D12_GPU_DESCRIPTOR_HANDLE>(record));
			continue;

		} else if (std::holds_alternative<D3D12_GPU_VIRTUAL_ADDRESS>(record)) {
			dst += WriteGPUVirtualAddress(dst, std::get<D3D12_GPU_VIRTUAL_ADDRESS>(record));
			continue;
		}

		assert(false); //!< 書き込み型の判別未定義
	}

	dst = entry + recordSize;
	return dst;
}
