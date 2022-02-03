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

#include "GregorianCalendar.hpp"

using Loherangrin::Gems::DateTime::GregorianCalendar;


Date::Day GregorianCalendar::GetLastMonthDay(Month i_month, Year i_year)
{
	switch(i_month)
	{
		case 2:
		{
			return (IsLeapYear(i_year))
				? DAYS_IN_LEAP_MONTH
				: DAYS_IN_SHORT_MONTH;
		}

		case 4:
		case 6:
		case 9:
		case 11:
		{
			return DAYS_IN_MID_MONTH;
		}

		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
		{
			return DAYS_IN_LONG_MONTH;
		}

		default:
		{
			AZ_Assert(false, "Unable to calculate end of an invalid month");
			return 0;
		}
	}
}

AZStd::string GregorianCalendar::GetMonthName(Month i_monthNumber)
{
	switch(i_monthNumber)
	{
		case 1:
			return "January";

		case 2:
			return "February";

		case 3:
			return "March";

		case 4:
			return "April";

		case 5:
			return "May";

		case 6:
			return "June";

		case 7:
			return "July";

		case 8:
			return "August";

		case 9:
			return "September";

		case 10:
			return "October";

		case 11:
			return "November";

		case 12:
			return "December";
			break;

		case 0:
			return "*";

		default:
			AZ_Assert(false, "Invalid month");
			return "N/A";
	}
}

bool GregorianCalendar::IsLeapYear(Year i_year)
{
	if((i_year % 4) != 0)
	{
		return false;
	}

	if((i_year % 100) == 0 && (i_year % 400) != 0)
	{
		return false;
	}

	return true;
}

bool GregorianCalendar::IsValidDay(Day i_day)
{
	return (i_day > 0 && i_day <= DAYS_IN_LONG_MONTH);
}

bool GregorianCalendar::IsValidMonth(Month i_month)
{
	return (i_month > 0 && i_month <= MONTHS_IN_YEAR);
}

bool GregorianCalendar::IsValidYear(Year i_year)
{
	return (i_year != 0);
}
