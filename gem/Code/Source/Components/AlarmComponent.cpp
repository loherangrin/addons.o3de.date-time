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

#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Serialization/SerializeContext.h>

#include <DateTime/Constants/DateTimeConstants.hpp>

#include "../Types/Alarm.hpp"

#include "AlarmComponent.hpp"

using Loherangrin::Gems::DateTime::Date;
using Loherangrin::Gems::DateTime::Time;
using Loherangrin::Gems::DateTime::AlarmId;
using Loherangrin::Gems::DateTime::AlarmComponent;


namespace Loherangrin::Gems::DateTime
{
	class AlarmComponentEventHandler
		: public AZ::SerializeContext::IEventHandler
	{
		void OnWriteEnd(void* io_classPtr)
		{
			auto* alarmComponent = reinterpret_cast<AlarmComponent*>(io_classPtr);

			alarmComponent->CancelAllAlarms();
			alarmComponent->AddAlarmsFromConfig(alarmComponent->m_config.m_alarms);
		}
	};

	class AlarmNotificationBusBehaviorHandler
		: public AlarmNotificationBus::Handler
		, public AZ::BehaviorEBusHandler
	{
	public:
		AZ_EBUS_BEHAVIOR_BINDER(AlarmNotificationBusBehaviorHandler, "{4E736648-7F9A-493A-811C-95AD6E855231}", AZ::SystemAllocator,
			OnAlarmBell
		);

		void OnAlarmBell(const Date& i_date, const Time& i_time) override
		{
			Call(FN_OnAlarmBell, i_date, i_time);
		}
	};

} // Loherangrin::Gems::DateTime

void AlarmComponent::Reflect(AZ::ReflectContext* io_context)
{
	ConfigClass::Reflect(io_context);

	if(auto serializeContext = azrtti_cast<AZ::SerializeContext*>(io_context))
	{
		serializeContext->Class<AlarmComponent, AZ::Component>()
			->Version(0)
			->EventHandler<AlarmComponentEventHandler>()
			->Field("Configuration", &AlarmComponent::m_config)
		;
	}

	if(auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(io_context))
	{
		behaviorContext->EBus<AlarmRequestBus>("AlarmRequestBus")
			->Attribute(AZ::Script::Attributes::Module, ScriptAttributes::MODULE)
			->Attribute(AZ::Script::Attributes::Category, ScriptAttributes::CATEGORY)

			->Event<AlarmId (AlarmRequestBus::Events::*)(const DateRecurrenceRuleBehaviorHandle&, const TimeRecurrenceRuleBehaviorHandle&, const AZ::EntityId&)>("SetAlarmToNotify", &AlarmRequestBus::Events::SetAlarmToNotify)

			->Event("CancelAlarm", &AlarmRequestBus::Events::CancelAlarm)
			->Event("CancelAllAlarms", &AlarmRequestBus::Events::CancelAllAlarms)
			->Event("SilenceAlarm", &AlarmRequestBus::Events::SilenceAlarm)
			->Event("SilenceAllAlarms", &AlarmRequestBus::Events::SilenceAllAlarms)

			->Event("GetWhenNextAlarm", &AlarmRequestBus::Events::GetWhenNextAlarm)
			->Event("GetNextAlarmDate", &AlarmRequestBus::Events::GetNextAlarmDate)
			->Event("GetNextAlarmTime", &AlarmRequestBus::Events::GetNextAlarmTime)
		;

		behaviorContext->EBus<AlarmNotificationBus>("AlarmNotificationBus")
			->Attribute(AZ::Script::Attributes::Module, ScriptAttributes::MODULE)
			->Attribute(AZ::Script::Attributes::Category, ScriptAttributes::CATEGORY)

			->Handler<AlarmNotificationBusBehaviorHandler>()
		;
	}
}

void AlarmComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& io_provided)
{
	io_provided.push_back(Services::ALARM);
}

void AlarmComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& io_incompatible)
{
	io_incompatible.push_back(Services::ALARM);
}

void AlarmComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& io_required)
{
	io_required.push_back(Services::TIME);
}

void AlarmComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& io_dependent)
{
	io_dependent.push_back(Services::DATE);
}

AlarmComponent::AlarmComponent()
	: m_isReversedTime { false }
	, m_alarmPrioritizer { m_isReversedTime }
	, m_allAlarms {}
	, m_orderedAlarms { m_alarmPrioritizer }
	, m_nowDate {}
	, m_config {}
{}

AlarmComponent::AlarmComponent(const ConfigClass& i_config)
	: m_isReversedTime { false }
	, m_alarmPrioritizer { m_isReversedTime }
	, m_allAlarms {}
	, m_orderedAlarms { m_alarmPrioritizer }
	, m_nowDate {}
	, m_config { i_config }
{
	AddAlarmsFromConfig(m_config.m_alarms);
}

