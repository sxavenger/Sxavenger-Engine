#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Entity/MonoBehaviour.h>

////////////////////////////////////////////////////////////////////////////////////////////
// GameObject class
////////////////////////////////////////////////////////////////////////////////////////////
class GameObject
	: public MonoBehaviour {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	virtual void Load() {} //!< Assetのロード

	virtual void Awake() {} //!< 生成時1度だけ呼ばれる

	virtual void Start() {} //!< 初期化処理

	virtual void Update() {} //!< 更新処理

private:
};
