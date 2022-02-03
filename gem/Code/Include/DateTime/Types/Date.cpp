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

#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/std/limits.h>

#include <DateTime/Constants/DateTimeConstants.hpp>
#include <DateTime/Utils/DateTimeMath.hpp>

#include "Date.hpp"

using Loherangrin::Gems::DateTime::Date;
using Loherangrin::Gems::DateTime::DateChange;
using Loherangrin::Gems::DateTime::DateComparison;
using Loherangrin::Gems::DateTime::DateDifference;
using Loherangrin::Gems::DateTime::DurationS;
using Loherangrin::Gems::DateTime::TimeS;


void Date::Reflect(AZ::ReflectContext* io_context)
{
	if(auto serializeContext = azrtti_cast<AZ::SerializeContext*>(io_context))
	{
		serializeContext->Class<Date>()
			->Version(0)
			->Field("Year", &Date::m_year)
			->Field("Month", &Date::m_month)
			->Field("Day", &Date::m_day)
			;
	}

	if(auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(io_context))
	{
		behaviorContext
			->Enum<(AZ::u8) DateChange::NONE>("DateChange_None")
			->Enum<(AZ::u8) DateChange::DAY>("DateChange_Day")
			->Enum<(AZ::u8) DateChange::MONTH>("DateChange_Month")
			->Enum<(AZ::u8) DateChange::YEAR>("DateChange_Year")
			->Enum<(AZ::u8) DateComparison::LESS_THAN>("DateComparison_LessThan")
			->Enum<(AZ::u8) DateComparison::EQUAL>("DateComparison_Equal")
			->Enum<(AZ::u8) DateComparison::GREATER_THAN>("DateComparison_GreaterThan")
			;

		behaviorContext->Class<Date>("Date")
			->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
			->Attribute(AZ::Script::Attributes::Module, ScriptAttributes::MODULE)
			->Attribute(AZ::Script::Attributes::Category, ScriptAttributes::CATEGORY)

			->Constructor()
			->Constructor<TimeS>()
			->Constructor<TimeS, Year>()
			->Constructor<Year, Month, Day>()

			->Property("DayNumber", &Date::GetDayNumber, &Date::SetDay)
			->Property("MonthName", &Date::GetMonthName, nullptr)
			->Property("MonthNumber", &Date::GetMonthNumber, &Date::SetMonth)
			->Property("Week", &Date::GetWeek, nullptr)
			->Property("Year", &Date::GetYear, &Date::SetYear)

			->Method("IsValid", &Date::IsValid)

			->Method<DateChange (Date::*)()>("AddDay", &Date::AddDay)
			->Method<DateChange (Date::*)()>("AddMonth", &Date::AddMonth)
			->Method<DateChange (Date::*)()>("AddYear", &Date::AddYear)

			->Method<DateChange (Date::*)(Date::Day)>("AddDays", &Date::AddDays)
			->Method<DateChange (Date::*)(Date::Month)>("AddMonths", &Date::AddMonths)
			->Method<DateChange (Date::*)(Date::Year)>("AddYears", &Date::AddYears)

			->Method<DateChange (Date::*)()>("SubtractDay", &Date::SubtractDay)
			->Method<DateChange (Date::*)()>("SubtractMonth", &Date::SubtractMonth)
			->Method<DateChange (Date::*)()>("SubtractYear", &Date::SubtractYear)

			->Method<DateChange (Date::*)(Date::Day)>("SubtractDays", &Date::SubtractDays)
			->Method<DateChange (Date::*)(Date::Month)>("SubtractMonths", &Date::SubtractMonths)
			->Method<DateChange (Date::*)(Date::Year)>("SubtractYears", &Date::SubtractYears)

			->Method("ToSeconds", &Date::ToSeconds)
			->Method("FromSeconds", [](Date& io_instance, TimeS i_value, Date::Year i_year) -> TimeS
				{
					TimeS remainder;
					io_instance.FromSeconds(i_value, remainder, i_year);

					return remainder;
				})
			->Method("MoveBySeconds", [](Date& io_instance, TimeS i_offset) -> AZStd::pair<TimeS, int>
				{
					TimeS remainder;
					DateChange dateChange = io_instance.MoveBySeconds(i_offset, remainder);

					return AZStd::make_pair(remainder, static_cast<int>(dateChange));
				})

			->Method("CountDaysTo", &Date::CountDaysTo)
			->Method("CountMonthsTo", &Date::CountMonthsTo)
			->Method("CountYearsTo", &Date::CountYearsTo)

			->Method("Format", &Date::Format)
			->Method("ToString", &Date::ToString)
				->Attribute(AZ::Script::Attributes::Operator, AZ::Script::Attributes::OperatorType::ToString)
		
			->Method("CompareTo", &Date::CompareTo)

			->Method("GetLastMonthDay", &Calendar::GetLastMonthDay)

			->Method("Equal", &Date::operator==)
				->Attribute(AZ::Script::Attributes::Operator, AZ::Script::Attributes::OperatorType::Equal)
				->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::All)

			->Method("LessThan", &Date::operator<)
				->Attribute(AZ::Script::Attributes::Operator, AZ::Script::Attributes::OperatorType::LessThan)
				->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::All)

			->Method("LessEqualThan", &Date::operator<=)
				->Attribute(AZ::Script::Attributes::Operator, AZ::Script::Attributes::OperatorType::LessEqualThan)
				->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::All)
		;
	}
}

