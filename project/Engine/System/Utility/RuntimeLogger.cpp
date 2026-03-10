#include "RuntimeLogger.h"
SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// RuntimeLogger class methods
////////////////////////////////////////////////////////////////////////////////////////////

RuntimeLogger::Data::Data(Level _level , const std::string& _category, const std::string& _label)
	: level(_level), category(_category), label(_label) {
	count = 1;
	Timestamp();
}

void RuntimeLogger::Data::Timestamp() {
	timestamp = LocalTimePoint::Now();
}

bool RuntimeLogger::Data::Compare(const Data& data) const {
	return level == data.level && category == data.category && label == data.label;
}

////////////////////////////////////////////////////////////////////////////////////////////
// RuntimeLogger class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RuntimeLogger::Log(Level level, const std::string& category, const std::string& label) {
	RuntimeLogger::Push({ level, category, label });
}

void RuntimeLogger::Push(const Data& data) {

	if (!logs_.empty()) {
		Data& front = logs_.front();

		if (front.Compare(data)) { //!< 先頭と比較
			front.count++;
			front.Timestamp();
			return;
		}
	}

	//!< 新規追加
	logs_.emplace_front(data);

	//!< 件数を制限
	auto itr = std::next(logs_.begin(), std::min<size_t>(limit_, logs_.size()));
	logs_.erase(itr, logs_.end());
}