AlarmComponent::AlarmComponent(ConfigClass&& io_config)
	: m_isReversedTime { false }
	, m_alarmPrioritizer { m_isReversedTime }
	, m_allAlarms {}
	, m_orderedAlarms { m_alarmPrioritizer }
	, m_nowDate {}
	, m_config { AZStd::move(io_config) }
{
	AddAlarmsFromConfig(m_config.m_alarms);
}

void AlarmComponent::Init()
{}

void AlarmComponent::Activate()
{
	SynchronizeWithDateAndTimeComponents();

	DateNotificationBus::Handler::BusConnect();
	TimeNotificationBus::Handler::BusConnect();
	AlarmRequestBus::Handler::BusConnect();
}

void AlarmComponent::Deactivate()
{
	AlarmRequestBus::Handler::BusDisconnect();
	TimeNotificationBus::Handler::BusDisconnect();
	DateNotificationBus::Handler::BusDisconnect();
}

void AlarmComponent::OnDateDayChanged(const Date& i_nowDate)
{
	m_nowDate = i_nowDate;
}

void AlarmComponent::OnDateReset(const Date& i_startDate)
{
	m_nowDate = i_startDate;
	Time nowTime = GetCurrentTime();

	ResetAlarms(nowTime);
}

void AlarmComponent::OnTimeSecondChanged(const Time& i_nowTime)
{
	ProcessAlarms(i_nowTime);
}

void AlarmComponent::OnTimeSpeedChanged(float i_speed)
{
	bool changed = CalculateTimeDirection(i_speed);
	if(!changed)
	{
		return;
	}

	Time nowTime;
	EBUS_EVENT_RESULT(nowTime, TimeRequestBus, GetCurrentTime);
	if(nowTime.IsValid())
	{
		ResetAlarms(nowTime);
	}
}

void AlarmComponent::OnTimeReset(const Time& i_startTime)
{
	ResetAlarms(i_startTime);
}

AlarmId AlarmComponent::SetAlarmToExecute(const DateRecurrenceRule& i_dateRule, const TimeRecurrenceRule& i_timeRule, const AlarmCallback& i_callback)
{
	Time nowTime = GetCurrentTime();

	AlarmHandle alarmHandle = Alarm::CreateInstance(i_dateRule, i_timeRule, i_callback);
	const AlarmId alarmId = AddAlarm(AZStd::move(alarmHandle), nowTime);

	return alarmId;
}

AlarmId AlarmComponent::SetAlarmToNotify(const DateRecurrenceRule& i_dateRule, const TimeRecurrenceRule& i_timeRule, const AZ::EntityId& i_requesterId)
{
	Time nowTime = GetCurrentTime();

	AlarmHandle alarmHandle = Alarm::CreateInstance(i_dateRule, i_timeRule, i_requesterId);
	const AlarmId alarmId = AddAlarm(AZStd::move(alarmHandle), nowTime);

	return alarmId;
}

AlarmId AlarmComponent::SetAlarmToNotify(const DateRecurrenceRuleBehaviorHandle& i_dateRuleHandle, const TimeRecurrenceRuleBehaviorHandle& i_timeRuleHandle, const AZ::EntityId& i_requester)
{
	const DateRecurrenceRule* dateRule = i_dateRuleHandle.GetRule();
	const TimeRecurrenceRule* timeRule = i_timeRuleHandle.GetRule();

	if(!dateRule || !timeRule)
	{
		AZ_Assert(false, "Recurrence rule cannot be null");
		return 0;
	}

	return SetAlarmToNotify(*dateRule, *timeRule, i_requester);
}

bool AlarmComponent::CancelAlarm(AlarmId i_alarmId)
{
	auto result = m_allAlarms.find(Alarm::CreateEmpty(i_alarmId));
	if(result == m_allAlarms.end())
	{
		return false;
	}

	Alarm* alarm = result->GetAlarm();
	AZ_Assert(alarm != nullptr, "An empty handler cannot be stored in the alarm queue");

	alarm->Cancel();

	return true;
}

void AlarmComponent::CancelAllAlarms()
{
	m_orderedAlarms = AlarmQueue { m_alarmPrioritizer };
	m_allAlarms.clear();
}

bool AlarmComponent::SilenceAlarm(AlarmId i_alarmId, bool i_silent)
{
	auto result = m_allAlarms.find(Alarm::CreateEmpty(i_alarmId));
	if(result == m_allAlarms.end())
	{
		return false;
	}

	Alarm* alarm = result->GetAlarm();
	AZ_Assert(alarm != nullptr, "An empty handler cannot be stored in the alarm queue");

	alarm->Silence(i_silent);

	return true;
}

