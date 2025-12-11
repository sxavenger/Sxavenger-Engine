#include "ParticleElement.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////////////////
// Albedo structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ParticleElement::Albedo::Init() {
	type   = Type::Value;
	albedo = kWhite3<float>;
	index  = NULL;
}

void ParticleElement::Albedo::SetValue(const Color3f& _albedo) {
	type   = Type::Value;
	albedo = _albedo;
}

void ParticleElement::Albedo::SetTexture(uint32_t _index, const Color3f& _albedo) {
	type   = Type::Texture;
	index  = _index;
	albedo = _albedo;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Transparent structure methods
////////////////////////////////////////////////////////////////////////////////////////////

void ParticleElement::Transparent::Init() {
	type        = Type::Value;
	transparent = 1.0f;
	index       = NULL;
}

void ParticleElement::Transparent::SetValue(float _transparent) {
	type        = Type::Value;
	transparent = _transparent;
}

void ParticleElement::Transparent::SetTexture(uint32_t _index, float _transparent) {
	type        = Type::Texture;
	index       = _index;
	transparent = _transparent;
}

////////////////////////////////////////////////////////////////////////////////////////////
// ParticleElement sturcture methods
////////////////////////////////////////////////////////////////////////////////////////////

void ParticleElement::Init() {
	albedo.Init();
	transparent.Init();

	isBillboard = true;
}
