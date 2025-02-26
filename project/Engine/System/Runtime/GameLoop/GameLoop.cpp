#include "GameLoop.h"

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* engine
#include <Engine/System/Utility/Logger.h>

//* c++
#include <algorithm>
#include <format>
#include <ranges>

////////////////////////////////////////////////////////////////////////////////////////////
// Context class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameLoop::Context::Run() {

	Log(std::format("[GameLoop::Context]: Begin Loop. ptr: {:p}", reinterpret_cast<void*>(this)));
	Loop();
	Log(std::format("[GameLoop::Context]: End Loop. ptr: {:p}", reinterpret_cast<void*>(this)));

	Clear();
}

void GameLoop::Context::SetState(State state, const std::optional<uint32_t>& _index, const StateFunc& function) {

	uint32_t index = 0;

	if (_index.has_value()) {
		index = _index.value();

	} else if (!stateFuncs_[static_cast<uint8_t>(state)].empty()) {
		index = FindMinIndex(state);
	}

	RegisterState(state, index, function);
}

uint32_t GameLoop::Context::FindMinIndex(State state) const {
	if (stateFuncs_[static_cast<uint8_t>(state)].empty()) {
		return 0;
	}

	uint32_t expected = 0;
	for (const auto& element : stateFuncs_[static_cast<uint8_t>(state)]) {

		if (element.first != expected) {
			return expected; // 連続していない最初の要素を返す
		}

		++expected;
	}

	return expected; // すべて連続している場合は次の要素を返す
}

void GameLoop::Context::RegisterState(State state, uint32_t index, const StateFunc& function) {
	Assert(!stateFuncs_[static_cast<uint8_t>(state)].contains(index), "state functions is layer conflict."); //!< 重複していないか確認
	stateFuncs_[static_cast<uint8_t>(state)].emplace(index, function);
}

void GameLoop::Context::Execute(State state, bool isReverse) {
	if (stateFuncs_[static_cast<uint8_t>(state)].empty()) {
		return;
	}

	if (isReverse) {
		for (const auto& func : stateFuncs_[static_cast<uint8_t>(state)] | std::views::reverse) {
			func.second();
		}

	} else {
		for (const auto& func : stateFuncs_[static_cast<uint8_t>(state)]) {
			func.second();
		}
	}
}

void GameLoop::Context::Loop() {

	if (conditionFuncs_.empty()) {
		return;
	}

	Execute(State::Init);

	while (true) {
		Execute(State::Begin);
		Execute(State::Update);
		Execute(State::Draw);
		Execute(State::End);

		if (std::any_of(conditionFuncs_.begin(), conditionFuncs_.end(), [](const auto& func) { return func(); })) {
			break;
		}
	}

	Execute(State::Term, true);
}

void GameLoop::Context::Clear() {
	for (uint8_t i = 0; i < kStateCount; ++i) {
		stateFuncs_[i].clear();
	}

	conditionFuncs_.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////
// Collection class methods
////////////////////////////////////////////////////////////////////////////////////////////

void GameLoop::Collection::Run() {
	Context::Run();

	while (!interfaces_.empty()) {
		interfaces_.back()->Term();
		interfaces_.pop_back();
	}
}
