//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* base
#include <Engine/System/Runtime/GameLoop/GameLoop.h>

//* engine
#include <Engine/System/Window/GameWindow.h>
#include <Engine/Asset/AssetCollection.h>

//* test !render
#include <Engine/System/DirectX/DxrObject/DxrAccelerationStructure.h>
#include <Engine/System/DirectX/DxrObject/DxrStateObjectContext.h>
#include <Engine/Content/TextureBuffer/UnorderedTexture.h>

////////////////////////////////////////////////////////////////////////////////////////////
// BetaSystemGameLoop class
////////////////////////////////////////////////////////////////////////////////////////////
class BetaSystemGameLoop
	: public GameLoop::Interface {
public:

	//=========================================================================================
	// public method
	//=========================================================================================

	void Init(GameLoop::Context* context) override;

	void Term() override;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	std::shared_ptr<GameWindow> main_;

	std::unique_ptr<DxrObject::RaytracingBlob> raygeneration_;
	std::unique_ptr<DxrObject::ExportGroup>    raygenerationExpt_;

	std::unique_ptr<DxrObject::RaytracingBlob> miss_;
	std::unique_ptr<DxrObject::ExportGroup>    missExpt_;

	std::shared_ptr<AssetModel> model_;
	std::unique_ptr<DxrObject::TopLevelAS> topLevelAS_;

	std::unique_ptr<DxrObject::StateObjectContext> stateObject_;

	std::unique_ptr<UnorderedTexture> texture_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	void InitSystem();

	void TermSystem();

	void UpdateSystem();

	void DrawSystem();

};
