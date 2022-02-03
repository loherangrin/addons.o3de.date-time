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
#include <AzCore/std/containers/queue.h>
#include <AzCore/std/containers/set.h>
#include <AzCore/std/containers/vector.h>

#include <DateTime/EBuses/AlarmBus.hpp>
#include <DateTime/EBuses/DateBus.hpp>
#include <DateTime/EBuses/TimeBus.hpp>

#include "../Types/AlarmHandle.hpp"
#include "AlarmComponentConfig.hpp"

namespace Loherangrin::Gems::DateTime
{
	class AlarmComponentEventHandler;
}


namespace Loherangrin::Gems::DateTime
{
	class AlarmComponent
		: public AZ::Component
		, protected AlarmRequestBus::Handler
		, protected DateNotificationBus::Handler
		, protected TimeNotificationBus::Handler
	{
	public:
		using ConfigClass = AlarmComponentConfig;

	public:
		AZ_COMPONENT(AlarmComponent, "{50ADA7E3-B494-4AFD-AB1F-1B85694D78D9}");
		static void Reflect(AZ::ReflectContext* io_context);

		static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& io_provided);
		static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& io_incompatible);
		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& io_required);
		static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& io_dependent);

		AlarmComponent();
		AlarmComponent(const ConfigClass& i_config);
		AlarmComponent(ConfigClass&& io_config);

	protected:
		// AZ::Component
		void Init() override;
		void Activate() override;
		void Deactivate() override;

		// DateNotificationBus
		void OnDateDayChanged(const Date& i_nowDate) override;
		void OnDateReset(const Date& i_startDate) override;

		// TimeNotificationBus
		void OnTimeSecondChanged(const Time& i_nowTime) override;
		void OnTimeSpeedChanged(float i_speed) override;
		void OnTimeReset(const Time& i_startTime) override;

		// AlarmRequestBus
		AlarmId SetAlarmToExecute(const DateRecurrenceRule& i_dateRule, const TimeRecurrenceRule& i_timeRule, const AlarmCallback& i_callback) override;
		AlarmId SetAlarmToNotify(const DateRecurrenceRule& i_dateRule, const TimeRecurrenceRule& i_timeRule, const AZ::EntityId& i_requester) override;
		AlarmId SetAlarmToNotify(const DateRecurrenceRuleBehaviorHandle& i_dateRuleHandle, const TimeRecurrenceRuleBehaviorHandle& i_timeRuleHandle, const AZ::EntityId& i_requester) override;

		bool CancelAlarm(AlarmId i_alarmId) override;
		void CancelAllAlarms() override;

		bool SilenceAlarm(AlarmId i_alarmId, bool i_silent) override;
		void SilenceAllAlarms(bool i_silent) override;

		AZStd::pair<Date, Time> GetWhenNextAlarm() const override;
		Date GetNextAlarmDate() const override;
		Time GetNextAlarmTime() const override;

	private:
		struct AlarmComparator
		{
			bool operator()(const AlarmHandle& i_lhs, const AlarmHandle& i_rhs) const;
		};	

		class AlarmPrioritizer
		{
		public:
			AlarmPrioritizer(bool i_reverse);

			bool operator()(const AlarmHandle* i_lhs, const AlarmHandle* i_rhs) const;

		private:
			bool m_isReversedTime;
		};

		using AlarmQueue = AZStd::priority_queue<AlarmHandle*, AZStd::vector<AlarmHandle*>, AlarmPrioritizer>;
		using AlarmSet = AZStd::set<AlarmHandle, AlarmComparator>;

		AlarmId AddAlarm(AlarmHandle&& io_alarmHandle, const Time& i_nowTime);
		void AddAlarmsFromConfig(const AZStd::vector<AlarmConfig>& i_alarmConfigs);
		bool DiscardPastAlarmRepeats(Alarm& io_alarm, const Time& i_nowTime);

		bool CalculateTimeDirection(float i_timeSpeed);
		void ProcessAlarms(const Time& i_nowTime);
		void ResetAlarms(const Time& i_nowTime);

		void SynchronizeWithDateAndTimeComponents();
		Time GetCurrentTime() const;

		bool m_isReversedTime;
		AlarmPrioritizer m_alarmPrioritizer;

		AlarmSet m_allAlarms;
		AlarmQueue m_orderedAlarms;

		bool m_hasDate;
		Date m_nowDate;
		Time::Hour m_lastHours;

		AlarmComponentConfig m_config;
		friend AlarmComponentEventHandler;
	};

} // Loherangrin::Gems::DateTime
