#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "BaseAsset.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/System/DirectX/DirectXAlignment.h>

//* lib
#include <Lib/Math/Color3.h>
#include <Lib/Math/Matrix4x4.h>
#include <Lib/Sxl/Flag.h>
#include <Lib/Transform/Transform2d.h>
#include <Lib/Adapter/Uuid/Uuid.h>

//* external
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//* c++
#include <array>
#include <optional>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// AssetMaterial class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief マテリアルの各種パラメータ(色/テクスチャ/UV変換等)を保持し, GPUへ転送するAsset
class AssetMaterial final
	: public BaseAsset {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Texture enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief マテリアルが持つテクスチャの種類
	enum class Texture : uint8_t {
		Albedo,
		Transparency,
		Normal,
		Roughness,
		Metallic,
		Emissive,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// UVTransformation structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief テクスチャのUV座標に適用する変換行列を保持する構造体
	struct UVTransformation {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Matrix4x4 mat = Matrix4x4::Identity();

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Albedo structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief アルベド(基本色)の値とテクスチャindexを保持する構造体
	struct Albedo {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Color3f value = kWhite3<float>;
		uint32_t index = NULL;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Transparency structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 透明度の値とテクスチャindexを保持する構造体
	struct Transparency {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		float value = 1.0f;
		uint32_t index = NULL;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Normal structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 法線マップのテクスチャindexを保持する構造体
	struct Normal {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint32_t index = NULL;

		};

	////////////////////////////////////////////////////////////////////////////////////////////
	// [Helper] Property structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief スカラー値とテクスチャindexを持つ汎用プロパティ構造体 (roughness/metallic等で使用)
	struct Property {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		float value = 0.0f;
		uint32_t index = NULL;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Emissive structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief 自己発光の色/テクスチャindex/強度を保持する構造体
	struct Emissive {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Color3f value = kBlack3<float>;
		uint32_t index = NULL;
		float intensity = 1.0f;

		};

	////////////////////////////////////////////////////////////////////////////////////////////
	// MaterialBuffer structure
	////////////////////////////////////////////////////////////////////////////////////////////
	//! @brief GPUの定数バッファへ転送するマテリアルパラメータの本体. 各テクスチャの有無をフラグで管理する
	struct MaterialBuffer {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// TextureFlag enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		//! @brief 各テクスチャが設定されているかをbitで表すフラグ
		enum class TextureFlag : uint32_t {
			None         = 0,
			Albedo       = 1 << 0,
			Transparency = 1 << 1,
			Normal       = 1 << 2,
			Roughness    = 1 << 3,
			Metallic     = 1 << 4,
			Emissive     = 1 << 5,
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//! @brief 各パラメータを既定値へ初期化する
		void Init();

		//* texture option *//

		//! @brief アルベドテクスチャのindexを設定し, 対応するフラグを立てる
		//! @param[in] descriptor テクスチャのディスクリプタ
		void SetAlbedoTexture(const DxObject::ReferenceDescriptor& descriptor);
		//! @brief アルベドテクスチャを未設定にし, フラグを下ろす
		void SetAlbedoTexture(std::nullopt_t);
		//! @brief アルベドテクスチャを設定する (optionalが無効なら未設定として扱う)
		//! @param[in] descriptor テクスチャのディスクリプタ (無効値可)
		void SetAlbedoTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor);

		//! @brief 透明度テクスチャのindexを設定し, 対応するフラグを立てる
		//! @param[in] descriptor テクスチャのディスクリプタ
		void SetTransparencyTexture(const DxObject::ReferenceDescriptor& descriptor);
		//! @brief 透明度テクスチャを未設定にし, フラグを下ろす
		void SetTransparencyTexture(std::nullopt_t);
		//! @brief 透明度テクスチャを設定する (optionalが無効なら未設定として扱う)
		//! @param[in] descriptor テクスチャのディスクリプタ (無効値可)
		void SetTransparencyTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor);

		//! @brief 法線テクスチャのindexを設定し, 対応するフラグを立てる
		//! @param[in] descriptor テクスチャのディスクリプタ
		void SetNormalTexture(const DxObject::ReferenceDescriptor& descriptor);
		//! @brief 法線テクスチャを未設定にし, フラグを下ろす
		void SetNormalTexture(std::nullopt_t);
		//! @brief 法線テクスチャを設定する (optionalが無効なら未設定として扱う)
		//! @param[in] descriptor テクスチャのディスクリプタ (無効値可)
		void SetNormalTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor);

		//! @brief ラフネステクスチャのindexを設定し, 対応するフラグを立てる
		//! @param[in] descriptor テクスチャのディスクリプタ
		void SetRoughnessTexture(const DxObject::ReferenceDescriptor& descriptor);
		//! @brief ラフネステクスチャを未設定にし, フラグを下ろす
		void SetRoughnessTexture(std::nullopt_t);
		//! @brief ラフネステクスチャを設定する (optionalが無効なら未設定として扱う)
		//! @param[in] descriptor テクスチャのディスクリプタ (無効値可)
		void SetRoughnessTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor);

		//! @brief メタリックテクスチャのindexを設定し, 対応するフラグを立てる
		//! @param[in] descriptor テクスチャのディスクリプタ
		void SetMetallicTexture(const DxObject::ReferenceDescriptor& descriptor);
		//! @brief メタリックテクスチャを未設定にし, フラグを下ろす
		void SetMetallicTexture(std::nullopt_t);
		//! @brief メタリックテクスチャを設定する (optionalが無効なら未設定として扱う)
		//! @param[in] descriptor テクスチャのディスクリプタ (無効値可)
		void SetMetallicTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor);

		//! @brief エミッシブテクスチャのindexを設定し, 対応するフラグを立てる
		//! @param[in] descriptor テクスチャのディスクリプタ
		void SetEmissiveTexture(const DxObject::ReferenceDescriptor& descriptor);
		//! @brief エミッシブテクスチャを未設定にし, フラグを下ろす
		void SetEmissiveTexture(std::nullopt_t);
		//! @brief エミッシブテクスチャを設定する (optionalが無効なら未設定として扱う)
		//! @param[in] descriptor テクスチャのディスクリプタ (無効値可)
		void SetEmissiveTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor);

		//* transformation option *//

		//! @brief UV変換行列を設定する
		//! @param[in] mat UVに適用する変換行列
		void SetTransformation(const Matrix4x4& mat) { transformation.mat = mat; }

		//=========================================================================================
		// public variables
		//=========================================================================================

		UVTransformation transformation;
		Albedo albedo;
		Property roughness;
		Property metallic;
		Transparency transparency;
		Normal normal;
		Emissive emissive;

		Sxl::Flag<TextureFlag> flags = TextureFlag::None;

		};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	AssetMaterial(const Uuid& id) : BaseAsset(id) { CreateBuffer(); }
	AssetMaterial(std::nullopt_t) : BaseAsset(std::nullopt) { CreateBuffer(); }

	~AssetMaterial() override = default;

	//* setup option *//

	//! @brief assimpのaiMaterialからマテリアルパラメータを読み込む
	//! @param[in] material  読み込み元のassimpマテリアル
	//! @param[in] directory テクスチャ解決の基準となるディレクトリ
	void Setup(const aiMaterial* material, const std::filesystem::path& directory); //!< assimp用setup

	//! @brief jsonからマテリアルパラメータを読み込む
	//! @param[in] data 読み込み元のjsonデータ
	void Setup(const json& data); //!< json用setup

	//* material option *//

	//! @brief 保持しているテクスチャindex等を反映し, GPUバッファを最新状態に更新する
	void Update();

	//! @brief マテリアル定数バッファのGPU仮想アドレスを取得する
	//! @return 定数バッファのGPU仮想アドレス
	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return buffer_.GetGPUVirtualAddress(); }

	//! @brief マテリアルバッファ本体を取得する
	//! @return MaterialBufferへの参照
	MaterialBuffer& GetBuffer() { return buffer_.At(); }
	//! @brief マテリアルバッファ本体を取得する (const版)
	//! @return MaterialBufferへのconst参照
	const MaterialBuffer& GetBuffer() const { return buffer_.At(); }

	//* operator [copy] *//

	//! @brief パラメータをコピーする (バッファは各インスタンス固有のため内容のみ複製する)
	//! @param[in] other コピー元マテリアル
	//! @return 自身への参照
	AssetMaterial& operator=(const AssetMaterial& other);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* buffer *//

	DxObject::ConstantBuffer<MaterialBuffer> buffer_;

	//* texture parameter *//

	std::array<std::optional<Uuid>, static_cast<uint8_t>(Texture::Emissive) + 1> textures_;

	//* value parameter *//

	Transform2d transform_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void CreateBuffer();

	//* setup helper methods *//

	static std::optional<Uuid> GetTextureId(const aiMaterial* aiMaterial, aiTextureType type, const std::filesystem::path& directory, bool isIntensity = false);

	static Transform2d GetTransform2d(const aiMaterial* aiMaterial);

	static std::optional<Uuid> GetTextureId(const json& data, bool isIntensity = false);

	//* update helper methods *//

	std::optional<DxObject::ReferenceDescriptor> GetTextureDescriptor(Texture texture) const;

};

SXAVENGER_ENGINE_NAMESPACE_END
