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

#include "Time.hpp"

using Loherangrin::Gems::DateTime::Time;
using Loherangrin::Gems::DateTime::TimeChange;
using Loherangrin::Gems::DateTime::TimeComparison;
using Loherangrin::Gems::DateTime::TimeDifference;
using Loherangrin::Gems::DateTime::TimeS;
using Loherangrin::Gems::DateTime::DurationS;


void Time::Reflect(AZ::ReflectContext* io_context)
{
	if(auto serializeContext = azrtti_cast<AZ::SerializeContext*>(io_context))
	{
		serializeContext->Class<Time>()
			->Version(0)
			->Field("Hours", &Time::m_hours)
			->Field("Minutes", &Time::m_minutes)
			->Field("Seconds", &Time::m_seconds)
			;
	}

	if(auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(io_context))
	{
		behaviorContext
			->Enum<(AZ::u8) TimeChange::NONE>("TimeChange_None")
			->Enum<(AZ::u8) TimeChange::SECOND>("TimeChange_Second")
			->Enum<(AZ::u8) TimeChange::MINUTE>("TimeChange_Minute")
			->Enum<(AZ::u8) TimeChange::HOUR>("TimeChange_Hour")
			->Enum<(AZ::u8) TimeChange::DAY>("TimeChange_Day")
			->Enum<(AZ::u8) TimeComparison::LESS_THAN>("TimeComparison_LessThan")
			->Enum<(AZ::u8) TimeComparison::EQUAL>("TimeComparison_Equal")
			->Enum<(AZ::u8) TimeComparison::GREATER_THAN>("TimeComparison_GreaterThan")
			;

		behaviorContext->Class<Time>("Time")
			->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
			->Attribute(AZ::Script::Attributes::Module, ScriptAttributes::MODULE)
			->Attribute(AZ::Script::Attributes::Category, ScriptAttributes::CATEGORY)

			->Constructor()
			->Constructor<TimeS>()
			->Constructor<Hour, Minute, Second>()

			->Property("Hours", &Time::GetHours, &Time::SetHours)
				->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::All)

			->Property("Minutes", &Time::GetMinutes, &Time::SetMinutes)
				->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::All)

			->Property("Seconds", &Time::GetSeconds, &Time::SetSeconds)
				->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::All)

			->Method("IsValid", &Time::IsValid)

			->Method("AddHour", &Time::AddHour)
			->Method("AddMinute", &Time::AddMinute)
			->Method("AddSecond", &Time::AddSecond)

			->Method("AddHours", &Time::AddHours)
			->Method("AddMinutes", &Time::AddMinutes)
			->Method("AddSeconds", &Time::AddSeconds)

			->Method("SubtractHour", &Time::SubtractHour)
			->Method("SubtractMinute", &Time::SubtractMinute)
			->Method("SubtractSecond", &Time::SubtractSecond)

			->Method("SubtractHours", &Time::SubtractHours)
			->Method("SubtractMinutes", &Time::SubtractMinutes)
			->Method("SubtractSeconds", &Time::SubtractSeconds)

			->Method("ToSeconds", &Time::ToSeconds)
			->Method("FromSeconds", [](Time& io_instance, TimeS i_value) -> TimeS
				{
					TimeS remainder;
					io_instance.FromSeconds(i_value, remainder);

					return remainder;
				})
			->Method("MoveBySeconds", [](Time& io_instance, TimeS i_offset) -> AZStd::pair<TimeS, AZ::u8>
				{
					TimeS remainder;
					TimeChange timeChange = io_instance.MoveBySeconds(i_offset, remainder);

					return AZStd::make_pair(remainder, static_cast<AZ::u8>(timeChange));
				})

			->Method("CountHoursTo", &Time::CountHoursTo)
			->Method("CountMinutesTo", &Time::CountMinutesTo)
			->Method("CountSecondsTo", &Time::CountSecondsTo)

			->Method("Format", &Time::Format)
			->Method("ToString", &Time::ToString)
				->Attribute(AZ::Script::Attributes::Operator, AZ::Script::Attributes::OperatorType::ToString)
		
			->Method("CompareTo", &Time::CompareTo)

			->Method("Equal", &Time::operator==)
				->Attribute(AZ::Script::Attributes::Operator, AZ::Script::Attributes::OperatorType::Equal)
				->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::All)

			->Method("LessThan", &Time::operator<)
				->Attribute(AZ::Script::Attributes::Operator, AZ::Script::Attributes::OperatorType::LessThan)
				->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::All)

			->Method("LessEqualThan", &Time::operator<=)
				->Attribute(AZ::Script::Attributes::Operator, AZ::Script::Attributes::OperatorType::LessEqualThan)
				->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::All)
		;
	}
}

