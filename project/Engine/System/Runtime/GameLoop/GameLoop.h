#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Runtime/Framework/IFramework.h>

//* c++
#include <cstdint>
#include <functional>
#include <array>
#include <optional>
#include <map>
#include <list>
#include <concepts>
#include <deque>
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////
// GameLoop namespace
////////////////////////////////////////////////////////////////////////////////////////////
namespace GameLoop {

	////////////////////////////////////////////////////////////////////////////////////////////
	// State enum class
	////////////////////////////////////////////////////////////////////////////////////////////
	enum class State : uint8_t { //!< 実行順
		Init,
		Begin,
		Update,
		Draw,
		End,
		Term, //!< reverse process order.
	};
	constexpr uint8_t kStateCount = static_cast<uint8_t>(State::Term) + 1;

	////////////////////////////////////////////////////////////////////////////////////////////
	// State Function using
	////////////////////////////////////////////////////////////////////////////////////////////

	using StateFunc     = std::function<void()>;
	using ConditionFunc = std::function<bool()>; //!< true: break loop

	////////////////////////////////////////////////////////////////////////////////////////////
	// State Array using
	////////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	using StateArray = std::array<T, kStateCount>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Context class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Context : public IFramework {
	public:

		//=========================================================================================
		// public method
		//=========================================================================================

		Context()                   = default;
		virtual ~Context() override = default;

		virtual void Run() override;

		void SetState(State state, const std::optional<uint32_t>& index, const StateFunc& function);

		void SetCondition(const ConditionFunc& func) { conditionFuncs_.emplace_back(func); }

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		StateArray<std::map<uint32_t, StateFunc>> stateFuncs_;
		std::list<ConditionFunc>                  conditionFuncs_;

		//=========================================================================================
		// private method
		//=========================================================================================

		uint32_t FindMinIndex(State state) const;

		void RegisterState(State state, uint32_t index, const StateFunc& function);

		void Execute(State state, bool isReverse = false);

		void Loop();

		void Clear();

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Interface class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Interface {
	public:

		//=========================================================================================
		// public method
		//=========================================================================================

		virtual ~Interface() = default;

		virtual void Init(Context* context) = 0;

		virtual void Term() = 0;

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Interface concept
	////////////////////////////////////////////////////////////////////////////////////////////
	template <class T>
	concept InterfaceConcept = std::derived_from<T, Interface>;

	////////////////////////////////////////////////////////////////////////////////////////////
	// Collection class
	////////////////////////////////////////////////////////////////////////////////////////////
	class Collection : public Context {
	public:

		//=========================================================================================
		// public method
		//=========================================================================================

		template <InterfaceConcept T>
		void Push();

		void Run() override;

	private:

		//=========================================================================================
		// private variables
		//=========================================================================================

		std::deque<std::shared_ptr<Interface>> interfaces_;
		// todo: vector<unique_ptr<>>に変更

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Collection class template method
	////////////////////////////////////////////////////////////////////////////////////////////

	template <InterfaceConcept T>
	inline void Collection::Push() {
		auto& ptr = interfaces_.emplace_back(std::make_shared<T>());
		ptr->Init(this);
	}

}
