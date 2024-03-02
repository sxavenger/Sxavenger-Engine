# (MyEngine) - CG2 ver 2024/3/3
## オリジナル追加要素
- BlinnPhong反射の追加  
    sprite以外のすべてにBlinnPhong反射の追加。  
    Objectの追加してからMaterial欄にチェックボックスで適用されます。  
    ONにするとspecPowのパラメーターがいじれます。  

- DescriptorHeapの改善  
    - 動的にDescriptorを消した際のシステム  
        動的にDescriptorを消したとき, Descriptorのindexを保存させており新しくDescriptorを作る場合は空いてるとこから優先的に作られます。
    
    - 同じテクスチャで重複してDescriptorが作られないようなシステム  
        同じファイルパスのテクスチャのDescriptorを重複させないようにしました。

    主にSRVのDescriptorで使ってます。  
    -> ImGuiの`[DxObject]:DescriptorHeaps - debacker`で現在空いてるDrescriptorのindexが確認できます。

- OBJでの読み込み関係
  - indexBufferViewの使用
        StanderdBunnyをcreateするととても重かったので作りました。今でも重いです。  
        StanderdBunnyはImGuiにvertex数, index数, face数をデバックとして表示させてあります。

  - f が4面でも対応  
        対応させました。  
        Planeは四面の.objを新しく作って表示させてます。  
        (plane2.obj .mtl)

- 自動リソースマンの追加
    ```c++
    delete ptr
    ptr = nullptr;
    ```
    と書くのがめんどくさかったので作りました。

### あとがき
2月中に提出できませんでした。申し訳ないです。  
今回はVS-PSで作りました。早めにMS導入します。gomennasai...  
pipelineの設定がやりにくかったり, MyEngine::がいたるところにはびこってたり, commandListにセットしにくかったりが多くあるのでいい感じにしていこうと思います。
