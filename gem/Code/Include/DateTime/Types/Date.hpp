/* Copyright 2022 Matteo Grasso
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <AzCore/RTTI/ReflectContext.h>
#include <AzCore/RTTI/TypeInfoSimple.h>
#include <AzCore/std/string/string.h>
#include <AzCore/base.h>

#include "GregorianCalendar.hpp"
#include "Time.hpp"


namespace Loherangrin::Gems::DateTime
{
	enum class DateChange : AZ::u8
	{
		NONE = 0,
		DAY,
		MONTH,
		YEAR
	};

	using DateComparison = TimeComparison;
	using DateDifference = AZ::s32;

	class Date
	{
	public:
		AZ_TYPE_INFO(Date, "{29E845C9-96FA-46B2-9755-8367F56D4CF3}");
		static void Reflect(AZ::ReflectContext* io_context);

		using Calendar = GregorianCalendar;

		using Year = Calendar::Year;
		using Month = Calendar::Month;
		using Day = Calendar::Day;
		using Week = Calendar::Week;

		using Change = DateChange;
		using Comparison = DateComparison;
		using Difference = DateDifference;

		Date();
		Date(TimeS i_seconds, Year i_epoch = Calendar::EPOCHS_UNIX);
		Date(Year i_year, Month i_month, Day i_day);
		virtual ~Date() = default;

		bool IsValid() const;

		Day GetDayNumber() const;
		AZStd::string GetMonthName() const;
		Month GetMonthNumber() const;
		Week GetWeek() const;
		Year GetYear() const;

		void SetDay(Day i_day);
		void SetMonth(Month i_month);
		void SetYear(Year i_year);

		DateChange AddDay();
		DateChange AddMonth();
		DateChange AddYear();

		DateChange AddDays(Day i_value);
		DateChange AddMonths(Month i_value);
		DateChange AddYears(Year i_value);

		DateChange SubtractDay();
		DateChange SubtractMonth();
		DateChange SubtractYear();

		DateChange SubtractDays(Day i_value);
		DateChange SubtractMonths(Month i_value);
		DateChange SubtractYears(Year i_value);

		TimeS ToSeconds(Year i_epoch = Calendar::EPOCHS_UNIX) const;
		void FromSeconds(TimeS i_value, TimeS& o_remainder, Year i_epoch = Calendar::EPOCHS_UNIX);
		DateChange MoveBySeconds(TimeS i_offset, TimeS& o_remainder);

		DateDifference CountDaysTo(const Date& i_other) const;
		DateDifference CountMonthsTo(const Date& i_other) const;
		DateDifference CountYearsTo(const Date& i_other) const;

		AZStd::string Format(const AZStd::string& i_pattern) const;
		AZStd::string ToString() const;
		
		DateComparison CompareTo(const Date& i_other) const;

		Date operator+(TimeS i_seconds) const;
		Date operator-(TimeS i_seconds) const;

		DurationS operator-(const Date& i_rhs) const;

		Date& operator+=(TimeS i_seconds);
		Date& operator-=(TimeS i_seconds);

		Date& operator++();
		Date operator++(int);
		Date& operator--();
		Date operator--(int);

		bool operator==(const Date& i_rhs) const;
		bool operator<(const Date& i_rhs) const;
		bool operator<=(const Date& i_rhs) const;
		bool operator>(const Date& i_rhs) const;
		bool operator>=(const Date& i_rhs) const;

		static constexpr TimeS SHORT_MONTH_TO_SECONDS = Calendar::DAYS_IN_SHORT_MONTH * Time::DAY_TO_SECONDS;
		static constexpr TimeS COMMON_YEAR_TO_SECONDS = Calendar::DAYS_IN_COMMON_YEAR * Time::DAY_TO_SECONDS;
		static constexpr TimeS LEAP_YEAR_TO_SECONDS = Calendar::DAYS_IN_LEAP_YEAR * Time::DAY_TO_SECONDS;

	private:
		DateChange AddMonth(bool i_dayValidation);
		DateChange AddYear(bool i_dayValidation);

		DateChange AddMonths(Month i_value, bool i_dayValidation);
		DateChange AddYears(Year i_value, bool i_dayValidation);

		DateChange SubtractMonth(bool i_dayValidation);
		DateChange SubtractYear(bool i_dayValidation);

		DateChange SubtractMonths(Month i_value, bool i_dayValidation);
		DateChange SubtractYears(Year i_value, bool i_dayValidation);

		void ValidateDay();

		Year m_year;
		Month m_month;
		Day m_day;
	};

} // Loherangrin::Gems::DateTime