Time::Time()
	: m_hours { AZStd::numeric_limits<Hour>::max() }
	, m_minutes { AZStd::numeric_limits<Minute>::max() }
	, m_seconds { AZStd::numeric_limits<Second>::max() }
{}

Time::Time(Hour i_hours, Minute i_minutes, Second i_seconds)
	: m_hours { i_hours }
	, m_minutes { i_minutes }
	, m_seconds { i_seconds }
{
	AZ_Assert(AreValidHours(i_hours), "Hours must be a number in range [0; %d)", HOURS_IN_DAY);
	AZ_Assert(AreValidMinutes(i_minutes), "Minutes must be a number in range [0; %d)", MINUTES_IN_HOUR);
	AZ_Assert(AreValidSeconds(i_seconds), "Seconds must be a number in range [0; %d)", SECONDS_IN_MINUTE);
}

Time::Time(TimeS i_seconds)
{	
	TimeS remainder;
	FromSeconds(i_seconds, remainder);
}

bool Time::IsValid() const
{
	return (
		AreValidHours(m_hours) &&
		AreValidMinutes(m_minutes) &&
		AreValidSeconds(m_seconds)
	);
}

Time::Hour Time::GetHours() const
{
	return m_hours;
}

Time::Minute Time::GetMinutes() const
{
	return m_minutes;
}

Time::Second Time::GetSeconds() const
{
	return m_seconds;
}

void Time::SetHours(Hour i_hours)
{
	if(!AreValidHours(i_hours))
	{
		AZ_Assert(false, "Argument must be a number in range [0; %d)", HOURS_IN_DAY);
		return;
	}

	m_hours = i_hours;
}

void Time::SetMinutes(Minute i_minutes)
{
	if(!AreValidMinutes(i_minutes))
	{
		AZ_Assert(false, "Argument must be a number in range [0; %d)", MINUTES_IN_HOUR);
		return;
	}

	m_minutes = i_minutes;
}

void Time::SetSeconds(Second i_seconds)
{
	if(!AreValidSeconds(i_seconds))
	{
		AZ_Assert(false, "Argument must be a number in range [0; %d)", SECONDS_IN_MINUTE);
		return;
	}

	m_seconds = i_seconds;
}

TimeChange Time::AddHour()
{
	return AddHours(static_cast<Hour>(1));
}

TimeChange Time::AddMinute()
{
	return AddMinutes(static_cast<Minute>(1));
}

TimeChange Time::AddSecond()
{
	return AddSeconds(static_cast<Second>(1));
}

TimeChange Time::AddHours(Hour i_value)
{
	AZ_Assert(AreValidHours(i_value), "Argument cannot be greater than %d", HOURS_IN_DAY);
	AZ_Assert(IsValid(), "Cannot add hours when at least one field is invalid");

	m_hours += i_value;
	if(m_hours < HOURS_IN_DAY)
	{
		return TimeChange::HOUR;
	}

	m_hours -= HOURS_IN_DAY;
	return TimeChange::DAY;
}

