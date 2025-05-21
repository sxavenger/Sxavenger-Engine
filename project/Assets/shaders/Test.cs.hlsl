
struct A {
	float a;
};
ConstantBuffer<A> gA : register(b0);

cbuffer B : register(b1) {
	float b;
};

RWTexture2D<float4> gOut : register(u0);

[numthreads(1, 1, 1)]
void main() {

	gOut[uint2(1, 1)] = float4(gA.a, b, 0, 1);
	
}
