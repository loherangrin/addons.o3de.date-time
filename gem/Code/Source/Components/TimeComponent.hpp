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
#include <AzCore/Component/TickBus.h>

#include <DateTime/EBuses/TimeBus.hpp>

#include "../Components/TimeComponentConfig.hpp"


namespace Loherangrin::Gems::DateTime
{
	class TimeComponent
		: public AZ::Component
		, protected AZ::TickBus::Handler
		, protected TimeRequestBus::Handler
	{
	public:
		using ConfigClass = TimeComponentConfig;

	public:
		AZ_COMPONENT(TimeComponent, "{A7B5F578-5C09-4DA1-AAA4-7D0F9A8F7225}");
		static void Reflect(AZ::ReflectContext* io_context);

		static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& io_provided);
		static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& io_incompatible);
		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& io_required);
		static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& io_dependent);

		TimeComponent();
		TimeComponent(const ConfigClass& i_config);
		TimeComponent(ConfigClass&& io_config);

	protected:
		// AZ::Component
		void Init() override;
		void Activate() override;
		void Deactivate() override;

		// AZ::TickBus
		void OnTick(float i_deltaTime, AZ::ScriptTimePoint i_time) override;
		
		// TimeRequestBus
		Time GetCurrentTime() const override;
		float GetTimeScale() const override;
		float GetTimeSpeed() const override;

		void SetCurrentTime(const Time& i_time) override;
		void SetTimeScale(float i_scale) override;
		void SetTimeSpeed(float i_speed) override;

		TimeS ConvertToRealTime(const TimeS& i_sceneSeconds) const override;
		TimeS ConvertToSceneTime(const TimeS& i_realSeconds) const override;

	private:
		bool CheckDateComponentExists();

		void CalculateIsTicking();
		void SignalTimeChange(TimeChange i_change) const;

		Time m_nowTime;
		TimeS m_nowFraction;

		float m_timeScale;
		float m_timeSpeed;

		bool m_isTicking;
		bool m_hasDate;
	};

} // Loherangrin::Gems::DateTime
