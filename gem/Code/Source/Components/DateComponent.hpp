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

#include <DateTime/EBuses/DateBus.hpp>
#include <DateTime/Types/Date.hpp>

#include "../EBuses/DateInternalBus.hpp"
#include "DateComponentConfig.hpp"


namespace Loherangrin::Gems::DateTime
{
	class DateComponent
		: public AZ::Component
		, protected DateRequestBus::Handler
		, protected DateInternalRequestBus::Handler
	{
	public:
		using ConfigClass = DateComponentConfig;

	public:
		AZ_COMPONENT(DateComponent, "{909AC34A-F809-4EF7-8B5B-A4B4AB3DE252}");
		static void Reflect(AZ::ReflectContext* io_context);

		static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& io_provided);
		static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& io_incompatible);
		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& io_required);
		static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& io_dependent);

		DateComponent();
		DateComponent(const ConfigClass& i_config);
		DateComponent(ConfigClass&& io_config);

	protected:
		// AZ::Component
		void Init() override;
		void Activate() override;
		void Deactivate() override;

		// DateInternalRequestBus
		void AddDayToDate() override;
		void SubtractDayToDate() override;

		float MoveDateBySeconds(float i_seconds) override;

		// DateRequestBus
		Date GetCurrentDate() const override;
		void SetCurrentDate(const Date& i_date) override;

	private:
		void SignalDateChange(DateChange i_change) const;

		Date m_nowDate;
	};

} // Loherangrin::Gems::DateTime
