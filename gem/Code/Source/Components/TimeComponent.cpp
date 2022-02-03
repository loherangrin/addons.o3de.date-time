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
#include <AzCore/std/limits.h>

#include <DateTime/Constants/DateTimeConstants.hpp>

#include "../EBuses/DateInternalBus.hpp"

#include "TimeComponent.hpp"

using Loherangrin::Gems::DateTime::Time;
using Loherangrin::Gems::DateTime::TimeComponent;


namespace Loherangrin::Gems::DateTime
{
	class TimeNotificationBusBehaviorHandler
		: public TimeNotificationBus::Handler
		, public AZ::BehaviorEBusHandler
	{
	public:
		AZ_EBUS_BEHAVIOR_BINDER(TimeNotificationBusBehaviorHandler, "{8AD5AEC1-22CE-44E9-937F-06EEE855F21D}", AZ::SystemAllocator,
			OnTimeHourChanged,
			OnTimeMinuteChanged,
			OnTimeSecondChanged,
			OnTimeScaleChanged,
			OnTimeSpeedChanged,
			OnTimeReset
		);

		void OnTimeHourChanged(const Time& i_nowTime) override
		{
			Call(FN_OnTimeHourChanged, i_nowTime);
		}

		void OnTimeMinuteChanged(const Time& i_nowTime) override
		{
			Call(FN_OnTimeMinuteChanged, i_nowTime);
		}

		void OnTimeSecondChanged(const Time& i_nowTime) override
		{
			Call(FN_OnTimeSecondChanged, i_nowTime);
		}

		void OnTimeScaleChanged(float i_scale) override
		{
			Call(FN_OnTimeScaleChanged, i_scale);
		}

		void OnTimeSpeedChanged(float i_speed) override
		{
			Call(FN_OnTimeSpeedChanged, i_speed);
		}

		void OnTimeReset(const Time& i_startTime) override
		{
			Call(FN_OnTimeReset, i_startTime);
		}
	};

} // Loherangrin::Gems::DateTime

void TimeComponent::Reflect(AZ::ReflectContext* io_context)
{
	ConfigClass::Reflect(io_context);

	if(auto serializeContext = azrtti_cast<AZ::SerializeContext*>(io_context))
	{
		serializeContext->Class<TimeComponent, AZ::Component>()
			->Version(0)
			->Field("NowTime", &TimeComponent::m_nowTime)
			->Field("NowFraction", &TimeComponent::m_nowFraction)
			->Field("TimeScale", &TimeComponent::m_timeScale)
			->Field("TimeSpeed", &TimeComponent::m_timeSpeed)
			->Field("HasDate", &TimeComponent::m_hasDate)
		;
	}

	if(auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(io_context))
	{
		behaviorContext->EBus<TimeRequestBus>("TimeRequestBus")
			->Attribute(AZ::Script::Attributes::Module, ScriptAttributes::MODULE)
			->Attribute(AZ::Script::Attributes::Category, ScriptAttributes::CATEGORY)

			->Event("GetCurrentTime", &TimeRequestBus::Events::GetCurrentTime)
			->Event("GetTimeScale", &TimeRequestBus::Events::GetTimeScale)
			->Event("GetTimeSpeed", &TimeRequestBus::Events::GetTimeSpeed)

			->Event("SetCurrentTime", &TimeRequestBus::Events::SetCurrentTime)
			->Event("SetTimeScale", &TimeRequestBus::Events::SetTimeScale)
			->Event("SetTimeSpeed", &TimeRequestBus::Events::SetTimeSpeed)

			->Event("ConvertToRealTime", &TimeRequestBus::Events::ConvertToRealTime)
			->Event("ConvertToSceneTime", &TimeRequestBus::Events::ConvertToSceneTime)

			->VirtualProperty("CurrentTime", "GetCurrentTime", "SetCurrentTime")
			->VirtualProperty("TimeScale", "GetTimeScale", "SetTimeScale")
			->VirtualProperty("TimeSpeed", "GetTimeSpeed", "SetTimeSpeed")
			;

		behaviorContext->EBus<TimeNotificationBus>("TimeNotificationBus")
			->Attribute(AZ::Script::Attributes::Module, ScriptAttributes::MODULE)
			->Attribute(AZ::Script::Attributes::Category, ScriptAttributes::CATEGORY)

			->Handler<TimeNotificationBusBehaviorHandler>()
			;
	}
}

void TimeComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& io_provided)
{
	io_provided.push_back(Services::TIME);
}

void TimeComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& io_incompatible)
{
	io_incompatible.push_back(Services::TIME);
}

void TimeComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& io_required)
{}

void TimeComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& io_dependent)
{
	io_dependent.push_back(Services::DATE);
}

TimeComponent::TimeComponent()
	: TimeComponent(ConfigClass {})
{}

TimeComponent::TimeComponent(const ConfigClass& i_config)
	: m_nowTime { i_config.m_nowTime }
	, m_nowFraction { 0.f }
	, m_timeScale { i_config.m_timeScale }
	, m_timeSpeed { i_config.m_timeSpeed }
	, m_isTicking { false }
	, m_hasDate { i_config.m_isEnabled }
{}

TimeComponent::TimeComponent(ConfigClass&& io_config)
	: m_nowTime { AZStd::move(io_config.m_nowTime) }
	, m_nowFraction { 0.f }
	, m_timeScale { AZStd::exchange(io_config.m_timeScale, 1.f) }
	, m_timeSpeed { AZStd::exchange(io_config.m_timeSpeed, 0.f) }
	, m_isTicking { false }
	, m_hasDate { AZStd::exchange(io_config.m_isEnabled, false) }
{}

void TimeComponent::Init()
{
	CalculateIsTicking();
}

void TimeComponent::Activate()
{
	if(m_hasDate)
	{
		CheckDateComponentExists();
	}

	TimeRequestBus::Handler::BusConnect();

	if(m_isTicking)
	{
		AZ::TickBus::Handler::BusConnect();
	}
}

void TimeComponent::Deactivate()
{
	if(m_isTicking)
	{
		AZ::TickBus::Handler::BusDisconnect();
	}

	TimeRequestBus::Handler::BusDisconnect();
}

void TimeComponent::OnTick(float i_deltaTime, AZ::ScriptTimePoint i_time)
{
	AZ_Assert(m_isTicking, "OnTick must not be called when the timer is inactive");

	TimeChange timeChange = TimeChange::NONE;

	m_nowFraction += i_deltaTime * m_timeSpeed * m_timeScale;
	const TimeS fraction = AZStd::abs(m_nowFraction);

	if(fraction < 1.f)
	{}
	else if(fraction < 2.f)
	{
		if(m_timeSpeed > 0.f)
		{
			m_nowFraction -= 1.f;
			timeChange = m_nowTime.AddSecond();

			if(m_hasDate && timeChange == TimeChange::DAY)
			{
				EBUS_EVENT(DateInternalRequestBus, AddDayToDate);
			}
		}
		else
		{
			m_nowFraction += 1.f;
			timeChange = m_nowTime.SubtractSecond();

			if(m_hasDate && timeChange == TimeChange::DAY)
			{
				EBUS_EVENT(DateInternalRequestBus, SubtractDayToDate);
			}
		}
	}
	else if(fraction < Time::MINUTE_TO_SECONDS)
	{
		const TimeS seconds = AZStd::floor(fraction);
		if(m_timeSpeed > 0.f)
		{
			m_nowFraction -= seconds;
			timeChange = m_nowTime.AddSeconds(seconds);

			if(m_hasDate && timeChange == TimeChange::DAY)
			{
				EBUS_EVENT(DateInternalRequestBus, AddDayToDate);
			}
		}
		else
		{
			m_nowFraction += seconds;
			timeChange = m_nowTime.SubtractSeconds(seconds);

			if(m_hasDate && timeChange == TimeChange::DAY)
			{
				EBUS_EVENT(DateInternalRequestBus, SubtractDayToDate);
			}
		}
	}
	else
	{
		TimeS timeSeconds = m_nowFraction;
		EBUS_EVENT_RESULT(timeSeconds, DateInternalRequestBus, MoveDateBySeconds, m_nowFraction);

		timeChange = m_nowTime.MoveBySeconds(timeSeconds, m_nowFraction);
	}

	SignalTimeChange(timeChange);
}

