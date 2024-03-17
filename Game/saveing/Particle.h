#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
// c++
#include <list>

// object
#include <ObjectStructure.h>

////////////////////////////////////////////////////////////////////////////////////////////
// Element structure
////////////////////////////////////////////////////////////////////////////////////////////
struct Element {
	Transform transform;
	Vector3f  velocity;
	Vector4f  color;

	float currentTime = 0.0f;
	float lifeTime;
};


////////////////////////////////////////////////////////////////////////////////////////////
// Particle base class
////////////////////////////////////////////////////////////////////////////////////////////
class Particle {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	Particle() {}

	//! @brief デストラクタ
	~Particle() { TermBase(); }

	//! @brief 初期化処理
	virtual void Init() = 0;

	//! @brief 更新処理
	virtual void Update() = 0;

	//! @brief 描画処理
	virtual void Draw() = 0;

	//! @brief 終了処理
	virtual void Term() = 0;

	//! @brief elementsにelementを設定
	//! 
	//! @param[in] element 設定するelement
	virtual void SetElements(const Element& element);

	//! @brief elementsを取得
	//! 
	//! @return elementsを返却
	std::list<Element>& GetList() { return elements_; }

protected:

	////////////////////////////////////////////////////////////////////////////////////////////
	// BillBoardType enum
	////////////////////////////////////////////////////////////////////////////////////////////
	enum BillBoardType {
		kBillBoard_NONE,
		kBillBoard_2D,
		kBillBoard_3D,
		
		kCountOfBillBoardType
	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// ParticleForGPU structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct ParticleForGPU {
		Matrix4x4 wvp;
		Matrix4x4 world;
		Vector4f  color;
	};

	//=========================================================================================
	// protected variables
	//=========================================================================================

	std::list<Element> elements_;

	BillBoardType billBoardType_ = BillBoardType::kBillBoard_NONE;

	//=========================================================================================
	// protected methods
	//=========================================================================================

	void SetOnImGuiBillBoardType();

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	static const char* billBoardTypeStr_[BillBoardType::kCountOfBillBoardType];

	//=========================================================================================
	// private methods
	//=========================================================================================

	void TermBase();

};