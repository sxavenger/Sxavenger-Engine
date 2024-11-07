#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// ProcessLayerType enum class
////////////////////////////////////////////////////////////////////////////////////////////
enum class ProcessLayerType {
	kProcessLayer_None,
	kProcessLayer_Glayscale,
};

////////////////////////////////////////////////////////////////////////////////////////////
// BaseProcessLayer class
////////////////////////////////////////////////////////////////////////////////////////////
class BaseProcessLayer {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	BaseProcessLayer()  = default;
	~BaseProcessLayer() { Term(); }

	void Init();

	void Term();

	//* console option *//

	void SetToConsole();
	void SetToConsole(const std::string& name);

	//* attribute option *//

	virtual void SetLayerAttributeImGui();

	//* layer getter *//

	const std::string& GetName() const { return name_; }

	ProcessLayerType GetType() const { return type_; }

protected:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::string name_      = "new layer";
	ProcessLayerType type_ = ProcessLayerType::kProcessLayer_None;

	// todo: node機能の実装
	
};