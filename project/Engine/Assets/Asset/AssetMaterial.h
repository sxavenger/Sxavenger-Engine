#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "BaseAsset.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>

//* lib
#include <Lib/Geometry/Color3.h>
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Transform/Transform.h>
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
	// Mode enum class 
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Mode : uint8_t {
		Opaque,      //!< 不透明
		Translucent, //!< 半透明
		Emissive
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// TextureType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class TextureType : uint8_t {
		Albedo,
		Bump,
		AmbientOcclusion,
		Roughness,
		Metallic,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Albedo structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Albedo {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Type enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Type : uint32_t {
			Value,
			Texture,
			Multiply,
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		//* option *//

		void SetValue(const Color3f& _color);

		void SetTexture(uint32_t _index);

		void SetValueMultiply(const Color3f& _color);

		void SetTextureMultiply(uint32_t _index);

		//* debug *//

		void SetImGuiCommand();

		//=========================================================================================
		// public variables
		//=========================================================================================

		Type type = Type::Value;

		Color3f color = kWhite3<float>;
		uint32_t index = NULL;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Transparency structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Transparency {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Type enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Type : uint32_t {
			Value,
			Texture,
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		//* option *//

		void SetValue(float _alpha);

		void SetTexture(uint32_t _index);

		//* debug *//

		void SetImGuiCommand();

		//=========================================================================================
		// public variables
		//=========================================================================================

		Type type = Type::Value;

		float alpha    = 1.0f;
		uint32_t index = NULL;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Normal structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Normal {
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Type enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Type : uint32_t {
			None,
			Texture,
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		//* option *//

		void SetNone();

		void SetTexture(uint32_t _index);

		//* debug *//

		void SetImGuiCommand();

		//=========================================================================================
		// public variables
		//=========================================================================================

		Type type = Type::None;

		uint32_t index = NULL;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Property structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Property { //!< helper struct
	public:

		////////////////////////////////////////////////////////////////////////////////////////////
		// Type enum class
		////////////////////////////////////////////////////////////////////////////////////////////
		enum class Type : uint32_t {
			Value,
			Texture,
		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		//* option *//

		void SetValue(float _value);

		void SetTexture(uint32_t _index);

		//* debug *//

		void SetImGuiCommand();

		//=========================================================================================
		// public methods
		//=========================================================================================

		Type type = Type::Value;

		float value    = 0.0f;
		uint32_t index = NULL;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// SurfaceProperties structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct SurfaceProperties {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		//* debug *//

		void SetImGuiCommand();

		//=========================================================================================
		// public variables
		//=========================================================================================

		Property ao;
		Property roughness;
		Property metallic;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// UVTransformation structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct UVTransformation {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		void Transfer(const Matrix4x4& _mat);

		//=========================================================================================
		// public variables
		//=========================================================================================

		Matrix4x4 mat;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// MaterialBuffer structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct MaterialBuffer {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		void Init();

		//=========================================================================================
		// public variables
		//=========================================================================================

		UVTransformation  transformation;
		Albedo            albedo;
		Transparency      transparency;
		Normal            normal;
		SurfaceProperties properties;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetMaterial(const Uuid& id) : BaseAsset(id) { CreateBuffer(); }
	AssetMaterial(std::nullopt_t) : BaseAsset(std::nullopt) { CreateBuffer(); }
	~AssetMaterial() override = default;

	void Setup(const aiMaterial* material, const std::filesystem::path& directory);

	void Update();

	void Copy(const AssetMaterial& material);

	void Wait();

	//* inspector *//

	void ShowInspector() override;

	//* option *//

	void SetMode(Mode mode) { mode_ = mode; }

	//* getter *//

	const D3D12_GPU_VIRTUAL_ADDRESS& GetGPUVirtualAddress() const;

	Mode GetMode() const { return mode_; }

	const MaterialBuffer& GetBuffer() const;
	MaterialBuffer& GetBuffer();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* texture parameter *//

	std::array<std::optional<Uuid>, static_cast<uint8_t>(TextureType::Metallic) + 1> textures_;

	//* value parameter *//

	Color3f color_   = kWhite3<>;
	float roughness_ = 1.0f;
	float metallic_  = 1.0f;

	Transform2d transform_; //!< 2D transform for UV mapping

	//* mode *//

	Mode mode_ = Mode::Opaque;

	//* buffer *//

	std::unique_ptr<DxObject::DimensionBuffer<MaterialBuffer>> buffer_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper methods *//

	static std::optional<Uuid> GetTextureId(const aiMaterial* aiMaterial, aiTextureType type, const std::filesystem::path& directory, bool isIntensity = false);

	void CreateBuffer();

};

SXAVENGER_ENGINE_NAMESPACE_END