TimeChange Time::AddMinutes(Minute i_value)
{
	AZ_Assert(AreValidMinutes(i_value), "Argument cannot be greater than %d", MINUTES_IN_HOUR);
	AZ_Assert(IsValid(), "Cannot add minutes when at least one field is invalid");

	m_minutes += i_value;
	if(m_minutes < MINUTES_IN_HOUR)
	{
		return TimeChange::MINUTE;
	}

	m_minutes -= MINUTES_IN_HOUR;
	return AddHour();
}

TimeChange Time::AddSeconds(Second i_value)
{
	AZ_Assert(AreValidSeconds(i_value), "Argument cannot be greater than %d", SECONDS_IN_MINUTE);
	AZ_Assert(IsValid(), "Cannot add seconds when at least one field is invalid");

	m_seconds += i_value;
	if(m_seconds < SECONDS_IN_MINUTE)
	{
		return TimeChange::SECOND;
	}

	m_seconds -= SECONDS_IN_MINUTE;
	return AddMinute();
}

TimeChange Time::SubtractHour()
{
	return SubtractHours(static_cast<Hour>(1));
}

TimeChange Time::SubtractMinute()
{
	return SubtractMinutes(static_cast<Minute>(1));
}

TimeChange Time::SubtractSecond()
{
	return SubtractSeconds(static_cast<Second>(1));
}

TimeChange Time::SubtractHours(Hour i_value)
{
	AZ_Assert(AreValidHours(i_value), "Argument cannot be greater than %d", HOURS_IN_DAY);
	AZ_Assert(IsValid(), "Cannot subtract hours when at least one field is invalid");

	if(m_hours >= i_value)
	{
		m_hours -= i_value;

		return TimeChange::HOUR;
	}

	m_hours += HOURS_IN_DAY - i_value;
	return TimeChange::DAY;
}

TimeChange Time::SubtractMinutes(Minute i_value)
{
	AZ_Assert(AreValidMinutes(i_value), "Argument cannot be greater than %d", MINUTES_IN_HOUR);
	AZ_Assert(IsValid(), "Cannot subtract minutes when at least one field is invalid");
	
	if(m_minutes >= i_value)
	{
		m_minutes -= i_value;

		return TimeChange::MINUTE;
	}

	m_minutes += MINUTES_IN_HOUR - i_value;
	return SubtractHour();
}

TimeChange Time::SubtractSeconds(Second i_value)
{
	AZ_Assert(AreValidSeconds(i_value), "Argument cannot be greater than %d", SECONDS_IN_MINUTE);
	AZ_Assert(IsValid(), "Cannot subtract seconds when at least one field is invalid");

	if(m_seconds >= i_value)
	{
		m_seconds -= i_value;

		return TimeChange::SECOND;
	}

	m_seconds += SECONDS_IN_MINUTE - i_value;
	return SubtractMinute();
}

TimeDifference Time::CountHoursTo(const Time& i_other) const
{
	const TimeDifference hours = static_cast<TimeDifference>(i_other.m_hours) - static_cast<TimeDifference>(m_hours);

	return hours;
}

TimeDifference Time::CountMinutesTo(const Time& i_other) const
{
	const TimeDifference hours = CountHoursTo(i_other);

	TimeDifference minutes = static_cast<TimeDifference>(i_other.m_minutes) - static_cast<TimeDifference>(m_minutes);
	if(hours != 0)
	{
		minutes += hours * static_cast<TimeDifference>(MINUTES_IN_HOUR);
	}
	
	return minutes;
}

TimeDifference Time::CountSecondsTo(const Time& i_other) const
{
	const TimeS thisSeconds = ToSeconds();
	const TimeS otherSeconds = i_other.ToSeconds();
	const TimeS seconds = otherSeconds - thisSeconds;

	return static_cast<TimeDifference>(seconds);
}

