#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Component/Entity/MonoBehaviour.h>
#include <Engine/Component/Components/Transform/RectTransformComponent.h>
#include <Engine/Component/Components/TextRenderer/TextRendererComponent.h>

////////////////////////////////////////////////////////////////////////////////////////////
// PerformanceActor class
////////////////////////////////////////////////////////////////////////////////////////////
//! @brief PerformanceのUI表示用クラス
class PerformanceActor
	: public MonoBehaviour {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	void Init(const Vector2f& position = { 8.0f, 8.0f });

	void Update();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	RectTransformComponent* transform_ = nullptr;
	TextRendererComponent* text_       = nullptr;

};
