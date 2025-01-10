#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Asset/AssetCollection.h>

//* c++
#include <optional>
#include <array>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Console;

////////////////////////////////////////////////////////////////////////////////////////////
// SystemConsole class
////////////////////////////////////////////////////////////////////////////////////////////
class AssetConsole {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(Console* console);

	void Term();

	void UpdateConsole();

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isDisplayAssetConsole_ = true;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* menu *//

	void ShowAssetMenu();

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// TextureType enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum TextureType : uint32_t {
		//!< file
		File,
		File_Texture,
		File_ModelAnimator,
		File_Shader,

		//!< folder
		Folder,
		Folder_Open,
		Folder_Empty,
		Folder_Empty_Open,
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	Console* console_ = nullptr;

	//* node *//

	std::optional<AssetCollection::FolderPair*> folder_ = std::nullopt;
	std::optional<AssetCollection::File*>       file_   = std::nullopt;

	//* texture *//

	std::array<std::shared_ptr<Texture>, TextureType::Folder_Empty_Open + 1> textures_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* assister *//

	static TextureType GetTextureType(const AssetCollection::File& file);

	void InitTexture();

	//* window *//

	void DisplayProject();

	void ShowFolderTree();
	void ShowFolderProject();

	void DisplayInspector();

	//* sub methods *//

	static void MenuDummy();

};
