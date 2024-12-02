#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* console
#include "RenderBehavior.h"

//* engine
#include <Engine/Module/Behavior/BaseBehavior.h>
#include <Engine/Module/Pipeline/RenderPipelineCollection.h>
#include <Engine/Module/Pipeline/ComputePipelineCollection.h>
#include <Engine/Module/SxavengerGraphics/SxavGraphicsFrame.h>

//* c++
#include <optional>

//-----------------------------------------------------------------------------------------
// forward
//-----------------------------------------------------------------------------------------
class Console;

////////////////////////////////////////////////////////////////////////////////////////////
// RenderConsole class
////////////////////////////////////////////////////////////////////////////////////////////
class RenderConsole {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	RenderConsole()  = default;
	~RenderConsole() = default;

	void Init(Console* console);

	void Term();

	void UpdateConsole();

	void Draw();

	//* behavior container option *//

	BehaviorIterator SetBehavior(BaseBehavior* behavior);

	void EraseBehavior(const BehaviorIterator& iterator);

	void ResetBehavior();

	void RemoveAttributeBehavior(const BehaviorIterator& iterator);

	void ResetAttributeBehavior();

	void RemoveUniqueBehavior();

	//* pipeline option *//

	void SetGraphicsPipeline(RenderPipelineType type, const DirectXThreadContext* context, const Vector2ui& size);

	void BindGraphicsBuffer(RenderPipelineType type, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc);

	void SetComputePipeline(ComputePipelineType type, const DirectXThreadContext* context);

	void BindComputeBuffer(ComputePipelineType type, const DirectXThreadContext* context, const DxObject::BindBufferDesc& desc);

	void DispatchCompute(const DirectXThreadContext* context, const Vector2ui& size);

protected:

	//=========================================================================================
	// protected variables
	//=========================================================================================

	bool isDisplayRenderConsole_ = true;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	//* menu *//

	void ShowRenderConsoleMenu();
	void ShowBehaviorMenu();

	//* sub *//

	void CreateSceneFrame(const Vector2ui& size);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	//* external *//

	Console* console_ = nullptr;

	//* behavior *//

	BehaviorContainer outliner_;

	std::optional<BehaviorIterator> attributeIterator_ = std::nullopt; //!< 現在選択されているitretor
	const BehaviorContainer*        attributeTable_    = nullptr;      //!< 選択されているiteratorのコンテナ

	//* render behavior *//

	std::list<std::unique_ptr<RenderBehavior>> behaviors_;

	//* pipeline *// HACK:...

	std::unique_ptr<RenderPipelineCollection> renderPipeline_;
	std::unique_ptr<ComputePipelineCollection> computePipeline_;

	//* frames *//

	std::unique_ptr<SxavGraphicsFrame> scene_;

	//=========================================================================================
	// private methods
	//=========================================================================================

	//* window *//

	void DisplayOutliner();
	void DisplayAttribute();

	//* behavior methods *//

	bool IsSelectedBehavior(BaseBehavior* behavior);
	void SelectableBehavior(const BehaviorContainer& container);

	void UpdateUniqueRemove();

	//* sub methods *//

	static void MenuDummy();

};