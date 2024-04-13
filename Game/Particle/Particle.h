#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// Particle class
////////////////////////////////////////////////////////////////////////////////////////////
class Particle {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	Particle() {}

	~Particle() {}

	virtual void Init() = 0;

	virtual void Update() = 0;

	virtual void Draw() = 0;

	virtual void Term() = 0;

protected:
private:
};