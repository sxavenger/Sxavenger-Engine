#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DirectXContext.h>
#include <Engine/Content/Texture/MultiViewTexture.h>

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

	//* getter *//

	const MultiViewTexture* GetTexture() const { return buffer_.get(); }

private:


	//=========================================================================================
	// public methods
	//=========================================================================================

	std::unique_ptr<MultiViewTexture> buffer_;

};