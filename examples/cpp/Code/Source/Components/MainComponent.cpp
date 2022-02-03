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

#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/EditContextConstants.inl>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/std/string/string.h>

// -------------------------------------
// [INSTRUCTIONS]: #4
#include <DateTime/Constants/DateTimeConstants.hpp>
// -------------------------------------

#include <LyShine/Bus/UiCanvasBus.h>
#include <LyShine/Bus/UiElementBus.h>
#include <LyShine/Bus/UiTextBus.h>


#include "MainComponent.hpp"

using Loherangrin::Gems::DateTime::MainComponent;


void MainComponent::Reflect(AZ::ReflectContext* io_context)
{
	if(auto serializeContext = azrtti_cast<AZ::SerializeContext*>(io_context))
	{
		serializeContext->Class<MainComponent, AZ::Component>()
			->Version(0)
		;

		if(AZ::EditContext* editContext = serializeContext->GetEditContext())
		{
			editContext->Class<MainComponent>("MainComponent", "Custom component to demonstrate gem features")
				->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)
			;
		}
	}
}

void MainComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& io_provided)
{
	io_provided.push_back(AZ_CRC_CE("DateTimeCppExampleMainService"));
}

void MainComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& io_incompatible)
{
	io_incompatible.push_back(AZ_CRC_CE("DateTimeCppExampleMainService"));
}

void MainComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& io_required)
{
	io_required.push_back(AZ_CRC("UiCanvasRefService", 0xb4cb5ef4));
}

void MainComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& io_dependent)
{
	// -------------------------------------
	// [INSTRUCTIONS]: #5
	io_dependent.push_back(Services::ALARM);
	io_dependent.push_back(Services::DATE);
	io_dependent.push_back(Services::TIME);
	// -------------------------------------
}

void MainComponent::Init()
{}

void MainComponent::Activate()
{
	const AZ::EntityId& thisEntityId = GetEntityId();
	UiCanvasRefNotificationBus::Handler::BusConnect(thisEntityId);

	AZ::EntityId mainCanvas;
	EBUS_EVENT_ID_RESULT(mainCanvas, thisEntityId, UiCanvasRefBus, GetCanvas);

	m_isAlarmComponentEnabled = AlarmRequestBus::HasHandlers();
	m_isDateComponentEnabled = DateRequestBus::HasHandlers();
	m_isTimeComponentEnabled = TimeRequestBus::HasHandlers();

	const bool found = FindAllUiElements(mainCanvas);
	if(found)
	{
		AskForCurrentDate();
		AskForCurrentTime();
	}

	// -------------------------------------
	// [INSTRUCTIONS]: #6
	AlarmNotificationBus::Handler::BusConnect(thisEntityId);
	DateNotificationBus::Handler::BusConnect();
	TimeNotificationBus::Handler::BusConnect();
	// -------------------------------------
}

void MainComponent::Deactivate()
{
	const AZ::EntityId& thisEntityId = GetEntityId();

	// -------------------------------------
	// [INSTRUCTIONS]: #7
	TimeNotificationBus::Handler::BusDisconnect();
	DateNotificationBus::Handler::BusDisconnect();
	AlarmNotificationBus::Handler::BusDisconnect(thisEntityId);
	// -------------------------------------

	UiCanvasRefNotificationBus::Handler::BusDisconnect(thisEntityId);
}

void MainComponent::OnCanvasRefChanged(AZ::EntityId i_uiCanvasRefEntity, AZ::EntityId i_uiCanvasEntity)
{
	const bool changed = FindAllUiElements(i_uiCanvasEntity);
	if(changed)
	{
		AskForCurrentDate();
		AskForCurrentTime();
	}
}

// -------------------------------------
// [INSTRUCTIONS]: #8

void MainComponent::OnAlarmBell(const Date& i_date, const Time& i_time)
{
	const AZStd::string notificationText = FormatMessageWithDateTime("Received a new notification", i_date, i_time);
	EditTextUiElement(m_currentAlarmTextId, notificationText);

	ShowNotification(notificationText, i_date, i_time);
}

void MainComponent::OnDateDayChanged(const Date& i_nowDate)
{
	EditTextUiElement(m_dateYearTextId, AZStd::to_string(i_nowDate.GetYear()));
	EditTextUiElement(m_dateMonthTextId, i_nowDate.GetMonthName());
	EditTextUiElement(m_dateDayTextId, AZStd::to_string(i_nowDate.GetDayNumber()));

	if(m_notificationCloseDate.IsValid() && (m_notificationCloseDate == i_nowDate))
	{
		m_isNotificationCloseDate = true;
	}
}

void MainComponent::OnTimeSecondChanged(const Time& i_nowTime)
{
	EditTextUiElement(m_timeHoursTextId, AZStd::to_string(i_nowTime.GetHours()));
	EditTextUiElement(m_timeMinutesTextId, AZStd::to_string(i_nowTime.GetMinutes()));
	EditTextUiElement(m_timeSecondsTextId, AZStd::to_string(i_nowTime.GetSeconds()));

	CloseNotificationIfExpired(i_nowTime);

	AskForNextAlarm();
}

// -------------------------------------