Time TimeComponent::GetCurrentTime() const
{
	return m_nowTime;
}

float TimeComponent::GetTimeScale() const
{
	return m_timeScale;
}

float TimeComponent::GetTimeSpeed() const
{
	return m_timeSpeed;
}

void TimeComponent::SetCurrentTime(const Time& i_time)
{
	if(!i_time.IsValid())
	{
		AZ_Assert(false, "Time cannot contains wildcard fields");
		return;
	}

	m_nowTime = i_time;
	EBUS_EVENT(TimeNotificationBus, OnTimeReset, m_nowTime);
}

void TimeComponent::SetTimeScale(float i_scale)
{
	if(i_scale < 0.f)
	{
		AZ_Assert(false, "Time scale cannot be negative. Use TimeSpeed to reverse the counter");
		return;
	}
	
	m_timeScale = i_scale;
	EBUS_EVENT(TimeNotificationBus, OnTimeScaleChanged, m_timeScale);
}

void TimeComponent::SetTimeSpeed(float i_speed)
{
	const bool wasTicking = m_isTicking;

	m_timeSpeed = i_speed;
	EBUS_EVENT(TimeNotificationBus, OnTimeSpeedChanged, m_timeSpeed);
	
	CalculateIsTicking();
	if(wasTicking == m_isTicking)
	{
		return;
	}

	if(m_isTicking)
	{
		AZ::TickBus::Handler::BusConnect();
	}
	else
	{
		AZ::TickBus::Handler::BusDisconnect();
	}
}

TimeS TimeComponent::ConvertToRealTime(const TimeS& i_sceneSeconds) const
{
	if(AZ::IsClose(m_timeSpeed, 0.f, AZ::Constants::FloatEpsilon) || AZ::IsClose(m_timeScale, 0.f, AZ::Constants::FloatEpsilon))
	{
		return AZStd::numeric_limits<TimeS>::infinity();
	}

	return (i_sceneSeconds / (m_timeSpeed * m_timeScale));
}

TimeS TimeComponent::ConvertToSceneTime(const TimeS& i_realSeconds) const
{
	return (i_realSeconds * m_timeSpeed * m_timeScale);
}

bool TimeComponent::CheckDateComponentExists()
{
	const bool oldHasDate = m_hasDate;
	m_hasDate = DateInternalRequestBus::HasHandlers();
	if(oldHasDate && !m_hasDate)
	{
		AZ_Assert(false, "A running DateManager instance is required to count days, but none was found. Counter will be limited to time only");
	}

	return m_hasDate;
}

void TimeComponent::CalculateIsTicking()
{
	m_isTicking = !(AZ::IsClose(m_timeSpeed, 0.0f, AZ::Constants::FloatEpsilon));
}

void TimeComponent::SignalTimeChange(TimeChange i_change) const
{
	switch(i_change)
	{
		case TimeChange::DAY:
		case TimeChange::HOUR:
			EBUS_EVENT(TimeNotificationBus, OnTimeHourChanged, m_nowTime);
			// Continue to the next case

		case TimeChange::MINUTE:
			EBUS_EVENT(TimeNotificationBus, OnTimeMinuteChanged, m_nowTime);
			// Continue to the next case

		case TimeChange::SECOND:
		{
			EBUS_EVENT(TimeNotificationBus, OnTimeSecondChanged, m_nowTime);
		}
		break;

		case TimeChange::NONE:
			break;

		default:
			AZ_Assert(false, "Unsupported TimeChange value: %d", i_change);
			break;
	}
}
