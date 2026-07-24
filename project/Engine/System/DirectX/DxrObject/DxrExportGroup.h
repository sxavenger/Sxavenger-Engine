#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include "DxrObjectCommon.h"
#include "DxrRootSignature.h"
#include "DxrRaytracingBlob.h"

//* c++
#include <optional>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
DXROBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ExportType enum class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief レイトレシェーダーのエクスポート種別
enum class ExportType : uint8_t {
	Raygeneration,
	Miss,
	Hitgroup,
	//Callable
};
constexpr uint8_t kExportTypeCount = static_cast<uint8_t>(ExportType::Hitgroup) + 1;

////////////////////////////////////////////////////////////////////////////////////////////
// ExportGroup class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief 1つのレイトレシェーダーエントリ(とローカルルートシグネチャ)をまとめたエクスポート単位
class ExportGroup
	: public LocalRootSignature {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Hitgroup structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief ヒットグループを構成するシェーダー(closesthit/anyhit/intersection)の指定
	struct Hitgroup {
		D3D12_HIT_GROUP_TYPE type = D3D12_HIT_GROUP_TYPE_TRIANGLES;
		std::wstring closesthit   = L"";
		std::wstring anyhit       = L"";
		std::wstring intersection = L"";
	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ExportGroup()  = default;
	~ExportGroup() = default;

	//* export options *//

	//! @brief RayGenerationシェーダーとしてエクスポートを設定する
	//! @param[in] entry シェーダーのエントリポイント名
	void ExportRaygeneration(const std::wstring& entry);

	//! @brief Missシェーダーとしてエクスポートを設定する
	//! @param[in] entry シェーダーのエントリポイント名
	void ExportMiss(const std::wstring& entry);

	//! @brief ヒットグループとしてエクスポートを設定する
	//! @param[in] name     ヒットグループ名
	//! @param[in] hitgroup 構成シェーダーの指定
	void ExportHitgroup(const std::wstring& name, const Hitgroup& hitgroup);

	//! @brief シェーダー本体のBlobを関連付ける
	//! @param[in] blob レイトレシェーダーBlob
	void SetBlob(const RaytracingBlob* blob) { blob_ = blob; }

	//* getter *//

	//! @brief このエクスポートの種別を取得する
	//! @return エクスポート種別
	ExportType GetType() const;

	//! @brief エクスポート名を取得する
	//! @return エクスポート名
	const std::wstring& GetName() const { return name_; }

	//! @brief ヒットグループ構成を取得する
	//! @return Hitgroupへの参照
	const Hitgroup& GetHitgroup() const { return hitgroup_; }

	//! @brief 関連付けられたシェーダーBlobを取得する
	//! @return シェーダーBlobへのポインタ
	const RaytracingBlob* GetBlob() const { return blob_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::wstring name_ = L"";
	Hitgroup hitgroup_ = {};

	std::optional<ExportType> type_ = std::nullopt;

	const RaytracingBlob* blob_ = nullptr;

};

DXROBJECT_NAMESPACE_END