void AlarmComponent::SilenceAllAlarms(bool i_silent)
{
	for(auto& alarmHandle : m_allAlarms)
	{
		Alarm* alarm = alarmHandle.GetAlarm();
		AZ_Assert(alarm != nullptr, "An empty handler cannot be stored in the alarm queue");

		alarm->Silence(i_silent);
	}
}

AZStd::pair<Date, Time> AlarmComponent::GetWhenNextAlarm() const
{
	if(m_orderedAlarms.empty())
	{
		return AZStd::make_pair(Date {}, Time {});
	}

	AlarmHandle* const alarmHandle = m_orderedAlarms.top();
	Alarm* alarm = alarmHandle->GetAlarm();
	AZ_Assert(alarm != nullptr, "An empty handler cannot be stored in the alarm queue");

	return AZStd::make_pair(alarm->GetDate(), alarm->GetTime());
}

Date AlarmComponent::GetNextAlarmDate() const
{
	AZStd::pair<Date, Time> nextAlarm = GetWhenNextAlarm();

	return nextAlarm.first;
}

Time AlarmComponent::GetNextAlarmTime() const
{
	AZStd::pair<Date, Time> nextAlarm = GetWhenNextAlarm();

	return nextAlarm.second;
}

bool AlarmComponent::DiscardPastAlarmRepeats(Alarm& io_alarm, const Time& i_nowTime)
{
	const bool moved = io_alarm.GoToRepeatAt(m_nowDate, i_nowTime, m_isReversedTime);

	return moved;
}

AlarmId AlarmComponent::AddAlarm(AlarmHandle&& io_alarmHandle, const Time& i_nowTime)
{
	auto result = m_allAlarms.find(io_alarmHandle);
	if(result != m_allAlarms.end())
	{
		const AlarmId existingAlarmId = result->GetAlarmId();
		return existingAlarmId;
	}

	AlarmHandle candidateAlarmHandle = AlarmHandle::ReWrapAlarm(AZStd::move(io_alarmHandle));
	Alarm* candidateAlarm = candidateAlarmHandle.GetAlarm();
	AZ_Assert(candidateAlarm != nullptr, "New alarm cannot be empty");

	const bool hasRepeats = DiscardPastAlarmRepeats(*candidateAlarm, i_nowTime);
	if(!hasRepeats)
	{
		return 0;
	}

	AlarmId nTries = 0;
	const AlarmId maxTries = AZStd::numeric_limits<AlarmId>::max();
	do
	{
		auto [ newAlarmHandle, inserted ] = m_allAlarms.insert(AZStd::move(candidateAlarmHandle));
		if(!inserted)
		{
			candidateAlarmHandle = AlarmHandle::ReWrapAlarm(AZStd::move(candidateAlarmHandle));
			++nTries;

			continue;
		}

		auto newAlarmHandleAddress = &(*newAlarmHandle);
		m_orderedAlarms.emplace(newAlarmHandleAddress);

		const AlarmId newAlarmId = newAlarmHandle->GetAlarmId();
		return newAlarmId;
	}
	while(nTries < maxTries);

	AZ_Assert(false, "Unable to find a free ID to insert the new alarm");
	return 0;
}

void AlarmComponent::AddAlarmsFromConfig(const AZStd::vector<AlarmConfig>& i_alarmConfigs)
{
	const Time emptyTime;

	for(const auto& alarmConfig : i_alarmConfigs)
	{
		AZStd::unique_ptr<DateRecurrenceRule> dateRule = alarmConfig.m_dateRule.CalculateRecurrenceRule();
		AZStd::unique_ptr<TimeRecurrenceRule> timeRule = alarmConfig.m_timeRule.CalculateRecurrenceRule();

		AlarmHandle alarmHandle;
		switch(alarmConfig.m_action)
		{
			case AlarmConfig::AlarmActionType::NOTIFY_REQUESTER:
			{
				alarmHandle = Alarm::CreateInstance(AZStd::move(dateRule), AZStd::move(timeRule), alarmConfig.m_actionRequesterId);
			}
			break;

			default:
			{
				AZ_Assert(false, "Unsupported action: %d", alarmConfig.m_action);
				return;
			}
		}

		if(alarmConfig.m_isSilent)
		{
			Alarm* alarm = alarmHandle.GetAlarm();
			alarm->Silence(true);
		}

		AddAlarm(AZStd::move(alarmHandle), emptyTime);
	}
}

bool AlarmComponent::CalculateTimeDirection(float i_timeSpeed)
{
	if(AZ::IsClose(i_timeSpeed, 0.0f, AZ::Constants::FloatEpsilon))
	{
		return false;
	}

	bool oldReversedTime = m_isReversedTime;

	m_isReversedTime = (i_timeSpeed < 0.f);

	return (m_isReversedTime != oldReversedTime);
}

