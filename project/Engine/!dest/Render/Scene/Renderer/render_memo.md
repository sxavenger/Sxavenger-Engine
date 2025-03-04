# Deferred, Forwardの各パスの比較
## 目的
FBaseRendererにおいての基底クラスにてFSceneなどの抽象化が不可能なのではないかと疑問に思ったのでまとめてみる。

## 現(Deferred)の構造
```c++
std::list<FGeometryActor*> A; //!< 描画されるobject
std::list<FLightActor*>    B; //!< Sceneに配置されているLight(Spot, Point, Directional...)
```

## Deferred
+ 各shader passが抽象化できていて調整がしやすい
+ Lightなど規約さえ守れば自作してLightを作れる


## Forward
- shader passが一つで完結しすぎて調整が効きにくい
