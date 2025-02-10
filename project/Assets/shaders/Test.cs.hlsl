#include "MaterialComponent.hlsli"

//=========================================================================================
// buffers
//=========================================================================================

struct A {
	float a;
};

struct B {
	float b;
};

struct Parameter {
	A a;
	B b;
};
ConstantBuffer<Parameter> gParameter : register(b0);

RWTexture2D<float4> gOutput : register(u0);

////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////
[numthreads(1, 1, 1)]
void main() {
	float a = gParameter.a.a;
	float b = gParameter.b.b;

	gOutput[uint2(0, 0)] = float4(a, b, 0.0f, 1.0f);

}
