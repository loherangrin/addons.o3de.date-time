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

#include <AzCore/Component/Component.h>

// -------------------------------------
// [INSTRUCTIONS]: #1
#include <DateTime/EBuses/AlarmBus.hpp>
#include <DateTime/EBuses/DateBus.hpp>
#include <DateTime/EBuses/TimeBus.hpp>
// -------------------------------------

#include <LyShine/Bus/World/UiCanvasRefBus.h>

namespace Loherangrin::Gems::DateTime
{
	class MainComponent
		: public AZ::Component
		, protected UiCanvasRefNotificationBus::Handler
		// -------------------------------------
		// [INSTRUCTIONS]: #2
		, protected AlarmNotificationBus::Handler
		, protected DateNotificationBus::Handler
		, protected TimeNotificationBus::Handler
		// -------------------------------------
	{
	public:
		AZ_COMPONENT(MainComponent, "{E7970F62-3F13-4FC6-A21B-8C158BCF68F9}");
		static void Reflect(AZ::ReflectContext* io_context);

		static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& io_provided);
		static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& io_incompatible);
		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& io_required);
		static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& io_dependent);

	protected:
		// AZ::Component
		void Init() override;
		void Activate() override;
		void Deactivate() override;

		// UiCanvasRefNotificationBus
		void OnCanvasRefChanged(AZ::EntityId i_uiCanvasRefEntity, AZ::EntityId i_uiCanvasEntity) override;

		// -------------------------------------
		// [INSTRUCTIONS]: #3

		// AlarmNotificationBus
		void OnAlarmBell(const Date& i_date, const Time& i_time) override;

		// DateNotificationBus
		void OnDateDayChanged(const Date& i_nowDate) override;

		// TimeNotificationBus
		void OnTimeSecondChanged(const Time& i_nowTime);

		// -------------------------------------

	private:
		static AZStd::string FormatMessageWithDateTime(const AZStd::string& i_message, const Date& i_date, const Time& i_time);

		void AskForCurrentDate();
		void AskForCurrentTime();
		void AskForNextAlarm() const;

		void ShowNotification(const AZStd::string& i_message, const Date& i_date, const Time& i_time);
		void CloseNotificationIfExpired(const Time& i_nowTime);

		bool FindAllUiElements(const AZ::EntityId& i_mainCanvasId);
		AZ::EntityId FindUiElement(const char* i_elementName) const;

		void EditTextUiElement(const AZ::EntityId& i_textElementId, const AZStd::string& i_textValue) const;

		void ShowUiElement(const AZ::EntityId& i_elementId) const;
		void HideUiElement(const AZ::EntityId& i_elementId) const;

		bool m_isAlarmComponentEnabled;
		bool m_isDateComponentEnabled;
		bool m_isTimeComponentEnabled;

		AZ::EntityId m_mainCanvasId;

		AZ::EntityId m_dateYearTextId;
		AZ::EntityId m_dateMonthTextId;
		AZ::EntityId m_dateDayTextId;

		AZ::EntityId m_timeHoursTextId;
		AZ::EntityId m_timeMinutesTextId;
		AZ::EntityId m_timeSecondsTextId;

		AZ::EntityId m_notificationPanelId;
		AZ::EntityId m_currentAlarmTextId;
		AZ::EntityId m_nextAlarmTextId;

		Date m_notificationCloseDate;
		Time m_notificationCloseTime;
		bool m_isNotificationCloseDate;

		static constexpr const char* UI_DATE_YEAR_TEXT = "YearFieldValue";
		static constexpr const char* UI_DATE_MONTH_TEXT = "MonthFieldValue";
		static constexpr const char* UI_DATE_DAY_TEXT = "DayFieldValue";
		static constexpr const char* UI_TIME_HOURS_TEXT = "HoursFieldValue";
		static constexpr const char* UI_TIME_MINUTES_TEXT = "MinutesFieldValue";
		static constexpr const char* UI_TIME_SECONDS_TEXT = "SecondsFieldValue";

		static constexpr const char* UI_NOTIFICATION_PANEL = "NotificationPanel";
		static constexpr const char* UI_CURRENT_ALARM_TEXT = "CurrentAlarmValue";
		static constexpr const char* UI_NEXT_ALARM_TEXT = "NextAlarmValue";

		static constexpr TimeS MAX_ALARM_NOTIFICATION_TIMEOUT { 2.f };
	};

} // Loherangrin::Gems::DateTime