AZStd::string Time::Format(const AZStd::string& i_pattern) const
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
				case 'h':
				{
					output += AreValidHours(m_hours) ? AZStd::to_string(m_hours) : "*";
				}
				break;

				case 'H':
				{
					const AZ::u8 twelveHours = (m_hours == 0 || m_hours == 12) ? 12 : (m_hours % 12);
					output += AreValidHours(m_hours) ? AZStd::to_string(twelveHours) : "*";
				}
				break;

				case 'm':
				{
					output += AreValidMinutes(m_minutes) ? AZStd::to_string(m_minutes) : "*";
				}
				break;

				case 'P':
				{
					output += (m_hours == 0 || m_hours < 12) ? "AM" : "PM";
				}
				break;

				case 's':
				{
					output += AreValidSeconds(m_seconds) ? AZStd::to_string(m_seconds) : "*";
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

TimeS Time::ToSeconds() const
{
	return (
		static_cast<TimeS>(m_hours) * HOUR_TO_SECONDS +
		static_cast<TimeS>(m_minutes) * MINUTE_TO_SECONDS +
		static_cast<TimeS>(m_seconds)
	);
}

void Time::FromSeconds(TimeS i_value, TimeS& o_remainder)
{
	if(AZ::IsClose(i_value, 0.f, AZ::Constants::FloatEpsilon))
	{
		m_hours = 0;
		m_minutes = 0;
		m_seconds = 0;

		o_remainder = 0.f;
		return;
	}

	TimeS seconds = (AZStd::abs(i_value) > DAY_TO_SECONDS)
		? Utils::CalculateRemainder(i_value, DAY_TO_SECONDS)
		: i_value;

	if(seconds < 0.f)
	{
		seconds = DAY_TO_SECONDS - seconds;
	}

	TimeS quotient = Utils::CalculateQuotient(seconds, HOUR_TO_SECONDS);
	TimeS remainder = Utils::CalculateRemainder(seconds, HOUR_TO_SECONDS, quotient);
	m_hours = static_cast<Hour>(quotient);

	quotient = Utils::CalculateQuotient(remainder, MINUTE_TO_SECONDS);
	remainder = Utils::CalculateRemainder(remainder, MINUTE_TO_SECONDS, quotient);
	m_minutes = static_cast<Minute>(quotient);

	m_seconds = static_cast<Second>(remainder);
	o_remainder = remainder - static_cast<TimeS>(m_seconds);
}

AZStd::string Time::ToString() const
{
	const AZStd::string output =
		(AreValidHours(m_hours) ? AZStd::to_string(m_hours) : "*") + ':' +
		(AreValidMinutes(m_minutes) ? AZStd::to_string(m_minutes) : "*") + ':' +
		(AreValidSeconds(m_seconds) ? AZStd::to_string(m_seconds) : "*")
	;

	return output;
}

TimeChange Time::MoveBySeconds(TimeS i_offset, TimeS& o_remainder)
{
	constexpr float tolerance = AZ::Constants::FloatEpsilon;
	if(AZ::IsClose(i_offset, 0.f, tolerance))
	{
		o_remainder = 0.f;

		return TimeChange::NONE;
	}

	Time oldTime {*this};

	const TimeS newSeconds = ToSeconds() + i_offset;
	FromSeconds(newSeconds, o_remainder);

	TimeChange change;
	const TimeS absoluteOffset = AZStd::abs(i_offset - o_remainder);
	if(m_hours != oldTime.m_hours || AZ::IsClose(Utils::CalculateRemainder(absoluteOffset, DAY_TO_SECONDS), 0.f, tolerance))
	{
		change = TimeChange::HOUR;
	}
	else if(m_minutes != oldTime.m_minutes  || AZ::IsClose(Utils::CalculateRemainder(absoluteOffset, HOUR_TO_SECONDS), 0.f, tolerance))
	{
		change = TimeChange::MINUTE;
	}
	else if(m_seconds != oldTime.m_seconds || AZ::IsClose(Utils::CalculateRemainder(absoluteOffset, MINUTE_TO_SECONDS), 0.f, tolerance))
	{
		change = TimeChange::SECOND;
	}
	else
	{
		change = TimeChange::NONE;
	}

	return change;
}

TimeComparison Time::CompareTo(const Time& i_other) const
{
	const bool isThisValid = IsValid();
	const bool isOtherValid = IsValid();

	if(isThisValid && isOtherValid)
	{
		if(m_hours == i_other.m_hours)
		{
			if(m_minutes == i_other.m_minutes)
			{
				if(m_seconds == i_other.m_seconds)
				{
					return TimeComparison::EQUAL;
				}
				else if(m_seconds > i_other.m_seconds)
				{
					return TimeComparison::GREATER_THAN;
				}
			}
			else if(m_minutes > i_other.m_minutes)
			{
				return TimeComparison::GREATER_THAN;
			}
		}
		else if(m_hours > i_other.m_hours)
		{
			return TimeComparison::GREATER_THAN;
		}

		return TimeComparison::LESS_THAN;
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

Time Time::operator+(TimeS i_seconds) const
{
	Time newTime {*this};
	newTime += i_seconds;

	return newTime;
}

Time Time::operator-(TimeS i_seconds) const
{
	Time newTime {*this};
	newTime -= i_seconds;

	return newTime;
}

DurationS Time::operator-(const Time& i_rhs) const
{	
	DurationS duration = ToSeconds() - i_rhs.ToSeconds();
	if(duration < 0.f)
	{
		duration = DAY_TO_SECONDS + duration;
	}

	return duration;
}

Time& Time::operator+=(TimeS i_seconds)
{
	const float unsignedSeconds = AZStd::abs(i_seconds);
	if(unsignedSeconds < MINUTE_TO_SECONDS)
	{
		if(i_seconds > 0.f)
		{
			AddSeconds(static_cast<Second>(unsignedSeconds));
		}
		else if(i_seconds < 0.f)
		{
			SubtractSeconds(static_cast<Second>(unsignedSeconds));
		}
	}
	else
	{
		float remainder;
		MoveBySeconds(i_seconds, remainder);
	}

	return *this;
}

Time& Time::operator-=(TimeS i_seconds)
{
	return operator+=(-i_seconds);
}

Time& Time::operator++()
{
	AddSecond();

	return *this;
}

Time Time::operator++(int)
{
	Time oldTime {*this};
	operator++();

	return oldTime;
}

Time& Time::operator--()
{
	SubtractSecond();

	return *this;
}

Time Time::operator--(int)
{
	Time oldTime {*this};
	operator--();

	return oldTime;
}

bool Time::operator==(const Time& i_rhs) const
{
	const TimeComparison comparison = CompareTo(i_rhs);
	
	return (comparison == TimeComparison::EQUAL);
}

bool Time::operator<(const Time& i_rhs) const
{
	const TimeComparison comparison = CompareTo(i_rhs);
	
	return (comparison == TimeComparison::LESS_THAN);
}

bool Time::operator<=(const Time& i_rhs) const
{
	const TimeComparison comparison = CompareTo(i_rhs);

	return (comparison == TimeComparison::LESS_THAN || comparison == TimeComparison::EQUAL);
}

bool Time::operator>(const Time& i_rhs) const
{
	const TimeComparison comparison = CompareTo(i_rhs);
	
	return (comparison == TimeComparison::GREATER_THAN);
}

bool Time::operator>=(const Time& i_rhs) const
{
	const TimeComparison comparison = CompareTo(i_rhs);

	return (comparison == TimeComparison::GREATER_THAN || comparison == TimeComparison::EQUAL);
}

bool Time::AreValidHours(Hour i_hours)
{
	return (i_hours >= 0 && i_hours < HOURS_IN_DAY);
}

bool Time::AreValidMinutes(Minute i_minutes)
{
	return (i_minutes >= 0 && i_minutes < MINUTES_IN_HOUR);
}

bool Time::AreValidSeconds(Second i_seconds)
{
	return (i_seconds >= 0 && i_seconds < SECONDS_IN_MINUTE);
}