Date::Date()
	: m_year { 0 }
	, m_month { AZStd::numeric_limits<Month>::max() }
	, m_day { AZStd::numeric_limits<Day>::max() }
{}

Date::Date(Year i_year, Month i_month, Day i_day)
	: m_year {i_year}
	, m_month {i_month}
	, m_day {i_day}
{
	AZ_Assert(Calendar::IsValidYear(i_year), "Year cannot be equal to %d", Date().GetYear());
	AZ_Assert(Calendar::IsValidMonth(i_month), "Month must be in range [1;%d]", Calendar::MONTHS_IN_YEAR);
	AZ_Assert(Calendar::IsValidDay(i_day), "Day must be in range [1;%d]", Calendar::DAYS_IN_LONG_MONTH);
}

Date::Date(TimeS i_seconds, Year i_epoch)
{	
	TimeS remainder;
	FromSeconds(i_seconds, remainder, i_epoch);
}

bool Date::IsValid() const
{
	return (
		Calendar::IsValidYear(m_year) &&
		Calendar::IsValidMonth(m_month) &&
		Calendar::IsValidDay(m_day)
	);
}

Date::Day Date::GetDayNumber() const
{
	return m_day;
}

AZStd::string Date::GetMonthName() const
{
	return Calendar::GetMonthName(m_month);
}

Date::Month Date::GetMonthNumber() const
{
	return m_month;
}

Date::Week Date::GetWeek() const
{
	const auto week = static_cast<Week>(Utils::CalculateQuotient(m_day, Calendar::DAYS_IN_WEEK));

	return week;
}

Date::Year Date::GetYear() const
{
	return m_year;
}

void Date::SetDay(Day i_day)
{
	if(!Calendar::IsValidDay(i_day))
	{
		AZ_Assert(false, "Argument must be a number in range [1; %d]", Calendar::DAYS_IN_LONG_MONTH);
		return;
	}

	m_day = i_day;
}

void Date::SetMonth(Month i_month)
{
	if(!Calendar::IsValidMonth(i_month))
	{
		AZ_Assert(false, "Argument must be a number in range [1; %d]", Calendar::MONTHS_IN_YEAR);
		return;
	}

	m_month = i_month;
	ValidateDay();
}

void Date::SetYear(Year i_year)
{
	if(!Calendar::IsValidYear(i_year))
	{
		AZ_Assert(false, "Argument cannot be equal to %d", Date().GetYear());
		return;
	}
	
	m_year = i_year;
	ValidateDay();
}

DateChange Date::AddDay()
{
	return AddDays(static_cast<Day>(1));
}

DateChange Date::AddMonth()
{
	return AddMonths(static_cast<Month>(1), true);
}

DateChange Date::AddYear()
{
	return AddYears(static_cast<Year>(1), true);
}

