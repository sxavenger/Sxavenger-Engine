#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxrObjectCommon
#include <DxrObjectCommon.h>

//* DxrObject
#include <DxrExportGroup.h>

//* DirectX12
#include <dxcapi.h>

//* c++
#include <list>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "dxcompiler.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxrObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class ShaderBlobManager;

////////////////////////////////////////////////////////////////////////////////////////////
// ShaderBlob class
////////////////////////////////////////////////////////////////////////////////////////////
class ShaderBlob {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	ShaderBlob() = default;

	void Create(const std::wstring& filename);

	void SetEntryPointExport(const ExportGroup* exportGroup);

	//* getter *//

	IDxcBlob* GetBlob() const { return blob_; }

	const std::list<const ExportGroup*> GetExports() const { return exports_; }

	//* external *//

	static void SetShaderBlobManager(ShaderBlobManager* manager) { manager_ = manager; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* externals *//

	static ShaderBlobManager* manager_;

	//* blob *//

	IDxcBlob* blob_ = nullptr;

	//* shader exports *//

	//! [list]
	//! value: blobに書かれているEntryPoint情報
	std::list<const ExportGroup*> exports_;

};

_DXROBJECT_NAMESPACE_END