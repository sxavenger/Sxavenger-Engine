#pragma once

//-----------------------------------------------------------------------------------------
// include
//-----------------------------------------------------------------------------------------
//* c++
#include <cstdint>
#include <chrono>
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////
// LocalTimePoint class
////////////////////////////////////////////////////////////////////////////////////////////
class LocalTimePoint {
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Date structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Date {
	public:

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		Date() = default;

		//* operator / constructor [assign] *//

		Date& operator=(const std::chrono::year_month_day& ymd);
		Date(const std::chrono::year_month_day& ymd) { *this = ymd; }

		//* operator [compare] *//

		bool operator==(const Date& rhs) const;

		//* serialize / deserialize *//

		std::string Serialize();

		static Date Deserialize(const std::string& s);

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint16_t year;  //!< [0000-9999] 年
		uint8_t  month; //!< [01-12] 月
		uint8_t  day;   //!< [01-31] 日

	};

	////////////////////////////////////////////////////////////////////////////////////////////
	// Time structure
	////////////////////////////////////////////////////////////////////////////////////////////
	struct Time {

		//=========================================================================================
		// public methods
		//=========================================================================================

		//* constructor *//

		Time() = default;

		//* operator / constructor [assign] *//

		Time& operator=(const std::chrono::hh_mm_ss<std::chrono::seconds>& hms);
		Time(const std::chrono::hh_mm_ss<std::chrono::seconds>& hms) { *this = hms; }

		//* operator [compare] *//

		bool operator==(const Time& rhs) const;

		//* serialize / deserialize *//

		std::string Serialize();

		static Time Deserialize(const std::string& s);

		//=========================================================================================
		// public variables
		//=========================================================================================

		uint8_t hour;   //!< [00-23] 時
		uint8_t minute; //!< [00-59] 分
		uint8_t second; //!< [00-59] 秒

	};

public:

	//=========================================================================================
	// public methods
	//=========================================================================================

	//* constructor *//

	LocalTimePoint() = default;

	//* operator / constructor [assign] *//

	LocalTimePoint& operator=(const std::chrono::zoned_time<std::chrono::seconds>& time);
	LocalTimePoint(const std::chrono::zoned_time<std::chrono::seconds>& time) { *this = time; }

	LocalTimePoint& operator=(const LocalTimePoint&) = default;
	LocalTimePoint(const LocalTimePoint&)            = default;

	//* operator [compare] *//

	bool operator==(const LocalTimePoint& rhs) const;

	//* serialize / deserialize *//

	std::string Serialize();

	static LocalTimePoint Deserialize(const std::string& s);

	//* getter *//

	const Date& GetDate() const { return date_; }
	const Time& GetTime() const { return time_; }

	//* helper methods *//

	static LocalTimePoint Now(
		const std::chrono::time_zone* zone = std::chrono::current_zone()
	);

	static LocalTimePoint Convert(
		const std::chrono::system_clock::time_point& time,
		const std::chrono::time_zone* zone = std::chrono::current_zone()
	);

private:

	//=========================================================================================
	// private variables
	//=========================================================================================

	Date date_; //!< 日付
	Time time_; //!< 時刻

};