DateChange Date::AddDays(Day i_value)
{
	AZ_Assert(i_value <= Calendar::DAYS_IN_SHORT_MONTH, "Argument cannot be greater than %d", Calendar::DAYS_IN_SHORT_MONTH);
	AZ_Assert(IsValid(), "Cannot add days when at least one field is invalid");

	m_day += i_value;
	const Day lastMonthDay = Calendar::GetLastMonthDay(m_month, m_year);
	if(m_day <= lastMonthDay)
	{
		return DateChange::DAY;
	}

	m_day -= lastMonthDay;
	return AddMonths(static_cast<Month>(1), false);
}

DateChange Date::AddMonths(Month i_value)
{
	return AddMonths(i_value, true);
}

DateChange Date::AddMonths(Month i_value, bool i_dayValidation)
{
	AZ_Assert(i_value <= Calendar::MONTHS_IN_YEAR, "Argument cannot be greater than %d", Calendar::MONTHS_IN_YEAR);
	AZ_Assert(IsValid(), "Cannot add months when at least one field is invalid");

	m_month += i_value;

	if(i_dayValidation)
	{
		ValidateDay();
	}

	if(m_month <= Calendar::MONTHS_IN_YEAR)
	{
		return DateChange::MONTH;
	}

	m_month -= Calendar::MONTHS_IN_YEAR;
	return AddYears(static_cast<Year>(1), false);
}

DateChange Date::AddYears(Year i_value)
{
	return AddYears(i_value, true);
}

DateChange Date::AddYears(Year i_value, bool i_dayValidation)
{
	AZ_Assert(IsValid(), "Cannot add years when at least one field is invalid");

	m_year += i_value;
	if(!Calendar::IsValidYear(m_year))
	{
		++m_year;
	}

	if(i_dayValidation)
	{
		ValidateDay();
	}

	return DateChange::YEAR;
}

DateChange Date::SubtractDay()
{
	return SubtractDays(static_cast<Day>(1));
}

DateChange Date::SubtractMonth()
{
	return SubtractMonths(static_cast<Month>(1), true);
}

DateChange Date::SubtractYear()
{
	return SubtractYears(static_cast<Year>(1), true);
}

DateChange Date::SubtractDays(Day i_value)
{
	AZ_Assert(i_value <= Calendar::DAYS_IN_SHORT_MONTH, "Argument cannot be greater than %d", Calendar::DAYS_IN_SHORT_MONTH);
	AZ_Assert(IsValid(), "Cannot subtract days when at least one field is invalid");

	if(m_day > i_value)
	{
		m_day -= i_value;

		return DateChange::DAY;
	}

	DateChange change = SubtractMonths(static_cast<Month>(1), false);

	const Day lastMonthDay = Calendar::GetLastMonthDay(m_month, m_year);
	m_day += lastMonthDay - i_value;

	return change;
}

DateChange Date::SubtractMonths(Month i_value)
{
	return SubtractMonths(i_value, true);
}

DateChange Date::SubtractMonths(Month i_value, bool i_dayValidation)
{
	AZ_Assert(i_value <= Calendar::MONTHS_IN_YEAR, "Argument cannot be greater than %d", Calendar::MONTHS_IN_YEAR);
	AZ_Assert(IsValid(), "Cannot subtract months when at least one field is invalid");

	if(m_month > i_value)
	{
		m_month -= i_value;

		return DateChange::MONTH;
	}
	
	m_month += Calendar::MONTHS_IN_YEAR - i_value;

	if(i_dayValidation)
	{
		ValidateDay();
	}

	return SubtractYears(static_cast<Year>(1), false);
}

DateChange Date::SubtractYears(Year i_value)
{
	return SubtractYears(i_value, true);
}

DateChange Date::SubtractYears(Year i_value, bool i_dayValidation)
{
	AZ_Assert(IsValid(), "Cannot subtract years when at least one field is invalid");

	m_year -= i_value;
	if(!Calendar::IsValidYear(m_year))
	{
		--m_year;
	}

	if(i_dayValidation)
	{
		ValidateDay();
	}

	return DateChange::YEAR;
}

void Date::ValidateDay()
{
	const Day lastMonthDay = Calendar::GetLastMonthDay(m_month, m_year);
	if(m_day > lastMonthDay)
	{
		m_day = lastMonthDay;
	}
}

