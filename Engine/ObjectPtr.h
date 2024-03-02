#pragma once

////////////////////////////////////////////////////////////////////////////////////////////
// object_ptr class
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
class Object_Ptr {
public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//! @brief コンストラクタ
	Object_Ptr() : ptr_(nullptr) {}

	//! @brief デストラクタ
	~Object_Ptr() { Release(); }

	//! @brief ptrを解放
	void Release() {
		delete ptr_;
		ptr_ = nullptr;
	}

	//! @brief ポインターを取得
	//! 
	//! @return ポインターを返却
	T* Get() const { return ptr_; }

	//=========================================================================================
	// operator
	//=========================================================================================

	void operator=(T* other) {
		ptr_ = other;
	}
	void operator=(Object_Ptr<T>& other) {
		ptr_ = other.ptr_;
		other.ptr_ = nullptr;
	}

	T* operator->() const {
		return ptr_;
	}

	Object_Ptr(T* other) {
		ptr_ = other;
	}

	Object_Ptr(const Object_Ptr&) = delete;
	Object_Ptr& operator=(const Object_Ptr&) = delete;

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	T* ptr_;

};
