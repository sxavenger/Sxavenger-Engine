# Sxavenger Engine
## Branch: Mesh

### Engine
`DxObject`  
DirectX3D関係のクラス

> DxObjectMethods   
    用途を絞った関数    
    `namespace DxObjectMethods`

> DxSystem   
    device, commandなどの根幹部分のクラス

> DxSource   
    BufferResource, StructuredBufferなどのsourceとして使うクラス

### TODO:
- meshletを使った描画
- ImGui::viewport機能をSRGBで使えるように改造
- Assimpの実装

### FIXME:
- StrucutredBufferをBufferResourceを所有せずID3D12Resourceを所有させて独立させる
```c++
// DxObject
class StructuredBuffer{
private:

    - ComPtr<ID3D12Resource> res...
    - Descriptor

};
```

