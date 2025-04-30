```c++
// init.cs.hlsl

RWStructuredBuffer<uint> gCounter : register(u0); //!< >

[numthreads(1, 1, 1)]
void main() {
	gCounter[0] = 0;
}

```