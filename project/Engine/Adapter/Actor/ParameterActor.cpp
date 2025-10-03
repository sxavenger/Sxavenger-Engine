#include "ParameterActor.h"

////////////////////////////////////////////////////////////////////////////////////////////
// SerializeParameter structure methods
////////////////////////////////////////////////////////////////////////////////////////////

ParameterActor::ParameterActor() {
	MonoBehaviour::SetName("Parameter Actor");
}

void ParameterActor::Inspectable() {
	SerializeGuiFormatter<float>::SliderScalar(param1_, -2.0f, 4.0f);
	SerializeGuiFormatter<float>::DragScalar2(param2_, 0.1f, 0.0f, std::nullopt);
	SerializeGuiFormatter<int32_t>::DragScalar3(param3_, 1.0f, -20, std::nullopt);

}
