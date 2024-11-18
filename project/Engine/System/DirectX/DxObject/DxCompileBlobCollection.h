#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* DXOBJECT
#include "DxObjectCommon.h"
#include "DxShaderCompiler.h"

//* lib
#include <Lib/Sxl/LowerUnorderedMap.h>

//* c++
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// DXOBJECT
////////////////////////////////////////////////////////////////////////////////////////////
_DXOBJECT_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////////////////
// CompileBlobCollection class
////////////////////////////////////////////////////////////////////////////////////////////
class CompileBlobCollection {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	CompileBlobCollection()  = default;
	~CompileBlobCollection() { Term(); };

	void Init(ShaderCompiler* compiler);

	void Term();

	std::weak_ptr<ComPtr<IDxcBlob>> TryCreateBlob(const std::wstring& filename, CompileProfile profile);

	void HotReload(const std::wstring& filename);

	std::weak_ptr<ComPtr<IDxcBlob>> GetBlob(const std::wstring& filename);

	//* setter *//

	void SetCompiler(ShaderCompiler* compiler) { compiler_ = compiler; }

private:

	////////////////////////////////////////////////////////////////////////////////////////////
	// CompileBlob structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct CompileBlob {
		std::shared_ptr<ComPtr<IDxcBlob>> blob;
		CompileProfile                    profile;
	};

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	ShaderCompiler* compiler_ = nullptr;

	//* collection *//

	Sxl::LowerUnorderedMapW<CompileBlob> blobs_;
	//!< main, profile統一

};

_DXOBJECT_NAMESPACE_END