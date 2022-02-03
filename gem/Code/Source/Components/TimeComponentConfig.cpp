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

#include <AzCore/Serialization/SerializeContext.h>

#include "TimeComponentConfig.hpp"

using Loherangrin::Gems::DateTime::TimeComponentConfig;


void TimeComponentConfig::Reflect(AZ::ReflectContext* io_context)
{
	Time::Reflect(io_context);

	if(auto serializeContext = azrtti_cast<AZ::SerializeContext*>(io_context))
	{
		serializeContext->Class<TimeComponentConfig>()
			->Version(0)
			->Field("IsEnabled", &TimeComponentConfig::m_isEnabled)
			->Field("CurrentTime", &TimeComponentConfig::m_nowTime)
			->Field("TimeScale", &TimeComponentConfig::m_timeScale)
			->Field("TimeSpeed", &TimeComponentConfig::m_timeSpeed)
			;
	}
}