AZStd::string MainComponent::FormatMessageWithDateTime(const AZStd::string& i_message, const Date& i_date, const Time& i_time)
{
	AZStd::string output = i_message;

	if(i_date.IsValid())
	{
		output += " on " + i_date.ToString();
	}

	if(i_time.IsValid())
	{
		output += " at " + i_time.ToString();
	}

	return output;
}

void MainComponent::AskForCurrentDate()
{
	if(!m_isDateComponentEnabled)
	{
		return;
	}

	Date nowDate;
	EBUS_EVENT_RESULT(nowDate, DateRequestBus, GetCurrentDate);

	OnDateDayChanged(nowDate);
}

void MainComponent::AskForCurrentTime()
{
	if(!m_isTimeComponentEnabled)
	{
		return;
	}

	Time nowTime;
	EBUS_EVENT_RESULT(nowTime, TimeRequestBus, GetCurrentTime);

	OnTimeSecondChanged(nowTime);
}

void MainComponent::AskForNextAlarm() const
{
	if(!m_isAlarmComponentEnabled)
	{
		return;
	}

	AZStd::pair<Date, Time> nextAlarm;
	EBUS_EVENT_RESULT(nextAlarm, AlarmRequestBus, GetWhenNextAlarm);

	const Date& nextDate = nextAlarm.first;
	const Time& nextTime = nextAlarm.second;

	AZStd::string nextAlarmText;
	if(!nextTime.IsValid())
	{
		nextAlarmText = "No alarm is left";
	}
	else
	{
		nextAlarmText = FormatMessageWithDateTime("Next alarm will be", nextDate, nextTime);
	}

	EditTextUiElement(m_nextAlarmTextId, nextAlarmText);
}

void MainComponent::ShowNotification(const AZStd::string& i_message, const Date& i_nowDate, const Time& i_nowTime)
{
	ShowUiElement(m_notificationPanelId);

	TimeS notificationTimeout = MAX_ALARM_NOTIFICATION_TIMEOUT;
	EBUS_EVENT_RESULT(notificationTimeout, TimeRequestBus, ConvertToSceneTime, notificationTimeout);

	TimeS remainder;
	if(i_nowDate.IsValid())
	{
		m_notificationCloseDate = i_nowDate;

		const DateChange dateChange = m_notificationCloseDate.MoveBySeconds(notificationTimeout, remainder);
		m_isNotificationCloseDate = (dateChange == DateChange::NONE);
	}
	else
	{
		remainder = notificationTimeout;
		m_isNotificationCloseDate = true;
	}

	TimeS fraction;
	m_notificationCloseTime = i_nowTime;
	m_notificationCloseTime.MoveBySeconds(remainder, fraction);
}

void MainComponent::CloseNotificationIfExpired(const Time& i_nowTime)
{
	if(!m_notificationCloseTime.IsValid())
	{
		return;
	}

	if(m_isNotificationCloseDate && i_nowTime >= m_notificationCloseTime)
	{
		HideUiElement(m_notificationPanelId);	

		m_notificationCloseDate = Date {};
		m_notificationCloseTime = Time {};
	}
}

bool MainComponent::FindAllUiElements(const AZ::EntityId& i_mainCanvasId)
{
	if(!i_mainCanvasId.IsValid() || i_mainCanvasId == m_mainCanvasId)
	{
		return false;
	}

	m_mainCanvasId = i_mainCanvasId;

	m_dateYearTextId = FindUiElement(UI_DATE_YEAR_TEXT);
	m_dateMonthTextId = FindUiElement(UI_DATE_MONTH_TEXT);
	m_dateDayTextId = FindUiElement(UI_DATE_DAY_TEXT);

	m_timeHoursTextId = FindUiElement(UI_TIME_HOURS_TEXT);
	m_timeMinutesTextId = FindUiElement(UI_TIME_MINUTES_TEXT);
	m_timeSecondsTextId = FindUiElement(UI_TIME_SECONDS_TEXT);

	m_notificationPanelId = FindUiElement(UI_NOTIFICATION_PANEL);

	m_currentAlarmTextId = FindUiElement(UI_CURRENT_ALARM_TEXT);
	m_nextAlarmTextId = FindUiElement(UI_NEXT_ALARM_TEXT);

	return true;
}

AZ::EntityId MainComponent::FindUiElement(const char* i_elementName) const
{
	AZ::EntityId elementId;
	EBUS_EVENT_ID_RESULT(elementId, m_mainCanvasId, UiCanvasBus, FindElementEntityIdByName, AZStd::string { i_elementName });

	return elementId;
}

void MainComponent::EditTextUiElement(const AZ::EntityId& i_textElementId, const AZStd::string& i_textValue) const
{	
	EBUS_EVENT_ID(i_textElementId, UiTextBus, SetText, i_textValue);
}

void MainComponent::ShowUiElement(const AZ::EntityId& i_elementId) const
{
	EBUS_EVENT_ID(i_elementId, UiElementBus, SetIsEnabled, true);
}

void MainComponent::HideUiElement(const AZ::EntityId& i_elementId) const
{
	EBUS_EVENT_ID(i_elementId, UiElementBus, SetIsEnabled, false);
}
