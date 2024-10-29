#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXROBJECT
#include <Engine/System/DxrObject/DxrObjectCommon.h>
#include <Engine/System/DxrObject/DxrExportGroup.h>

//* DirectX12
#include <dxcapi.h>

//* c++
#include <string>
#include <unordered_set>

////////////////////////////////////////////////////////////////////////////////////////////
// DXROBJECT namespace
////////////////////////////////////////////////////////////////////////////////////////////
_DXROBJECT_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class RaytracingShaderBlobManager;

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingShaderBlob class
////////////////////////////////////////////////////////////////////////////////////////////
class RaytracingShaderBlob {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RaytracingShaderBlob()  = default;
	~RaytracingShaderBlob() { Term(); }

	void Create(const std::wstring& filename);

	void Term();

	//* export optional *//

	void SetExportGroup(const ExportGroup* exportGroup);

	//* getter *//

	IDxcBlob* GetBlob() const { return blob_; }

	const std::unordered_set<const ExportGroup*>& GetExports() const { return exports_; }

	//* setter *//

	static void SetRaytracingShaderBlobManager(RaytracingShaderBlobManager* manager);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* externals *//

	static RaytracingShaderBlobManager* manager_;

	//* blob *//

	IDxcBlob* blob_ = nullptr;

	//* export container *//

	std::unordered_set<const ExportGroup*> exports_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// RaytracingShaderBlobGroup class
////////////////////////////////////////////////////////////////////////////////////////////
class RaytracingShaderBlobGroup {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RaytracingShaderBlobGroup()  = default;
	~RaytracingShaderBlobGroup() { Term(); }

	void Create();

	void Term();

	//* container option *//

	void SetRaytracingShaderBlob(const RaytracingShaderBlob* blob);

	//* getter *//

	const std::unordered_set<const RaytracingShaderBlob*>& GetBlobs() const { return blobs_; }

private:

	//=========================================================================================
	// private varinables
	//=========================================================================================

	std::unordered_set<const RaytracingShaderBlob*> blobs_;

};

_DXROBJECT_NAMESPACE_END