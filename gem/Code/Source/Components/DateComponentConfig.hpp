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
#include <AzCore/RTTI/TypeInfoSimple.h>

#include <DateTime/Types/Date.hpp>

#include "TimeComponentConfig.hpp"


namespace Loherangrin::Gems::DateTime
{
	struct DateComponentConfig
	{
		enum class CalendarType : AZ::u8
		{
			GREGORIAN = 0
		};

		AZ_TYPE_INFO(DateComponentConfig, "{9A5E3288-270C-403E-BB52-C71908872DFA}");
		static void Reflect(AZ::ReflectContext* io_context);

		bool IsCalendarVisible() const;
		bool IsNowDateVisible() const;

		AZ::Crc32 OnIsEnabledChanged() const;

		bool m_isEnabled { false };

		CalendarType m_calendar { CalendarType::GREGORIAN };

		Date m_nowDate { 1, 1, 1 };

		TimeComponentConfig* m_timeConfig { nullptr };
	};

} // Loherangrin::Gems::DateTime
