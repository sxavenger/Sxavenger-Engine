#include "DxrShaderTable.h"
_DXROBJECT_USING

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Sxavenger.h>

//* lib
#include <Lib/Environment.h>

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderTable class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ShaderTable::Create(
	StateObject* stateObject, TopLevelAS* tlas,
	const BufferRecoreder* raygeneration, const BufferRecoreder* miss) {

	// shader単体のsizeの設定
	// raygeneration
	UINT raygenerationRecordSize = kDefaultShaderRecordSize;
	raygenerationRecordSize     += static_cast<UINT>(stateObject->GetMaxBufferSize(ExportGroupType::kExportGroup_Raygeneration));
	raygenerationRecordSize      = Alignment(raygenerationRecordSize, kShaderRecordAlignment);

	// miss
	UINT missRecordSize = kDefaultShaderRecordSize;
	missRecordSize     += static_cast<UINT>(stateObject->GetMaxBufferSize(ExportGroupType::kExportGroup_Miss));
	missRecordSize      = Alignment(missRecordSize, kShaderRecordAlignment);

	// hitgroup
	UINT hitgroupRecordSize = kDefaultShaderRecordSize;
	hitgroupRecordSize     += static_cast<UINT>(stateObject->GetMaxBufferSize(ExportGroupType::kExportGroup_Hitgroup));
	hitgroupRecordSize      = Alignment(hitgroupRecordSize, kShaderRecordAlignment);

	// hack: raygenerationは1つしか含まれない
	Assert(stateObject->GetExportCount(ExportGroupType::kExportGroup_Raygeneration) == 1, "raytracing enviorment can set one raygeneration.");

	// 使用する各シェーダーの個数より、シェーダーテーブルのサイズを求める.
	UINT raygenerationSize = 1 * raygenerationRecordSize;
	UINT missSize          = static_cast<UINT>(stateObject->GetExportCount(ExportGroupType::kExportGroup_Miss)) * missRecordSize;
	UINT hitgroupSize      = static_cast<UINT>(tlas->GetInstances().size()) * hitgroupRecordSize;

	// 各テーブル開始位置にアライメント調整
	UINT raygenerationRegion = Alignment(raygenerationSize, kShaderTableAlignment);
	UINT missRegion          = Alignment(missSize, kShaderTableAlignment);
	UINT hitgroupRegion      = Alignment(hitgroupSize, kShaderTableAlignment);

	// 合計したtableのサイズ
	UINT tableSize = raygenerationRegion + missRegion + hitgroupRegion;

	table_.Reset();

	// tableのresourceの生成
	table_ = DxObject::CreateBufferResource(
		Sxavenger::GetDevice(),
		D3D12_HEAP_TYPE_UPLOAD,
		tableSize,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_RESOURCE_STATE_GENERIC_READ
	);

	properties_ = stateObject->GetProperties();

	// mapping
	void* mapped = nullptr;
	table_->Map(0, nullptr, &mapped);
	uint8_t* addressStart = static_cast<uint8_t*>(mapped);

	// raygeneration
	{
		auto raygenerationStart = addressStart;
		uint8_t* record = raygenerationStart;

		record = WriteShaderRecord(record, raygenerationRecordSize, raygeneration);
	}

	// missのrecord書き込み
	{
		auto missStart  = addressStart + raygenerationRegion;
		uint8_t* record = missStart;

		record = WriteShaderRecord(record, missRecordSize, miss);
	}

	// hitgroupの書き込み
	{
		auto hitgroupStart = addressStart + raygenerationRegion + missRegion;
		uint8_t* record = hitgroupStart;

		for (const auto& instance : tlas->GetInstances()) {
			record = WriteShaderRecord(record, hitgroupRecordSize, instance.recorder);
		}
	}

	table_->Unmap(0, nullptr);

	// 情報をもとにDESCに設定
	auto address = table_->GetGPUVirtualAddress();

	dispathRayDesc_ = {};

	// raygenerationの設定
	dispathRayDesc_.RayGenerationShaderRecord.StartAddress = address;
	dispathRayDesc_.RayGenerationShaderRecord.SizeInBytes  = raygenerationSize;
	
	// address位置をmissが書き込まれてる位置に変換
	address += raygenerationRegion;

	// missの設定
	dispathRayDesc_.MissShaderTable.StartAddress   = address;
	dispathRayDesc_.MissShaderTable.SizeInBytes    = missSize;
	dispathRayDesc_.MissShaderTable.StrideInBytes  = missRecordSize;

	// address位置をhitgroupが書き込まれてる位置に変換
	address += missRegion;

	// hitgroupの設定
	if (!tlas->GetInstances().empty()) {
		dispathRayDesc_.HitGroupTable.StartAddress  = address;
		dispathRayDesc_.HitGroupTable.SizeInBytes   = hitgroupSize;
		dispathRayDesc_.HitGroupTable.StrideInBytes = hitgroupRecordSize;
	}

	address += hitgroupRegion;

	// FIXME: 描画Textureからsizeを持ってくる
	dispathRayDesc_.Width  = kWindowSize.x;
	dispathRayDesc_.Height = kWindowSize.y;
	dispathRayDesc_.Depth  = 1;
}

void ShaderTable::DispatchRays() {

	auto commandList = Sxavenger::GetCommandList();

	commandList->DispatchRays(&dispathRayDesc_);
}

uint8_t* ShaderTable::WriteShaderRecord(uint8_t* dst, UINT recordSize, const BufferRecoreder* recorder) {

	// 開始点の保存
	auto entry = dst;

	// exportnameの取得
	auto id = properties_->GetShaderIdentifier(recorder->GetExportGroup()->GetName().c_str());
	AssertW(id != nullptr, L"Not found export identifier. \nexport name: " + recorder->GetExportGroup()->GetName());

	// id分の書き込み
	dst += WirteShaderIdentifier(dst, id);
	
	// bufferの書き込み
	for (const auto& buffer : recorder->GetRecorder()) {

		// 型の判別
		if (std::holds_alternative<GPUAddress>(buffer)) { //!< Addressの場合
			// Addressの参照取得
			const auto& address = std::get<GPUAddress>(buffer);
			Assert(address.has_value(), "address is not set."); //!< addressがsetされていない

			// Addressの書き込み
			dst += WriteGPUAddress(dst, address.value());
			continue;

		} else if (std::holds_alternative<GPUHandle>(buffer)) {
			// handleの参照取得
			const auto& handle = std::get<GPUHandle>(buffer);
			Assert(handle.has_value(), "handle is not set."); //!< handleがsetされてない

			// handleの書き込み
			dst += WriteGPUHandle(dst, handle.value());
			continue;
		}

		Assert(false); //!< 例外の型が書き込まれてる
	}
	

	dst = entry + recordSize;
	return dst;
}

UINT ShaderTable::Alignment(size_t size, UINT align) {
	return UINT(size + align - 1) & ~(align - 1);
}

UINT ShaderTable::WirteShaderIdentifier(void* dst, const void* shaderId) {
	std::memcpy(dst, shaderId, kDefaultShaderRecordSize);
	return kDefaultShaderRecordSize;
}

UINT ShaderTable::WriteGPUAddress(void* dst, const D3D12_GPU_VIRTUAL_ADDRESS& address) {
	std::memcpy(dst, &address, kGPUBufferSize);
	return kGPUBufferSize;
}

UINT ShaderTable::WriteGPUHandle(void* dst, const D3D12_GPU_DESCRIPTOR_HANDLE& handle) {
	std::memcpy(dst, &handle, kGPUBufferSize);
	return kGPUBufferSize;
}


