#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DxObjectCommon
#include <DxObjectCommon.h>

//* DirectX12
#include <dxcapi.h>

//* c++
#include <array>

//-----------------------------------------------------------------------------------------
// comment
//-----------------------------------------------------------------------------------------
#pragma comment(lib, "dxcompiler.lib")

////////////////////////////////////////////////////////////////////////////////////////////
// DxObject
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class ShaderBlobManager;

////////////////////////////////////////////////////////////////////////////////////////////
// BaseShaderBlob base class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseShaderBlob {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseShaderBlob()          = default;
	virtual ~BaseShaderBlob() = default;

	static void SetShaderBlobManager(ShaderBlobManager* manager) { manager_ = manager; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	//* external *//

	static ShaderBlobManager* manager_;

};

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicShaderType enum
////////////////////////////////////////////////////////////////////////////////////////////
enum GraphicShaderType { //!< DxShaderManagerのShaderTypeと合わせること
	//* vertex pipeline *//
	GRAPHICS_VERTEX,
	GRAPHICS_GEOMETRY,

	//* mesh pipeline *//
	GRAPHICS_MESH,
	GRAPHICS_AMPLIFICATION,

	GRAPHICS_PIXEL,

	kCountOfGraphicShaderType
};

////////////////////////////////////////////////////////////////////////////////////////////
// GraphicsBlob class
////////////////////////////////////////////////////////////////////////////////////////////
class GraphicsBlob
	: public BaseShaderBlob {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	GraphicsBlob() = default;
	~GraphicsBlob() { Term(); }

	void Create(const std::wstring& filename, GraphicShaderType type);

	void Term();

	void Merge(GraphicsBlob* other);

	//* Getter *//

	const std::array<IDxcBlob*, GraphicShaderType::kCountOfGraphicShaderType>& GetGraphicsBlobs() const { return graphicsBlobs_; }

	bool IsUseMeshPipeline() const { return isUseMeshPipeline_; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* graphics blob container *//

	std::array<IDxcBlob*, GraphicShaderType::kCountOfGraphicShaderType> graphicsBlobs_ = { nullptr };

	//* parameter *//

	bool isUseMeshPipeline_ = false; //!< mesh shader, amplification shaderを使う際はpipelineが変わるのでtrueになる。

};

////////////////////////////////////////////////////////////////////////////////////////////
// CSBlob class
////////////////////////////////////////////////////////////////////////////////////////////
class CSBlob
	: public BaseShaderBlob {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CSBlob() = default;
	~CSBlob() { Term(); }

	void Create(const std::wstring& filename);

	void Term();

	//* getter *//

	IDxcBlob* const GetBlob() const { return blob_; }

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	IDxcBlob* blob_ = nullptr;

};

_DXOBJECT_NAMESPACE_END