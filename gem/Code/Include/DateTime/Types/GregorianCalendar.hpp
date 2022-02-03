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

#include <AzCore/base.h>
#include <AzCore/std/string/string.h>


namespace Loherangrin::Gems::DateTime
{
	class GregorianCalendar
	{
	public:
		using Day = AZ::u8;
		using Month = AZ::u8;
		using Week = AZ::u8;
		using Year = AZ::s16;

		static Day GetLastMonthDay(Month i_month, Year i_year);
		static AZStd::string GetMonthName(Month i_monthNumber);

		static bool IsLeapYear(Year i_year);

		static bool IsValidDay(Day i_day);
		static bool IsValidMonth(Month i_month);
		static bool IsValidYear(Year i_year);

		static constexpr AZ::u8 DAYS_IN_WEEK = 7;
		static constexpr AZ::u8 DAYS_IN_SHORT_MONTH = 28;
		static constexpr AZ::u8 DAYS_IN_MID_MONTH = 30;
		static constexpr AZ::u8 DAYS_IN_LONG_MONTH = 31;
		static constexpr AZ::u8 DAYS_IN_LEAP_MONTH = DAYS_IN_SHORT_MONTH + 1;
		static constexpr AZ::u16 DAYS_IN_COMMON_YEAR = 365;
		static constexpr AZ::u16 DAYS_IN_LEAP_YEAR = DAYS_IN_COMMON_YEAR + 1;
		static constexpr AZ::u8 MONTHS_IN_YEAR = 12;

		static constexpr const char* SUFFIX_BEFORE_EPOCH = "BC";

		static constexpr Year EPOCHS_UNIX = 1970;
	};

} // Loherangrin::Gems::DateTime