void AlarmComponent::ProcessAlarms(const Time& i_nowTime)
{
	while(!m_orderedAlarms.empty())
	{
		AlarmHandle* const alarmHandle = m_orderedAlarms.top();
		Alarm* alarm = alarmHandle->GetAlarm();
		AZ_Assert(alarm != nullptr, "An empty handler cannot be stored in the alarm queue");

		if(!alarm->IsGoneOff(m_nowDate, i_nowTime, m_isReversedTime))
		{
			if(!m_hasDate)
			{
				if(m_lastHours > i_nowTime.GetHours())
				{
					for(auto& alarmHandle : m_allAlarms)
					{
						Alarm* alarm = alarmHandle.GetAlarm();
						AZ_Assert(alarm != nullptr, "An empty handler cannot be stored in the alarm queue");

						alarm->ClearExtension();
					}
				}

				m_lastHours = i_nowTime.GetHours();
			}

			return;
		}

		bool hasRepeat;
		if(!alarm->IsCanceled())
		{
			if(!alarm->IsSilent())
			{
				if(const AZ::EntityId* requesterId = alarm->GetActionIfType<AZ::EntityId>())
				{
					EBUS_EVENT_ID(*requesterId, AlarmNotificationBus, OnAlarmBell, alarm->GetDate(), alarm->GetTime());
				}
				else if(const AlarmCallback* callback = alarm->GetActionIfType<AlarmCallback>())
				{
					(*callback)(alarm->GetDate(), alarm->GetTime());
				}
				else
				{
					AZ_Assert(false, "Unexpected action type: %d", alarm->GetActionType());
					return;
				}
			}
			
			hasRepeat = alarm->NextRepeat(m_isReversedTime);
		}
		else
		{
			hasRepeat = false;
		}

		m_orderedAlarms.pop();
		if(hasRepeat)
		{
			m_orderedAlarms.emplace(alarmHandle);
		}
		else
		{
			auto result = m_allAlarms.find(*alarmHandle);
			if(result != m_allAlarms.end())
			{
				m_allAlarms.erase(result);
			}
		}
	}
}

void AlarmComponent::ResetAlarms(const Time& i_nowTime)
{
	if(!m_orderedAlarms.empty())
	{
		m_orderedAlarms = AlarmQueue { m_alarmPrioritizer };
	}

	for(auto& alarmHandle : m_allAlarms)
	{
		Alarm* alarm = alarmHandle.GetAlarm();
		AZ_Assert(alarm != nullptr, "An empty handler cannot be stored in the alarm queue");
		alarm->Reset();

		const bool hasRepeats = DiscardPastAlarmRepeats(*alarm, i_nowTime);
		if(!hasRepeats)
		{
			continue;
		}

		m_orderedAlarms.emplace(&alarmHandle);
	}
}

void AlarmComponent::SynchronizeWithDateAndTimeComponents()
{
	Time nowTime;
	EBUS_EVENT_RESULT(nowTime, TimeRequestBus, GetCurrentTime);
	if(!nowTime.IsValid())
	{
		AZ_Assert(false, "A running TimeComponent instance is required to process alarms, but none was found");
		return;
	}

	float timeSpeed;
	EBUS_EVENT_RESULT(timeSpeed, TimeRequestBus, GetTimeSpeed);
	CalculateTimeDirection(timeSpeed);

	Date nowDate;
	EBUS_EVENT_RESULT(nowDate, DateRequestBus, GetCurrentDate);
	m_hasDate = nowDate.IsValid();
	if(!m_hasDate)
	{
		AZ_Warning("AlarmLevelComponent", false, "Unable to find a DateComponent instance to synchronize with. Alarms will only be checked against time");
	}

	m_nowDate = nowDate;

	ResetAlarms(nowTime);
}

Time AlarmComponent::GetCurrentTime() const
{
	Time nowTime;
	EBUS_EVENT_RESULT(nowTime, TimeRequestBus, GetCurrentTime);
	AZ_Assert(nowTime.IsValid(), "A running TimeComponent instance is required to add a new alarm, but none was found");

	return nowTime;
}

// ---

bool AlarmComponent::AlarmComparator::operator()(const AlarmHandle& i_lhs, const AlarmHandle& i_rhs) const
{
	return (i_lhs.CompareTo(i_rhs) == AlarmComparison::LESS_THAN);
}

AlarmComponent::AlarmPrioritizer::AlarmPrioritizer(bool i_reverse)
	: m_isReversedTime { i_reverse }
{}

bool AlarmComponent::AlarmPrioritizer::operator()(const AlarmHandle* i_lhs, const AlarmHandle* i_rhs) const
{
	return (i_lhs->CompareTo(*i_rhs) == ((m_isReversedTime) ? AlarmComparison::LESS_THAN : AlarmComparison::GREATER_THAN));
}
