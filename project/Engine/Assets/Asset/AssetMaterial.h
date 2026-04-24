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
#include <Lib/Geometry/Color3.h>
#include <Lib/Geometry/Matrix4x4.h>
#include <Lib/Sxl/Flag.h>
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
// Asset namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace Asset {

	////////////////////////////////////////////////////////////////////////////////////////////
	// Material class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Material final
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

			Color3f value  = kWhite3<float>;
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

			float value    = 1.0f;
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

			float value    = 0.0f;
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

			Color3f value   = kBlack3<float>;
			uint32_t index  = NULL;
			float intensity = 1.0f;

		};

		////////////////////////////////////////////////////////////////////////////////////////////
		// MaterialBuffer structure
		////////////////////////////////////////////////////////////////////////////////////////////
		struct MaterialBuffer {
		public:

			////////////////////////////////////////////////////////////////////////////////////////////
			// TextureFlags enum class
			////////////////////////////////////////////////////////////////////////////////////////////
			enum class TextureFlags : uint32_t {
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

			Sxl::Flag<TextureFlags> flags = TextureFlags::None;

		};

	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor / destructor *//

		Material(const Uuid& id) : BaseAsset(id) { CreateBuffer(); }
		Material(std::nullopt_t) : BaseAsset(std::nullopt) { CreateBuffer(); }

		~Material() override = default;

		//* setup option *//

		void Setup(const aiMaterial* material, const std::filesystem::path& directory); //!< assimp用setup

		void Setup(const json& data); //!< json用setup

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

		static std::optional<Uuid> GetTextureId(const json& data, Texture texture, bool isIntensity = false);

	};

}

SXAVENGER_ENGINE_NAMESPACE_END
