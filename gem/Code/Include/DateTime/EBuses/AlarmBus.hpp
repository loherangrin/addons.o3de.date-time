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

#include <AzCore/EBus/EBus.h>
#include <AzCore/std/utils.h>

#include <DateTime/Types/Alarm.hpp>
#include <DateTime/Types/Date.hpp>
#include <DateTime/Types/RecurrenceRules.hpp>
#include <DateTime/Types/Time.hpp>


namespace Loherangrin::Gems::DateTime
{
	class AlarmRequests
	{
	public:
		AZ_RTTI(AlarmRequests, "{DB5DC454-A29E-4C6C-87BC-ABD399113A11}");
		virtual ~AlarmRequests() = default;

		virtual AlarmId SetAlarmToExecute(const DateRecurrenceRule& i_dateRule, const TimeRecurrenceRule& i_timeRule, const AlarmCallback& i_callback) = 0;
		virtual AlarmId SetAlarmToNotify(const DateRecurrenceRule& i_dateRule, const TimeRecurrenceRule& i_timeRule, const AZ::EntityId& i_requester) = 0;
		virtual AlarmId SetAlarmToNotify(const DateRecurrenceRuleBehaviorHandle& i_dateRuleHandle, const TimeRecurrenceRuleBehaviorHandle& i_timeRuleHandle, const AZ::EntityId& i_requester) = 0;

		virtual bool CancelAlarm(AlarmId i_alarmId) = 0;
		virtual void CancelAllAlarms() = 0;

		virtual bool SilenceAlarm(AlarmId i_alarmId, bool i_silent) = 0;
		virtual void SilenceAllAlarms(bool i_silent) = 0;

		virtual AZStd::pair<Date, Time> GetWhenNextAlarm() const = 0;
		virtual Date GetNextAlarmDate() const = 0;
		virtual Time GetNextAlarmTime() const = 0;
	};
	
	class AlarmRequestBusTraits
		: public AZ::EBusTraits
	{
	public:
		// EBusTraits
		static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
		static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
	};

	using AlarmRequestBus = AZ::EBus<AlarmRequests, AlarmRequestBusTraits>;

	// ---

    class AlarmNotifications
    {
    public:
        AZ_RTTI(AlarmNotifications, "{B932C604-C7FD-4BB6-98D8-F73D8365A54E}");
        virtual ~AlarmNotifications() = default;

		virtual void OnAlarmBell(const Date& i_date, const Time& i_time){}
    };
    
    class AlarmNotificationBusTraits
        : public AZ::EBusTraits
    {
    public:
		// EBusTraits
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;
        using BusIdType = AZ::EntityId;
    };

    using AlarmNotificationBus = AZ::EBus<AlarmNotifications, AlarmNotificationBusTraits>;

} // Loherangrin::Gems::DateTime
