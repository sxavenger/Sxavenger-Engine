#include "DxrExportGroup.h"
_DXROBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// ExportGroup class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ExportGroup::Create(const std::wstring& name, ExportGroupType type) {
	// 引数の確認
	Assert(!name.empty());
	Assert(type != kCountOfExportGroupType);

	// parameterの保存
	name_ = name;
	type_ = type;
}

void ExportGroup::CreateRootSignature(const LocalRootSignatureDesc& desc) {
	rootSignature_ = std::make_unique<LocalRootSignature>(desc);
}

void ExportGroup::SetEntryPointHitgroup(const std::wstring& closesthit, const std::wstring& anyhit, const std::wstring& intersection) {
	
	Assert(type_ == kExport_Hitgroup);

	// entryPointの登録
	entryPoint_ = ExportEntryPointHitgroup{
		.closesthit   = closesthit,
		.anyhit       = anyhit,
		.intersection = intersection
	};
}

const ExportGroup::ExportEntryPointHitgroup& ExportGroup::GetExportHitgroupEntryPoints() const {
	Assert(type_ == ExportGroupType::kExport_Hitgroup);

	if (!std::holds_alternative<ExportEntryPointHitgroup>(entryPoint_)) {
		Assert(false); //!< entryPoint未登録
	}

	return std::get<ExportEntryPointHitgroup>(entryPoint_);
}
