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

#include <AzCore/Math/Crc.h>


namespace Loherangrin::Gems::DateTime
{
	namespace EditorServices
	{
		static constexpr AZ::Crc32 SYSTEM = AZ_CRC_CE("DateTimeSystemEditorService");

		static constexpr AZ::Crc32 ALARM_DATE_TIME = AZ_CRC_CE("AlarmDateTimeEditorService");
	}

	namespace UIHandlers
	{
		static constexpr AZ::Crc32 DATE = AZ_CRC_CE("DatePicker");
		static constexpr AZ::Crc32 TIME = AZ_CRC_CE("TimePicker");
	}

	namespace UILabels
	{
		static constexpr const char* YEAR = "Year";
		static constexpr const char* MONTH = "Month";
		static constexpr const char* DAY = "Day";
		static constexpr const char* HOUR = "Hour";
		static constexpr const char* MINUTE = "Min.";
		static constexpr const char* SECOND = "Sec.";
	}

} // Loherangrin::Gems::DateTime
