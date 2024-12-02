#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/Content/Texture/MultiViewTexture.h>

////////////////////////////////////////////////////////////////////////////////////////////
// VisualProcessFrame class
////////////////////////////////////////////////////////////////////////////////////////////
class VisualProcessFrame {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	VisualProcessFrame()  = default;
	~VisualProcessFrame() { Term(); }

	void Create(const Vector2ui& size);

	void Term();

	//* visual option *//

	void TransitionBeginProcess(const DirectXThreadContext* context);

	void TransitonEndProcess(const DirectXThreadContext* context);

	//* index option *//

	void NextResultBufferIndex();

	void ResetResultBufferIndex();

	//* getter *//

	MultiViewTexture* GetResultBuffer() const { return buffers_.at(resultBufferIndex_).get(); }
	

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* parameter *//

	static const uint8_t kProcessBufferNum_ = 2;
	uint8_t resultBufferIndex_              = NULL;

	//* buffer *//

	std::array<std::unique_ptr<MultiViewTexture>, kProcessBufferNum_> buffers_;

private:
	static_assert(kProcessBufferNum_ >= 2, "Process Texture must be at least 2.");
	//!< process bufferは2つ以上で作成する
};
