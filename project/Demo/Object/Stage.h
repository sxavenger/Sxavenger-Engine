#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Asset/AssetStorage.h>
#include <Engine/Asset/Observer/AssetObserver.h>
#include <Engine/Module/GameObject/GameObject.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Stage class
////////////////////////////////////////////////////////////////////////////////////////////
class Stage
	: public GameObject {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Load() override;

	void Awake() override;

	void Start() override;

private:

	//=========================================================================================
	// private methods
	//=========================================================================================

	AssetObserver<AssetModel> stageModel_;
	AssetObserver<AssetModel> chessboardModel_;

	std::unique_ptr<MonoBehaviour> stage_;
	std::unique_ptr<MonoBehaviour> chessboard_;

};
