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

#include "DateComponent.hpp"

using Loherangrin::Gems::DateTime::Date;
using Loherangrin::Gems::DateTime::DateComponent;


namespace Loherangrin::Gems::DateTime
{
	class DateNotificationBusBehaviorHandler
		: public DateNotificationBus::Handler
		, public AZ::BehaviorEBusHandler
	{
	public:
		AZ_EBUS_BEHAVIOR_BINDER(DateNotificationBusBehaviorHandler, "{33B97A8C-03E0-4742-A931-5A56C6DC4EEF}", AZ::SystemAllocator,
			OnDateYearChanged,
			OnDateMonthChanged,
			OnDateDayChanged,
			OnDateReset
		);

		void OnDateYearChanged(const Date& i_nowDate) override
		{
			Call(FN_OnDateYearChanged, i_nowDate);
		}

		void OnDateMonthChanged(const Date& i_nowDate) override
		{
			Call(FN_OnDateMonthChanged, i_nowDate);
		}

		void OnDateDayChanged(const Date& i_nowDate) override
		{
			Call(FN_OnDateDayChanged, i_nowDate);
		}

		void OnDateReset(const Date& i_startDate) override
		{
			Call(FN_OnDateReset, i_startDate);
		}
	};

} // Loherangrin::Gems::DateTime

void DateComponent::Reflect(AZ::ReflectContext* io_context)
{
	ConfigClass::Reflect(io_context);

	if(auto serializeContext = azrtti_cast<AZ::SerializeContext*>(io_context))
	{
		serializeContext->Class<DateComponent, AZ::Component>()
			->Version(0)
			->Field("NowDate", &DateComponent::m_nowDate)
		;
	}

	if(auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(io_context))
	{
		behaviorContext->EBus<DateRequestBus>("DateRequestBus")
			->Attribute(AZ::Script::Attributes::Module, ScriptAttributes::MODULE)
			->Attribute(AZ::Script::Attributes::Category, ScriptAttributes::CATEGORY)

			->Event("GetCurrentDate", &DateRequestBus::Events::GetCurrentDate)
			->Event("SetCurrentDate", &DateRequestBus::Events::SetCurrentDate)

			->VirtualProperty("CurrentDate", "GetCurrentDate", "SetCurrentDate")
		;

		behaviorContext->EBus<DateNotificationBus>("DateNotificationBus")
			->Attribute(AZ::Script::Attributes::Module, ScriptAttributes::MODULE)
			->Attribute(AZ::Script::Attributes::Category, ScriptAttributes::CATEGORY)

			->Handler<DateNotificationBusBehaviorHandler>()
		;
	}
}

void DateComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& io_provided)
{
	io_provided.push_back(Services::DATE);
}

void DateComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& io_incompatible)
{
	io_incompatible.push_back(Services::DATE);
}

void DateComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& io_required)
{}

void DateComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& io_dependent)
{}

DateComponent::DateComponent()
	: DateComponent(ConfigClass {})
{}

DateComponent::DateComponent(const ConfigClass& i_config)
	: m_nowDate { i_config.m_nowDate }
{}

DateComponent::DateComponent(ConfigClass&& io_config)
	: m_nowDate { AZStd::move(io_config.m_nowDate) }
{}

void DateComponent::Init()
{}

void DateComponent::Activate()
{
	DateInternalRequestBus::Handler::BusConnect();
	DateRequestBus::Handler::BusConnect();
}

void DateComponent::Deactivate()
{
	DateRequestBus::Handler::BusDisconnect();
	DateInternalRequestBus::Handler::BusDisconnect();
}

void DateComponent::AddDayToDate()
{
	const DateChange dateChange = m_nowDate.AddDay();

	SignalDateChange(dateChange);
}

void DateComponent::SubtractDayToDate()
{
	const DateChange dateChange = m_nowDate.SubtractDay();

	SignalDateChange(dateChange);
}

float DateComponent::MoveDateBySeconds(float i_seconds)
{
	float remainder;
	const DateChange dateChange = m_nowDate.MoveBySeconds(i_seconds, remainder);

	SignalDateChange(dateChange);

	return remainder;
}

Date DateComponent::GetCurrentDate() const
{
	return m_nowDate;
}

void DateComponent::SetCurrentDate(const Date& i_date)
{
	if(!i_date.IsValid())
	{
		AZ_Assert(false, "Date cannot contains invalid fields");
		return;
	}

	m_nowDate = i_date;
	EBUS_EVENT(DateNotificationBus, OnDateReset, m_nowDate);
}

void DateComponent::SignalDateChange(DateChange i_change) const
{
	switch(i_change)
	{
		case DateChange::NONE:
			break;

		case DateChange::YEAR:
			EBUS_EVENT(DateNotificationBus, OnDateYearChanged, m_nowDate);
			// Continue to the next case

		case DateChange::MONTH:
			EBUS_EVENT(DateNotificationBus, OnDateMonthChanged, m_nowDate);
			// Continue to the next case

		case DateChange::DAY:
			EBUS_EVENT(DateNotificationBus, OnDateDayChanged, m_nowDate);
			break;

		default:
			AZ_Assert(false, "Unsupported DateChange value: %d", i_change);
			break;
	}
}
