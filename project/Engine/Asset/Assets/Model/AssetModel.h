#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* asset
#include "../BaseAsset.h"
#include "Model.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// Asset Model class
//////////////////////////////////////////////////////////////////////////////////////////////
class AssetModel
	: public BaseAsset, public Model {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	AssetModel()  = default;
	~AssetModel() = default;

	void Load(const DirectXThreadContext* context) override;

	AsyncExecution GetAsyncExecution() const override { return AsyncExecution::Compute; }

	void ShowInspector() override;

private:
};