TimeS Date::ToSeconds(Year i_epoch) const
{
	AZ_Assert(IsValid(), "Cannot convert to second when at least one field is invalid");

	Year startYear, endYear;
	Month startMonth, endMonth;
	Day startDay, endDay;

	const bool isBeforeEpoch = (m_year < i_epoch);
	if(isBeforeEpoch)
	{
		startYear = m_year;
		endYear = i_epoch;

		startMonth = m_month + 1;
		endMonth = Calendar::MONTHS_IN_YEAR + 1;

		startDay = m_day + 1;
		endDay = Calendar::GetLastMonthDay(m_month, m_year) + 1;
	}
	else
	{
		startYear = i_epoch;
		endYear = m_year;

		startMonth = 1;
		endMonth = m_month;

		startDay = 1;
		endDay = m_day;
	}

	TimeS seconds = 0.f;
	for(Year year = startYear; year < endYear; ++year)
	{
		const auto nYearDays = static_cast<TimeS>(Calendar::IsLeapYear(year) ? Calendar::DAYS_IN_LEAP_YEAR : Calendar::DAYS_IN_COMMON_YEAR);
		seconds += nYearDays * Time::DAY_TO_SECONDS;
	}

	for(Month month = startMonth; month < endMonth; ++month)
	{
		const auto nMonthDays = static_cast<TimeS>(Calendar::GetLastMonthDay(month, endYear));
		seconds += nMonthDays * Time::DAY_TO_SECONDS;
	}

	seconds += (endDay - startDay) * Time::DAY_TO_SECONDS;

	if(isBeforeEpoch)
	{
		seconds = -seconds;
	}

	return seconds;
}

void Date::FromSeconds(TimeS i_value, TimeS& o_remainder, Year i_epoch)
{
	m_day = 1;
	m_month = 1;
	m_year = i_epoch;

	constexpr float tolerance = AZ::Constants::FloatEpsilon;
	if(AZ::IsClose(i_value, 0.f, tolerance))
	{
		o_remainder = 0.f;
		return;
	}

	TimeS seconds;
	DateChange (Date::* incrementYear)();
	DateChange (Date::* incrementMonth)();

	const bool isBeforeEpoch = (i_value < 0.f);
	if(isBeforeEpoch)
	{
		seconds = AZStd::abs(i_value);

		incrementYear = &Date::AddYear;
		incrementMonth = &Date::AddMonth;
	}
	else
	{
		seconds = i_value;

		incrementYear = &Date::SubtractYear;
		incrementMonth = &Date::SubtractMonth;
	}

	float quotient = Utils::CalculateQuotient(seconds, Time::DAY_TO_SECONDS);
	o_remainder = Utils::CalculateRemainder(seconds, Time::DAY_TO_SECONDS, quotient);
	auto nDays = static_cast<AZ::u32>(quotient);

	while(nDays > 0)
	{
		const auto nYearDays = static_cast<AZ::u32>(Calendar::IsLeapYear(m_year) ? Calendar::DAYS_IN_LEAP_YEAR : Calendar::DAYS_IN_COMMON_YEAR);
		if(nDays > nYearDays)
		{
			(this->*incrementYear)();
			nDays -= nYearDays;

			continue;
		}
		
		const auto nMonthDays = static_cast<AZ::u32>(Calendar::GetLastMonthDay(m_month, m_year));
		if(nDays > nMonthDays)
		{
			(this->*incrementMonth)();
			nDays -= nMonthDays;

			continue;
		}

		m_day = nDays;
		break;
	}
}

DateChange Date::MoveBySeconds(TimeS i_offset, TimeS& o_remainder)
{
	if(AZStd::abs(i_offset) < Time::DAY_TO_SECONDS)
	{
		o_remainder = i_offset;

		return DateChange::NONE;
	}

	Date oldDate {*this};

	const TimeS newSeconds = ToSeconds(m_year) + i_offset;
	FromSeconds(newSeconds, o_remainder, m_year);

	DateChange change;
	if(m_year != oldDate.m_year)
	{
		change = DateChange::YEAR;
	}
	else if(m_month != oldDate.m_month)
	{
		change = DateChange::MONTH;
	}
	else if(m_day != oldDate.m_day)
	{
		change = DateChange::DAY;
	}
	else
	{
		change = DateChange::NONE;
	}

	return change;
}

