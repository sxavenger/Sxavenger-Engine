#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// concept
////////////////////////////////////////////////////////////////////////////////////////////
class IScene;

//!< ISceneに継承してるクラスのみのtemplate
template <class T>
concept DerivedFromIScene = std::is_base_of_v<IScene, T>;

////////////////////////////////////////////////////////////////////////////////////////////
// Scene Interfase class
////////////////////////////////////////////////////////////////////////////////////////////
class IScene {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	IScene()          = default;
	virtual ~IScene() = default;

	virtual void Init() = 0;

	virtual void Term() = 0;

	virtual void Update() = 0;

	virtual void Draw() = 0;

	//* scene manager option *//

	bool IsRequsetedNextScene() const { return nextScene_ != nullptr; }

	std::unique_ptr<IScene>&& GetNextScene() { return std::move(nextScene_); }

protected:

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* scene option *//

	template <DerivedFromIScene T>
	void SetNextScene();

private:

	//=========================================================================================
	// private varaibles
	//=========================================================================================

	//* scene member *//

	std::unique_ptr<IScene> nextScene_ = nullptr;

	
};

////////////////////////////////////////////////////////////////////////////////////////////
// IScene class template methods
////////////////////////////////////////////////////////////////////////////////////////////

template<DerivedFromIScene T>
inline void IScene::SetNextScene() {
	nextScene_ = std::make_unique<T>();
}