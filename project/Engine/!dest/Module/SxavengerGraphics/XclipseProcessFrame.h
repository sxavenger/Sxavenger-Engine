#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/Content/TextureBuffer/MultiViewTextureBuffer.h>

////////////////////////////////////////////////////////////////////////////////////////////
// XclipseProcessFrame class
////////////////////////////////////////////////////////////////////////////////////////////
class XclipseProcessFrame {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	XclipseProcessFrame() = default;
	~XclipseProcessFrame() { Term(); }

	void Create(const Vector2ui& size);

	void Term();

	//* xclipse option *//

	void TransitionBeginProcess(const DirectXThreadContext* context);

	void TransitionEndProcess(const DirectXThreadContext* context);

	//* index option *//

	void NextResultBufferIndex();

	void ResetResultBufferIndex();
	
	//* getter *//

	MultiViewTextureBuffer* GetResultBuffer() const { return buffers_.at(resultBufferIndex_).get(); }

	MultiViewTextureBuffer* GetPrevBuffer(uint32_t prev = 1) const;

private:


	//=========================================================================================
	// public methods
	//=========================================================================================

	//* parameter *//

	static const uint8_t kProcessBufferNum_ = 2;
	uint8_t resultBufferIndex_ = NULL;

	//* buffer *//

	std::array<std::unique_ptr<MultiViewTextureBuffer>, kProcessBufferNum_> buffers_;

private:
	static_assert(kProcessBufferNum_ >= 2, "Process Texture must be at least 2.");
	//!< process bufferは2つ以上で作成する
};
