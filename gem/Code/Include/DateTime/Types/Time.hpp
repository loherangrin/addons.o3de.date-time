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


namespace Loherangrin::Gems::DateTime
{
	using DurationS = float;
	using TimeS = float;

	enum class TimeChange : AZ::u8
	{
		NONE = 0,
		SECOND,
		MINUTE,
		HOUR,
		DAY
	};

	enum class TimeComparison : AZ::s8
	{
		LESS_THAN = -1,
		EQUAL,
		GREATER_THAN
	};

	using TimeDifference = AZ::s16;

	class Time
	{
	public:
		AZ_TYPE_INFO(Time, "{7E0E6142-2ED0-48A1-BF5F-2D42DD6120AA}");
		static void Reflect(AZ::ReflectContext* io_context);

		using Hour = AZ::u8;
		using Minute = Hour;
		using Second = Hour;

		using Change = TimeChange;
		using Comparison = TimeComparison;
		using Difference = TimeDifference;

		Time();
		Time(TimeS i_seconds);
		Time(Hour i_hours, Minute i_minutes, Second i_seconds);
		virtual ~Time() = default;

		bool IsValid() const;

		Hour GetHours() const;
		Minute GetMinutes() const;
		Second GetSeconds() const;

		void SetHours(Hour i_hours);
		void SetMinutes(Minute i_minutes);
		void SetSeconds(Second i_seconds);

		TimeChange AddHour();
		TimeChange AddMinute();
		TimeChange AddSecond();

		TimeChange AddHours(Hour i_value);
		TimeChange AddMinutes(Minute i_value);
		TimeChange AddSeconds(Second i_value);

		TimeChange SubtractHour();
		TimeChange SubtractMinute();
		TimeChange SubtractSecond();

		TimeChange SubtractHours(Hour i_value);
		TimeChange SubtractMinutes(Minute i_value);
		TimeChange SubtractSeconds(Second i_value);

		TimeS ToSeconds() const;
		void FromSeconds(TimeS i_value, TimeS& o_remainder);
		TimeChange MoveBySeconds(TimeS i_offset, TimeS& o_remainder);

		TimeDifference CountHoursTo(const Time& i_other) const;
		TimeDifference CountMinutesTo(const Time& i_other) const;
		TimeDifference CountSecondsTo(const Time& i_other) const;

		AZStd::string Format(const AZStd::string& i_pattern) const;
		AZStd::string ToString() const;

		TimeComparison CompareTo(const Time& i_other) const;

		Time operator+(TimeS i_seconds) const;
		Time operator-(TimeS i_seconds) const;

		DurationS operator-(const Time& i_rhs) const;

		Time& operator+=(TimeS i_seconds);
		Time& operator-=(TimeS i_seconds);

		Time& operator++();
		Time operator++(int);
		Time& operator--();
		Time operator--(int);

		bool operator==(const Time& i_rhs) const;
		bool operator<(const Time& i_rhs) const;
		bool operator<=(const Time& i_rhs) const;
		bool operator>(const Time& i_rhs) const;
		bool operator>=(const Time& i_rhs) const;

		static constexpr AZ::u8 HOURS_IN_DAY = 24;
		static constexpr AZ::u8 MINUTES_IN_HOUR = 60;
		static constexpr AZ::u8 SECONDS_IN_MINUTE = 60;

		static constexpr TimeS MINUTE_TO_SECONDS = static_cast<TimeS>(SECONDS_IN_MINUTE);
		static constexpr TimeS HOUR_TO_SECONDS = MINUTES_IN_HOUR * MINUTE_TO_SECONDS;
		static constexpr TimeS DAY_TO_SECONDS = HOURS_IN_DAY * HOUR_TO_SECONDS;

	private:
		static bool AreValidHours(Minute i_hours);
		static bool AreValidMinutes(Minute i_minutes);
		static bool AreValidSeconds(Minute i_seconds);

		Hour m_hours;
		Minute m_minutes;
		Second m_seconds;
	};

} // Loherangrin::Gems::DateTime
