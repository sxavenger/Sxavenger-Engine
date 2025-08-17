#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* ucontent
#include "UBaseContent.h"

//* lib
#include <Lib/Adapter/Uuid/Uuid.h>

//* directx12
#include <d3dx12.h>
#include <DirectXTex.h>

//* c++
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////////////////
// UContentTexture class
////////////////////////////////////////////////////////////////////////////////////////////
class UContentTexture final
	: public UBaseContent {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Encoding enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class Encoding : bool {
		Lightness, //!< sRGB
		Intensity,
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Option structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Option {
	public:

		//=========================================================================================
		// public variables
		//=========================================================================================

		Encoding encoding     = Encoding::Lightness;
		bool isGenerateMipmap = true;

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	UContentTexture()           = default;
	~UContentTexture() override = default;

	void AsyncLoad(_MAYBE_UNUSED const DirectXQueueContext* context) override;

	AsyncExecution GetAsyncExecution() const { return AsyncExecution::Compute; }

	//* content option *//

	void Load(const DirectXQueueContext* context, const std::filesystem::path& filepath, const Option& option);

	const Uuid& GetId() const { return id_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================
	
	Uuid id_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* helper methods *//

	void GetUuid();

	//* texture load helper methods *//

	static Encoding GetFormatEncoding(DXGI_FORMAT format) { return DirectX::IsSRGB(format) ? Encoding::Lightness : Encoding::Intensity; }

	static DirectX::ScratchImage LoadFromDDSFile(const std::filesystem::path& filepath, const Option& option);
	static DirectX::ScratchImage LoadFromHDRFile(const std::filesystem::path& filepath, const Option& option);
	static DirectX::ScratchImage LoadFromTGAFile(const std::filesystem::path& filepath, const Option& option);
	static DirectX::ScratchImage LoadFromWICFile(const std::filesystem::path& filepath, const Option& option);

	static DirectX::ScratchImage LoadTexture(const std::filesystem::path& filepath, const Option& option);

};
