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
class AssetMaterial final
	: public BaseAsset {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Texture enum class
	////////////////////////////////////////////////////////////////////////////////////////////
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
	struct MaterialBuffer {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// TextureFlag enum class
		////////////////////////////////////////////////////////////////////////////////////////////
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

		void Init();

		//* texture option *//

		void SetAlbedoTexture(const DxObject::ReferenceDescriptor& descriptor);
		void SetAlbedoTexture(std::nullopt_t);
		void SetAlbedoTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor);

		void SetTransparencyTexture(const DxObject::ReferenceDescriptor& descriptor);
		void SetTransparencyTexture(std::nullopt_t);
		void SetTransparencyTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor);

		void SetNormalTexture(const DxObject::ReferenceDescriptor& descriptor);
		void SetNormalTexture(std::nullopt_t);
		void SetNormalTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor);

		void SetRoughnessTexture(const DxObject::ReferenceDescriptor& descriptor);
		void SetRoughnessTexture(std::nullopt_t);
		void SetRoughnessTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor);

		void SetMetallicTexture(const DxObject::ReferenceDescriptor& descriptor);
		void SetMetallicTexture(std::nullopt_t);
		void SetMetallicTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor);

		void SetEmissiveTexture(const DxObject::ReferenceDescriptor& descriptor);
		void SetEmissiveTexture(std::nullopt_t);
		void SetEmissiveTextureOptional(const std::optional<DxObject::ReferenceDescriptor>& descriptor);

		//* transformation option *//

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

	void Setup(const aiMaterial* material, const std::filesystem::path& directory); //!< assimp用setup

	void Setup(const json& data); //!< json用setup

	//* material option *//

	void Update();

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return buffer_.GetGPUVirtualAddress(); }

	MaterialBuffer& GetBuffer() { return buffer_.At(); }
	const MaterialBuffer& GetBuffer() const { return buffer_.At(); }

	//* operator [copy] *//

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
