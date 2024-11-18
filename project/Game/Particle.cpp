#include "Particle.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/Game/SxavengerGame.h>

//* lib
#include <Lib/Adapter/Random.h>
#include <Lib/Easing.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Particle class methods
////////////////////////////////////////////////////////////////////////////////////////////

void Particle::Init(const Vector3f& position, const Vector3f& velocity, const Curve* curve) {
	ModelInstanceBehavior::renderingFlag_ = kBehaviorRender_Adaptive;
	ModelInstanceBehavior::model_         = SxavengerGame::LoadModel("resources/model", "enemy.obj");
	SetName("particle");

	elements_.resize(kInstanceCount_);

	for (size_t i = 0; i < kInstanceCount_; ++i) {
		elements_[i].lifeTime.time       = lifeTime_.time;
		elements_[i].transform.scale     = { 0.2f, 0.2f, 0.2f };
		elements_[i].transform.translate = position;

		elements_[i].velocity = { Random::Generate(-0.2f, 0.2f), Random::Generate(-0.2f, 0.2f), Random::Generate(-0.2f, 0.2f) };
		velocity;
	}

	curve_ = curve;

	CreateInstance(kInstanceCount_);
}

void Particle::Term() {
}

void Particle::Update() {
	if (current_.time >= lifeTime_.time) {
		isDelete_ = true;
		return;
	}

 	current_.AddDeltaTime();

	for (uint32_t i = 0; i < kInstanceCount_; ++i) {

		float t = curve_->GetOutput(std::min(current_.time / elements_[i].lifeTime.time, 1.0f));
		float scale = std::lerp(0.04f, 0.0f, t);
		elements_[i].transform.scale = { scale, scale, scale };
		elements_[i].transform.rotate *= MakeAxisAngle({0.0f, 1.0f, 0.0f}, Random::Generate(-0.01f, 0.01f));
		elements_[i].transform.translate += elements_[i].velocity * Performance::GetDeltaTime(s).time;

		(*instanceBuffer_)[i].Transfer(elements_[i].transform.ToMatrix());
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
// ParticleCollection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void ParticleCollection::Init() {
	SetName("particle collection");

	curve_ = std::make_unique<Curve>();
	curve_->LoadJson("curve.json");
}

void ParticleCollection::Update() {

	particles_.remove_if([](const std::unique_ptr<Particle>& p) {
		return p->IsDelete();
	});

	for (auto& particle : particles_) {
		particle->Update();
	}
}

void ParticleCollection::Term() {
	particles_.clear();
}

void ParticleCollection::CreateParticle(const Vector3f& position, const Vector3f& velocity) {

	std::unique_ptr<Particle> particle = std::make_unique<Particle>();
	particle->Init(position, velocity, curve_.get());

	SetChild(particle.get());

	particles_.emplace_back(std::move(particle));
}

void ParticleCollection::SetAttributeImGui() {
	curve_->SetImGuiCommand();
}

