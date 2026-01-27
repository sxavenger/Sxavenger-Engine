# レンダリング - features
## 名称の変更
- FDeferredGBuffer
- FLightingBuffer
- FMainBuffer
// G-Bufferの名称の修正。

## Reflectionの追加
### Deferred G-Bufferの拡張
``` c++
// Deferred GBuffer
enum class Layout {
	Albedo, //!< [DXGI_FORMAT_RGBA16(MainFormat基準)] RGB: albedo
	Normal, //!< [DXGI_FORMAT_RGB10A2] RGB: normal(mapping済み)
	MaterialARM, //!< [DXGI_FORMAT_RGB10A2] R: ao, G: roughness, B: metallic
	Position, //!< [DXGI_FORMAT_RGBA16] XYZ: world position
	Velocity, //!< [DXGI_FORMAT_...]
	Reflection //!< [DXGI_FORMAT_RGBA16(MainFormat基準)] RGB: Reflection Color(Albedo / Lit Color)
};
```

### Reflection計算方法
BXDFに対しreflect()でのlとしてSpecular成分として結果を加算
```c++
float3 direction = reflect(Surface.normal, Camera.Direction);
BXDF specular = BXDF::Create(direction);

return specular * NdotL * Reflection.Color;

// blended Src + Dst
// 正確にいうとSpecularの集合率から解きたい。
```

## Primitive Static Meshの埋め込み
- Cube (読み込みの際の仮meshとして使用)

## Global Illumination - features
### DDGIの作成
早めに手を動かす

### ReSTIRの修正
Spatio部分の修正

