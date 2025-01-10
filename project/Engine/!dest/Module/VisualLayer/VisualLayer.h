#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/DirectX/DxObject/DxDimensionBuffer.h>
#include <Engine/Module/SxavengerGraphics/SxavGraphicsFrame.h>
#include <Engine/Module/Camera/Camera3D.h>

//* lib
#include <Lib/CXXAttributeConfig.h>

//* c++
#include <string>
#include <memory>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class BaseVisualLayer;

////////////////////////////////////////////////////////////////////////////////////////////
// using
////////////////////////////////////////////////////////////////////////////////////////////
using VisualCanvas   = std::list<BaseVisualLayer*>;
using VisualIterator = VisualCanvas::const_iterator;

////////////////////////////////////////////////////////////////////////////////////////////
// BaseVisualLayer class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseVisualLayer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseVisualLayer()  = default;
	virtual ~BaseVisualLayer() { Term(); }

	virtual void Init() = 0;

	void Term();

	//* interface *//

	virtual void Process(_MAYBE_UNUSED const DirectXThreadContext* context, _MAYBE_UNUSED SxavGraphicsFrame* frame) = 0;

	virtual void SetLayerImGui() = 0;

	//* cosole option *//

	void SetToConsole();
	void SetToConsole(const std::string& name);

	void ResetIterator();

	//* getter *//

	const std::string& GetName() const { return name_; }

	//* setter *//

	void SetName(const std::string& name) { name_ = name; }

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::string name_ = "new base layer";

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::optional<VisualIterator> iterator_ = std::nullopt;

};