DateDifference Date::CountDaysTo(const Date& i_other) const
{
	const Date* firstDate;
	const Date* lastDate;

	const bool isEarlierYear = (m_year < i_other.m_year);
	if(isEarlierYear)
	{
		firstDate = this;
		lastDate = &i_other;
	}
	else
	{
		firstDate = &i_other;
		lastDate = this;
	}

	const auto nFirstMonthDays = static_cast<DateDifference>(Calendar::GetLastMonthDay(firstDate->m_month, firstDate->m_year));
	DateDifference days = nFirstMonthDays - firstDate->m_day;

	for(Month month = firstDate->m_month + 1; month <= Calendar::MONTHS_IN_YEAR; ++month)
	{
		const auto nMonthDays = static_cast<DateDifference>(Calendar::GetLastMonthDay(month, firstDate->m_year));
		days += nMonthDays;
	}

	Date::Year secondYear = firstDate->m_year + ((firstDate->m_year != -1) ? 1 : 2);
	Date::Year penultimateYear = lastDate->m_year - ((lastDate->m_year != 1) ? 1 : 2);
	for(Date::Year year = secondYear; year <= penultimateYear; ++year)
	{
		const auto nYearDays = static_cast<TimeS>(Calendar::IsLeapYear(year) ? Calendar::DAYS_IN_LEAP_YEAR : Calendar::DAYS_IN_COMMON_YEAR);
		days += nYearDays;
	}

	for(Month month = 1; month < lastDate->m_month; ++month)
	{
		const auto nMonthDays = static_cast<DateDifference>(Calendar::GetLastMonthDay(month, firstDate->m_year));
		days += nMonthDays;
	}

	days += lastDate->m_day;

	if(isEarlierYear)
	{
		days = -days;
	}

	return days;
}

DateDifference Date::CountMonthsTo(const Date& i_other) const
{
	const DateDifference years = CountYearsTo(i_other);

	DateDifference months = static_cast<DateDifference>(i_other.m_month) - static_cast<DateDifference>(m_month);
	if(years != 0)
	{
		months += years * static_cast<DateDifference>(Calendar::MONTHS_IN_YEAR);
	}

	return months;
}

DateDifference Date::CountYearsTo(const Date& i_other) const
{
	const DateDifference years = static_cast<DateDifference>(i_other.m_year) - static_cast<DateDifference>(m_year);

	return years;
}

AZStd::string Date::Format(const AZStd::string& i_pattern) const
{
	AZStd::string output;
	if(i_pattern.size() < 2)
	{
		output = i_pattern;

		return output;
	}

	AZStd::string::size_type i = 0;
	for(; i < i_pattern.size() - 1; ++i)
	{
		const char& currentChar = i_pattern[i];
		const char& nextChar = i_pattern[i+1];

		if(currentChar == '\\' && nextChar == '%')
		{
			output += '%';
			++i;
		}
		else if(currentChar == '%')
		{
			++i;
			switch(nextChar)
			{
				case 'd':
				{
					output += Calendar::IsValidDay(m_day) ? AZStd::to_string(m_day) : "*";
				}
				break;

				case 'm':
				{
					output += Calendar::IsValidMonth(m_month) ? AZStd::to_string(m_month) : "*";
				}
				break;

				case 'M':
				{
					output += GetMonthName();
				}
				break;

				case 'y':
				{
					output += Calendar::IsValidYear(m_year) ? AZStd::to_string(m_year) : "*";
				}
				break;

				case 'Y':
				{
					output += Calendar::IsValidYear(m_year) ? AZStd::to_string(AZStd::abs(m_year)) : "*";
					if(m_year < 0)
					{
						output += " ";
						output += Calendar::SUFFIX_BEFORE_EPOCH;
					}
				}
				break;

				default:
				{
					AZ_Assert(false, "Unsupported specifier: %c", nextChar);
					continue;
				}
			}
		}
		else
		{
			output += currentChar;
		}
	}

	if(i < i_pattern.size())
	{
		output += i_pattern[i];
	}

	return output;
}

