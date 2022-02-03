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

#include <AzCore/Serialization/EditContextConstants.inl>
#include <AzCore/Serialization/SerializeContext.h>

#include "AlarmComponentConfig.hpp"
#include "DateComponentConfig.hpp"

using Loherangrin::Gems::DateTime::DateComponentConfig;


void DateComponentConfig::Reflect(AZ::ReflectContext* io_context)
{
	Date::Reflect(io_context);

	if(auto serializeContext = azrtti_cast<AZ::SerializeContext*>(io_context))
	{
		serializeContext->Class<DateComponentConfig>()
			->Version(0)
			->Field("IsEnabled", &DateComponentConfig::m_isEnabled)
			->Field("Calendar", &DateComponentConfig::m_calendar)
			->Field("CurrentDate", &DateComponentConfig::m_nowDate)
			;
	}
}

bool DateComponentConfig::IsCalendarVisible() const
{
	return m_isEnabled;
}

bool DateComponentConfig::IsNowDateVisible() const
{
	return m_isEnabled;
}

AZ::Crc32 DateComponentConfig::OnIsEnabledChanged() const
{
	if(m_timeConfig)
	{
		m_timeConfig->m_hasDate = m_isEnabled;
		AlarmConfig::s_hasDate = m_isEnabled;
	}

	return AZ::Edit::PropertyRefreshLevels::EntireTree;
}
