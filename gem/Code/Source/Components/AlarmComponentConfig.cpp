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
#include <AzCore/std/limits.h>

#include "DateComponentConfig.hpp"

#include "AlarmComponentConfig.hpp"

using Loherangrin::Gems::DateTime::AlarmConfig;
using Loherangrin::Gems::DateTime::AlarmComponentConfig;
using Loherangrin::Gems::DateTime::DateRecurrenceRule;
using Loherangrin::Gems::DateTime::TimeRecurrenceRule;


void AlarmConfig::Reflect(AZ::ReflectContext* io_context)
{
	constexpr const char* datePrefix = "Date";
	constexpr const char* timePrefix = "Time";

	DateRecurrenceRuleConfig::Reflect(io_context, datePrefix);
	TimeRecurrenceRuleConfig::Reflect(io_context, timePrefix);

	DateRecurrenceRuleBehaviorHandle::Reflect(io_context, datePrefix);
	TimeRecurrenceRuleBehaviorHandle::Reflect(io_context, timePrefix);

	if(auto serializeContext = azrtti_cast<AZ::SerializeContext*>(io_context))
	{
		serializeContext->Class<AlarmConfig>()
			->Version(0)
			->Field("DateRule", &AlarmConfig::m_dateRule)
			->Field("TimeRule", &AlarmConfig::m_timeRule)
			->Field("Silent", &AlarmConfig::m_isSilent)
			->Field("Action", &AlarmConfig::m_action)
			->Field("Requester", &AlarmConfig::m_actionRequesterId)
		;
	}

	if(auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(io_context))
	{
		behaviorContext
			->Enum<(AZ::u8) DateRepeatMode::ONLY_ONCE>("DateRepeatMode_OnlyOnce")
			->Enum<(AZ::u8) DateRepeatMode::EVERY_DAYS>("DateRepeatMode_EveryDays")
			->Enum<(AZ::u8) DateRepeatMode::EVERY_MONTHS>("DateRepeatMode_EveryMonths")
			->Enum<(AZ::u8) DateRepeatMode::EVERY_YEARS>("DateRepeatMode_EveryYears")

			->Enum<(AZ::u8) TimeRepeatMode::ONLY_ONCE>("TimeRepeatMode_OnlyOnce")
			->Enum<(AZ::u8) TimeRepeatMode::EVERY_SECONDS>("TimeRepeatMode_EverySeconds")
			->Enum<(AZ::u8) TimeRepeatMode::EVERY_MINUTES>("TimeRepeatMode_EveryMinutes")
			->Enum<(AZ::u8) TimeRepeatMode::EVERY_HOURS>("TimeRepeatMode_EveryHours")

			->Enum<(AZ::u8) EndCondition::INFINITE>("EndCondition_Infinite")
			->Enum<(AZ::u8) EndCondition::N_TIMES>("EndCondition_NTimes")
			->Enum<(AZ::u8) EndCondition::NOT_AFTER>("EndCondition_NotAfter")
		;
	}
}

bool AlarmConfig::IsActionNotifyRequesterVisible() const
{
	return (m_action == AlarmActionType::NOTIFY_REQUESTER);
}

// ---

void AlarmComponentConfig::Reflect(AZ::ReflectContext* io_context)
{
	AlarmConfig::Reflect(io_context);

	if(auto serializeContext = azrtti_cast<AZ::SerializeContext*>(io_context))
	{
		serializeContext->Class<AlarmComponentConfig>()
			->Version(0)
			->Field("IsEnabled", &AlarmComponentConfig::m_isEnabled)
			->Field("Alarms", &AlarmComponentConfig::m_alarms)
			;
	}
}
