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
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>

#include <DateTime/Constants/DateTimeEditorConstants.hpp>

#include "../Components/AlarmComponent.hpp"
#include "../Components/DateComponent.hpp"
#include "../Components/TimeComponent.hpp"

#include "DateTimeEditorLevelComponent.hpp"

using Loherangrin::Gems::DateTime::DateTimeEditorLevelComponent;


void DateTimeEditorLevelComponent::Reflect(AZ::ReflectContext* io_context)
{
	if(auto serializeContext = azrtti_cast<AZ::SerializeContext*>(io_context))
	{
		serializeContext->Class<DateTimeEditorLevelComponent, AzToolsFramework::Components::EditorComponentBase>()
			->Version(0)
			->Field("TimeConfiguration", &DateTimeEditorLevelComponent::m_timeConfig)
			->Field("DateConfiguration", &DateTimeEditorLevelComponent::m_dateConfig)
			->Field("AlarmConfiguration", &DateTimeEditorLevelComponent::m_alarmConfig)
		;

		if(AZ::EditContext* editContext = serializeContext->GetEditContext())
		{
			editContext->Class<DateTimeEditorLevelComponent>("Date & Time", "Measure date and time in the time, notifying any change to external listeners")
				->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Level"))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)

				->DataElement(AZ::Edit::UIHandlers::Default, &DateTimeEditorLevelComponent::m_timeConfig, "Time", "")
					->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly)

				->DataElement(AZ::Edit::UIHandlers::Default, &DateTimeEditorLevelComponent::m_dateConfig, "Date", "")
					->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly)

				->DataElement(AZ::Edit::UIHandlers::Default, &DateTimeEditorLevelComponent::m_alarmConfig, "Alarm", "")
					->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly)
			;


			editContext->Class<AlarmComponentConfig>("Alarm Configuration", "")
				->DataElement(AZ::Edit::UIHandlers::Default, &AlarmComponentConfig::m_isEnabled, "Use alarms", "It allows to set alarms that can trigger actions at recurrent instants (e.g. every <X> hours, ...). When disabled, no alarm can be set both in the interface and by code")
					->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)

				->DataElement(AZ::Edit::UIHandlers::Default, &AlarmComponentConfig::m_alarms, "Alarms", "List of registered alarms")
					->Attribute(AZ::Edit::Attributes::Visibility, &AlarmComponentConfig::m_isEnabled)
			;

			editContext->Class<AlarmConfig>("Alarm", "")
				->DataElement(AZ::Edit::UIHandlers::Default, &AlarmConfig::m_dateRule, "Date", "")
				->DataElement(AZ::Edit::UIHandlers::Default, &AlarmConfig::m_timeRule, "Time", "")

				->ClassElement(AZ::Edit::ClassElements::Group, "Action")
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)

					->DataElement(AZ::Edit::UIHandlers::Default, &AlarmConfig::m_isSilent, "Silent", "When enabled, the alarm is ticking but no action will be executed when it goes off. This status can be toggled at runtime by code")

					->DataElement(AZ::Edit::UIHandlers::ComboBox, &AlarmConfig::m_action, "Type", "Action to be performed when the alarm goes off")
						->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
						->EnumAttribute(AlarmConfig::AlarmActionType::NOTIFY_REQUESTER, "Notify Entity")

					->DataElement(AZ::Edit::UIHandlers::Default, &AlarmConfig::m_actionRequesterId, "Requester", "Reference to the entity that need to be notified")
						->Attribute(AZ::Edit::Attributes::Visibility, &AlarmConfig::IsActionNotifyRequesterVisible)
			;

			editContext->Class<AlarmConfig::DateRecurrenceRuleConfig>("Date", "")
				->DataElement(AZ::Edit::UIHandlers::Default, &AlarmConfig::DateRecurrenceRuleConfig::m_hasNoStartValue, "Immediate", "When enabled, the start date of this alarm matches the one of the scene. Disable this flag to insert a manual value")
					->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
					->Attribute(AZ::Edit::Attributes::Visibility, &AlarmConfig::DateRecurrenceRuleConfig::AreFieldsVisible)

				->DataElement(AZ::Edit::UIHandlers::Default, &AlarmConfig::DateRecurrenceRuleConfig::m_startValue, "From", "Date when the alarm will go off for the first time")
					->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::AttributesAndValues)
					->Attribute(AZ::Edit::Attributes::Visibility, &AlarmConfig::DateRecurrenceRuleConfig::IsStartVisible)

				->DataElement(AZ::Edit::UIHandlers::ComboBox, &AlarmConfig::DateRecurrenceRuleConfig::m_repeatMode, "Repeat", "Rule to define how the alarm is repeated after going off")
					->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
					->Attribute(AZ::Edit::Attributes::Visibility, &AlarmConfig::DateRecurrenceRuleConfig::AreFieldsVisible)
					->EnumAttribute(AlarmConfig::DateRepeatMode::ONLY_ONCE, "No")
					->EnumAttribute(AlarmConfig::DateRepeatMode::EVERY_DAYS, "Every <X> days")
					->EnumAttribute(AlarmConfig::DateRepeatMode::EVERY_MONTHS, "Every <X> months")
					->EnumAttribute(AlarmConfig::DateRepeatMode::EVERY_YEARS, "Every <X> years")

				->DataElement(AZ::Edit::UIHandlers::Default, &AlarmConfig::DateRecurrenceRuleConfig::m_repeatInterval, "Interval", "Date difference between two repeats of this alarm. It is X value in the previous field")
					->Attribute(AZ::Edit::Attributes::Visibility, &AlarmConfig::DateRecurrenceRuleConfig::IsIntervalVisible)
					->Attribute(AZ::Edit::Attributes::Min, 1)
					->Attribute(AZ::Edit::Attributes::Max, &AlarmConfig::DateRecurrenceRuleConfig::GetMaxInterval)

				->DataElement(AZ::Edit::UIHandlers::ComboBox, &AlarmConfig::DateRecurrenceRuleConfig::m_endCondition, "Last", "Condition on the date value to stop setting a new repeat for this alarm")
					->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
					->Attribute(AZ::Edit::Attributes::Visibility, &AlarmConfig::DateRecurrenceRuleConfig::IsConditionVisible)
					->EnumAttribute(AlarmConfig::EndCondition::INFINITE, "Forever")
					->EnumAttribute(AlarmConfig::EndCondition::N_TIMES, "For <N> times")
					->EnumAttribute(AlarmConfig::EndCondition::NOT_AFTER, "To <Y> (date)")

				->DataElement(AZ::Edit::UIHandlers::Default, &AlarmConfig::DateRecurrenceRuleConfig::m_nRepeats, "Times", "How many times a repeat is set automatically after the first ring (excluded). It is N value in the previous field")
					->Attribute(AZ::Edit::Attributes::Visibility, &AlarmConfig::DateRecurrenceRuleConfig::IsNRepeatsVisible)
					->Attribute(AZ::Edit::Attributes::Min, 1)

				->DataElement(AZ::Edit::UIHandlers::Default, &AlarmConfig::DateRecurrenceRuleConfig::m_endValue, "To", "Date after when new repeats are not set anymore")
					->Attribute(AZ::Edit::Attributes::Min, &AlarmConfig::DateRecurrenceRuleConfig::GetMinEnd)
					->Attribute(AZ::Edit::Attributes::Visibility, &AlarmConfig::DateRecurrenceRuleConfig::IsEndVisible)
			;

			editContext->Class<AlarmConfig::TimeRecurrenceRuleConfig>("Time", "")
				->DataElement(AZ::Edit::UIHandlers::Default, &AlarmConfig::TimeRecurrenceRuleConfig::m_hasNoStartValue, "Immediate", "When enabled, the start time of this alarm matches the one of the scene. Disable this flag to insert a manual value")
					->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)

				->DataElement(AZ::Edit::UIHandlers::Default, &AlarmConfig::TimeRecurrenceRuleConfig::m_startValue, "From", "Date when the alarm will goes off for the first time")
					->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::AttributesAndValues)
					->Attribute(AZ::Edit::Attributes::Visibility, &AlarmConfig::TimeRecurrenceRuleConfig::IsStartVisible)

				->DataElement(AZ::Edit::UIHandlers::ComboBox, &AlarmConfig::TimeRecurrenceRuleConfig::m_repeatMode, "Repeat", "Rule to define how the alarm is repeated after going off")
					->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
					->EnumAttribute(AlarmConfig::TimeRepeatMode::ONLY_ONCE, "No")
					->EnumAttribute(AlarmConfig::TimeRepeatMode::EVERY_HOURS, "Every <X> hours")
					->EnumAttribute(AlarmConfig::TimeRepeatMode::EVERY_MINUTES, "Every <X> minutes")
					->EnumAttribute(AlarmConfig::TimeRepeatMode::EVERY_SECONDS, "Every <X> seconds")

				->DataElement(AZ::Edit::UIHandlers::Default, &AlarmConfig::TimeRecurrenceRuleConfig::m_repeatInterval, "Interval", "Time difference between two repeats of this alarm. It is X value in the previous field")
					->Attribute(AZ::Edit::Attributes::Visibility, &AlarmConfig::TimeRecurrenceRuleConfig::IsIntervalVisible)
					->Attribute(AZ::Edit::Attributes::Min, 1)
					->Attribute(AZ::Edit::Attributes::Max, &AlarmConfig::TimeRecurrenceRuleConfig::GetMaxInterval)

				->DataElement(AZ::Edit::UIHandlers::ComboBox, &AlarmConfig::TimeRecurrenceRuleConfig::m_endCondition, "Last", "Condition on the time value to stop setting a new repeat for this alarm")
					->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
					->Attribute(AZ::Edit::Attributes::Visibility, &AlarmConfig::TimeRecurrenceRuleConfig::IsConditionVisible)
					->EnumAttribute(AlarmConfig::EndCondition::INFINITE, "Forever")
					->EnumAttribute(AlarmConfig::EndCondition::N_TIMES, "For <N> times")
					->EnumAttribute(AlarmConfig::EndCondition::NOT_AFTER, "To <Y> (time)")

				->DataElement(AZ::Edit::UIHandlers::Default, &AlarmConfig::TimeRecurrenceRuleConfig::m_nRepeats, "Times", "How many times a repeat is set automatically after the first ring (excluded). It is N value in the previous field")
					->Attribute(AZ::Edit::Attributes::Visibility, &AlarmConfig::TimeRecurrenceRuleConfig::IsNRepeatsVisible)
					->Attribute(AZ::Edit::Attributes::Min, 1)

				->DataElement(AZ::Edit::UIHandlers::Default, &AlarmConfig::TimeRecurrenceRuleConfig::m_endValue, "To", "Time after when new repeats are not set anymore")
					->Attribute(AZ::Edit::Attributes::Min, &AlarmConfig::TimeRecurrenceRuleConfig::GetMinEnd)
					->Attribute(AZ::Edit::Attributes::Visibility, &AlarmConfig::TimeRecurrenceRuleConfig::IsEndVisible)
			;


			editContext->Class<DateComponentConfig>("Date Configuration", "")
				->DataElement(AZ::Edit::UIHandlers::Default, &DateComponentConfig::m_isEnabled, "Use dates", "It allows to count days using a calendar. When disabled, time is reset at the end of the day without providing any date change")
					->Attribute(AZ::Edit::Attributes::ChangeNotify, &DateComponentConfig::OnIsEnabledChanged)

				->DataElement(AZ::Edit::UIHandlers::ComboBox, &DateComponentConfig::m_calendar, "Calendar", "How days are organized")
					->Attribute(AZ::Edit::Attributes::Visibility, &DateComponentConfig::IsCalendarVisible)
					->EnumAttribute(DateComponentConfig::CalendarType::GREGORIAN, "Gregorian")

				->DataElement(AZ::Edit::UIHandlers::Default, &DateComponentConfig::m_nowDate, "Start Date", "Initial date value of the counter when the scene is started. It is expressed in scene-time unit")
					->Attribute(AZ::Edit::Attributes::Visibility, &DateComponentConfig::IsNowDateVisible)
			;

			editContext->Class<TimeComponentConfig>("Time Configuration", "")
				->DataElement(AZ::Edit::UIHandlers::Default, &TimeComponentConfig::m_timeScale, "Time Scale", "Conversion factor between one second in the real world and <X> seconds in the scene")
					->Attribute(AZ::Edit::Attributes::Min, 0.f)

				->DataElement(AZ::Edit::UIHandlers::Default, &TimeComponentConfig::m_timeSpeed, "Time Speed", "Counting speed expressed in scene-time seconds. Use 0 to stop the clock, or negative values to count in reverse order")

				->DataElement(AZ::Edit::UIHandlers::Default, &TimeComponentConfig::m_nowTime, "Start Time", "Initial time value of the counter when the scene is started. It is expressed in scene-time unit")
			;
		}
	}
}

void DateTimeEditorLevelComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& io_provided)
{
	io_provided.push_back(EditorServices::ALARM_DATE_TIME);
}

void DateTimeEditorLevelComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& io_incompatible)
{
	io_incompatible.push_back(EditorServices::ALARM_DATE_TIME);
}

void DateTimeEditorLevelComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& io_required)
{}

void DateTimeEditorLevelComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& io_dependent)
{}

void DateTimeEditorLevelComponent::Init()
{
	m_dateConfig.m_timeConfig = &m_timeConfig;
	m_dateConfig.OnIsEnabledChanged();
}

void DateTimeEditorLevelComponent::Activate()
{}

void DateTimeEditorLevelComponent::Deactivate()
{}

void DateTimeEditorLevelComponent::BuildGameEntity(AZ::Entity* io_gameEntity)
{
	if(m_dateConfig.m_isEnabled)
	{
		DateComponent* dateRuntimeComponent = io_gameEntity->CreateComponent<DateComponent>(AZStd::move(m_dateConfig));
		if(!dateRuntimeComponent)
		{
			AZ_Error(GetNamedEntityId().ToString().c_str(), false, "Unable to create DateComponent into entity: %s (%d)", io_gameEntity->GetName().c_str(), io_gameEntity->GetId());
			return;
		}
	}

	if(m_timeConfig.m_isEnabled)
	{
		TimeComponent* timeRuntimeComponent = io_gameEntity->CreateComponent<TimeComponent>(AZStd::move(m_timeConfig));
		if(!timeRuntimeComponent)
		{
			AZ_Error(GetNamedEntityId().ToString().c_str(), false, "Unable to create TimeComponent into entity: %s (%d)", io_gameEntity->GetName().c_str(), io_gameEntity->GetId());
			return;
		}
	}

	if(m_alarmConfig.m_isEnabled)
	{
		for(auto& alarm : m_alarmConfig.m_alarms)
		{
			if(m_dateConfig.m_isEnabled)
			{
				if(alarm.m_dateRule.m_hasNoStartValue)
				{
					alarm.m_dateRule.m_startValue = m_dateConfig.m_nowDate;
				}
			}
			else
			{
				alarm.m_dateRule.m_startValue = Date {};
			}

			if(alarm.m_timeRule.m_hasNoStartValue)
			{
				alarm.m_timeRule.m_startValue = m_timeConfig.m_nowTime;
			}
		}

		AlarmComponent* alarmRuntimeComponent = io_gameEntity->CreateComponent<AlarmComponent>(AZStd::move(m_alarmConfig));
		if(!alarmRuntimeComponent)
		{
			AZ_Error(GetNamedEntityId().ToString().c_str(), false, "Unable to create AlarmComponent into entity: %s (%d)", io_gameEntity->GetName().c_str(), io_gameEntity->GetId());
			return;
		}
	}
}
