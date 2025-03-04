#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* layer
#include "../FSceneTextures.h"
#include "Layer/LLayer.h"

//* engine
#include <Engine/System/DirectX/DirectXContext.h>

//* c++
#include <list>

////////////////////////////////////////////////////////////////////////////////////////////
// FCanvas class
////////////////////////////////////////////////////////////////////////////////////////////
class FCanvas {
	//* canvasに描画されるUIの管理クラス
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	FCanvas()  = default;
	~FCanvas() = default;

	//* render *//

	void Render(const DirectXThreadContext* context);

	bool CheckRender() const;

	//* setter *//

	void AddLayer(LLayer* layer) { layers_.emplace_back(layer); }

	void SetTextures(FSceneTextures* textures) { textures_ = textures; }

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* layers *//

	std::list<LLayer*> layers_;

	//* view info *//

	FSceneTextures* textures_ = nullptr;

};
