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
	namespace ScriptAttributes
	{
		static constexpr const char* CATEGORY = "Date & Time";
		static constexpr const char* MODULE = "loherangrin.date_time";
	}

	namespace Services
	{
		static constexpr AZ::Crc32 SYSTEM = AZ_CRC_CE("DateTimeSystemService");

		static constexpr AZ::Crc32 ALARM = AZ_CRC_CE("AlarmService");
		static constexpr AZ::Crc32 DATE = AZ_CRC_CE("DateService");
		static constexpr AZ::Crc32 TIME = AZ_CRC_CE("TimeService");
	}

	namespace Constants
	{
		namespace TimeSpeeds
		{
			static constexpr float FREEZE = 0.f;
			static constexpr float SLOW = 0.25f;
			static constexpr float NORMAL = 1.f;
			static constexpr float FAST = 4.f;
		}
	}

} // Loherangrin::Gems::DateTime