AZStd::string Date::ToString() const
{
	const AZStd::string output =
		(Calendar::IsValidYear(m_year) ? AZStd::to_string(m_year) : "*") + '-' +
		(Calendar::IsValidMonth(m_month) ? AZStd::to_string(m_month) : "*") + '-' +
		(Calendar::IsValidDay(m_day) ? AZStd::to_string(m_day) : "*")
	;

	return output;
}

DateComparison Date::CompareTo(const Date& i_other) const
{
	const bool isThisValid = IsValid();
	const bool isOtherValid = i_other.IsValid();

	if(isThisValid && isOtherValid)
	{
		if(m_year == i_other.m_year)
		{
			if(m_month == i_other.m_month)
			{
				if(m_day == i_other.m_day)
				{
					return DateComparison::EQUAL;
				}
				else if(m_day > i_other.m_day)
				{
					return DateComparison::GREATER_THAN;
				}
			}
			else if(m_month > i_other.m_month)
			{
				return DateComparison::GREATER_THAN;
			}
		}
		else if(m_year > i_other.m_year)
		{
			return DateComparison::GREATER_THAN;
		}

		return DateComparison::LESS_THAN;
	}
	else if(isThisValid)
	{
		return DateComparison::GREATER_THAN;
	}
	else if(isOtherValid)
	{
		return DateComparison::LESS_THAN;
	}
	else
	{
		return DateComparison::EQUAL;
	}
}

Date Date::operator+(TimeS i_seconds) const
{
	Date newDate {*this};
	newDate += i_seconds;

	return newDate;
}

Date Date::operator-(TimeS i_seconds) const
{
	Date newDate {*this};
	newDate -= i_seconds;

	return newDate;
}

DurationS Date::operator-(const Date& i_rhs) const
{	
	const DurationS duration = ToSeconds(m_year) + i_rhs.ToSeconds(m_year);
	return duration;
}

Date& Date::operator+=(TimeS i_seconds)
{
	const float unsignedSeconds = AZStd::abs(i_seconds);
	if(unsignedSeconds < Time::DAY_TO_SECONDS)
	{}
	else if(unsignedSeconds < SHORT_MONTH_TO_SECONDS)
	{
		if(i_seconds > 0.f)
		{
			const auto days = static_cast<Day>(Utils::CalculateQuotient(unsignedSeconds, Time::DAY_TO_SECONDS));
			AddDays(days);
		}
		else if(i_seconds < 0.f)
		{
			const auto days = static_cast<Day>(Utils::CalculateQuotient(unsignedSeconds, Time::DAY_TO_SECONDS));
			SubtractDays(days);
		}
	}
	else
	{
		TimeS remainder;
		MoveBySeconds(i_seconds, remainder);
	}

	return *this;
}

Date& Date::operator-=(TimeS i_seconds)
{
	return operator+=(-i_seconds);
}

Date& Date::operator++()
{
	AddDay();

	return *this;
}

Date Date::operator++(int)
{
	Date oldDate {*this};
	operator++();

	return oldDate;
}

Date& Date::operator--()
{
	SubtractDay();

	return *this;
}

Date Date::operator--(int)
{
	Date oldDate {*this};
	operator--();

	return oldDate;
}

bool Date::operator==(const Date& i_rhs) const
{
	const DateComparison comparison = CompareTo(i_rhs);
	
	return (comparison == DateComparison::EQUAL);
}

bool Date::operator<(const Date& i_rhs) const
{
	const DateComparison comparison = CompareTo(i_rhs);
	
	return (comparison == DateComparison::LESS_THAN);
}

bool Date::operator<=(const Date& i_rhs) const
{
	const DateComparison comparison = CompareTo(i_rhs);

	return (comparison == DateComparison::LESS_THAN || comparison == DateComparison::EQUAL);
}

bool Date::operator>(const Date& i_rhs) const
{
	const DateComparison comparison = CompareTo(i_rhs);
	
	return (comparison == DateComparison::GREATER_THAN);
}

bool Date::operator>=(const Date& i_rhs) const
{
	const DateComparison comparison = CompareTo(i_rhs);

	return (comparison == DateComparison::GREATER_THAN || comparison == DateComparison::EQUAL);
}
