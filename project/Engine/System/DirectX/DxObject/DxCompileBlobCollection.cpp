#include "DxCompileBlobCollection.h"
_DXOBJECT_USING

////////////////////////////////////////////////////////////////////////////////////////////
// CompileBlobCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void CompileBlobCollection::Init(ShaderCompiler* compiler) {
	SetCompiler(compiler);
}

void CompileBlobCollection::Term() {
}

std::weak_ptr<ComPtr<IDxcBlob>> CompileBlobCollection::TryCreateBlob(const std::wstring& filename, CompileProfile profile) {

	if (!blobs_.Contains(filename)) {
		std::shared_ptr<ComPtr<IDxcBlob>> blob = std::make_shared<ComPtr<IDxcBlob>>(compiler_->Compile(filename, profile, L"main"));

		blobs_.Emplace(filename, { std::move(blob), profile});
	}

	return blobs_.At(filename).blob;
}

void CompileBlobCollection::HotReload(const std::wstring& filename) {
	if (!blobs_.Contains(filename)) {
		return; //!< filenameのblobが見つからなかったため
	}

	blobs_[filename].blob = std::make_shared<ComPtr<IDxcBlob>>(compiler_->Compile(filename, blobs_.At(filename).profile, L"main"));
}

std::weak_ptr<ComPtr<IDxcBlob>> CompileBlobCollection::GetBlob(const std::wstring& filename) {
	AssertW(blobs_.Contains(filename), L"Compile Blob not found. filename: " + filename);
	return blobs_.At(filename).blob;
}
