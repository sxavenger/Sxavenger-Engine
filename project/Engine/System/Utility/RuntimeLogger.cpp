#include "RuntimeLogger.h"
SXAVENGER_ENGINE_USING

////////////////////////////////////////////////////////////////////////////////////////////
// RuntimeLogger class methods
////////////////////////////////////////////////////////////////////////////////////////////

RuntimeLogger::Data::Data(Type _type, const std::string& _category, const std::string& _label)
	: type(_type), category(_category), label(_label) {
	count = 1;
	Timestamp();
}

void RuntimeLogger::Data::Timestamp() {
	auto current = std::chrono::system_clock::now();
	timestamp = std::chrono::zoned_time{
		std::chrono::current_zone(),
		std::chrono::floor<std::chrono::seconds>(current)
	};
}

bool RuntimeLogger::Data::Compare(const Data& data) const {
	return type == data.type && category == data.category && label == data.label;
}

////////////////////////////////////////////////////////////////////////////////////////////
// RuntimeLogger class methods
////////////////////////////////////////////////////////////////////////////////////////////

void RuntimeLogger::Log(Type type, const std::string& category, const std::string& label) {
	RuntimeLogger::Push({ type, category, label });
}

void RuntimeLogger::LogComment(const std::string& category, const std::string& label) {
	RuntimeLogger::Log(Type::Comment, category, label);
}

void RuntimeLogger::LogWarning(const std::string& category, const std::string& label) {
	RuntimeLogger::Log(Type::Warning, category, label);
}

void RuntimeLogger::LogError(const std::string& category, const std::string& label) {
	RuntimeLogger::Log(Type::Error, category, label);
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
