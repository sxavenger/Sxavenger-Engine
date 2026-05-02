#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* content
#include "BaseContent.h"

//* engine
#include <Engine/Foundation.h>
#include <Engine/System/DirectX/Context/DirectXQueueContext.h>

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* DirectX12
#include <d3dx12.h>
#include <DirectXTex.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Sxavenger Engine namespace
////////////////////////////////////////////////////////////////////////////////////////////
SXAVENGER_ENGINE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// ContentTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class ContentTexture final
	: public BaseContent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Encoding enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Encoding : bool {
		Lightness, //!< sRGB
		Intensity, //!< Linear
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Option structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Option {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Encoding encoding = Encoding::Lightness;
		//!< 読み込み時のエンコード形式の指定. LightnessはsRGB形式(Albedo etc...), IntensityはLinear形式で読み込む.(Normal etc...)

		bool isGenerateMipmap = true;
		bool useCompress      = true; //!< 圧縮されたテクスチャを使用するかどうか. 圧縮されたテクスチャが存在しない場合は、通常のテクスチャを使用する.

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor / destructor *//

	ContentTexture() : BaseContent(Async::Execution::Copy) {}

	~ContentTexture() override = default;

	//* content option *//

	void Attach(const std::filesystem::path& filepath, const std::any& parameter) override;

	void Load(MAYBE_UNUSED const DirectXQueueContext* context) override;

	//* id option *//

	const Uuid& GetId() const { return id_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Uuid id_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	static std::string GetEncoding(Encoding encoding);

	void AttachUuid(const std::filesystem::path& filepath);

	//* load helper methods *//

	Option GetOption();

	DirectX::ScratchImage LoadContent(const DirectXQueueContext* context, const std::filesystem::path& filepath, const Option& option);

	//* texture load helper methods *//

	static Encoding GetFormatEncoding(DXGI_FORMAT format) { return DirectX::IsSRGB(format) ? Encoding::Lightness : Encoding::Intensity; }

	static DirectX::ScratchImage LoadFromDDSFile(const std::filesystem::path& filepath, const Option& option);
	static DirectX::ScratchImage LoadFromHDRFile(const std::filesystem::path& filepath, const Option& option);
	static DirectX::ScratchImage LoadFromTGAFile(const std::filesystem::path& filepath, const Option& option);
	static DirectX::ScratchImage LoadFromWICFile(const std::filesystem::path& filepath, const Option& option);

	static DirectX::ScratchImage LoadTexture(const std::filesystem::path& filepath, const Option& option);

	//* compress helper methods *//

	bool ExistsCompressed(const std::filesystem::path& filepath) const;

	bool CheckCompress(const std::filesystem::path& filepath) const;

	static std::filesystem::path GetCompressedPath(const std::filesystem::path& filepath);

	static void Compress(const std::filesystem::path& filepath, const Option& option);

};

SXAVENGER_ENGINE_NAMESPACE_END
