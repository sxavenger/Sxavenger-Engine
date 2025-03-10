#include "SkinnedMeshRendererComponent.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* component
#include "../../MonoBehaviour.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SkinnedMeshRendererComponent class methods
////////////////////////////////////////////////////////////////////////////////////////////

const TransformComponent* SkinnedMeshRendererComponent::GetTransform() const {
	return BaseComponent::GetBehaviour()->RequireComponent<TransformComponent>();
}
