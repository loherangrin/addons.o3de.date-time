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

#include <DateTime/Types/Time.hpp>


namespace Loherangrin::Gems::DateTime
{
	class TimeRequests
	{
	public:
		AZ_RTTI(TimeRequests, "{33B44262-7461-4F5E-94A7-79D0F466217B}");
		virtual ~TimeRequests() = default;

		virtual Time GetCurrentTime() const = 0;
		virtual float GetTimeScale() const = 0;
		virtual float GetTimeSpeed() const = 0;

		virtual void SetCurrentTime(const Time& i_time) = 0;
		virtual void SetTimeScale(float i_scale) = 0;
		virtual void SetTimeSpeed(float i_speed) = 0;

		virtual TimeS ConvertToRealTime(const TimeS& i_sceneSeconds) const = 0;
		virtual TimeS ConvertToSceneTime(const TimeS& i_realSeconds) const = 0;
	};
	
	class TimeRequestBusTraits
		: public AZ::EBusTraits
	{
	public:
		// EBusTraits
		static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
		static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
	};

	using TimeRequestBus = AZ::EBus<TimeRequests, TimeRequestBusTraits>;

	// ---

    class TimeNotifications
    {
    public:
        AZ_RTTI(TimeNotifications, "{7548D8A9-FEBD-4F89-B65D-E9A0CBB4025D}");
        virtual ~TimeNotifications() = default;

		virtual void OnTimeHourChanged(const Time& i_nowTime){}
		virtual void OnTimeMinuteChanged(const Time& i_nowTime){}
		virtual void OnTimeSecondChanged(const Time& i_nowTime){}

		virtual void OnTimeScaleChanged(float i_scale){}
		virtual void OnTimeSpeedChanged(float i_speed){}

		virtual void OnTimeReset(const Time& i_startTime){}
    };
    
    class TimeNotificationBusTraits
        : public AZ::EBusTraits
    {
    public:
		// EBusTraits
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
    };

    using TimeNotificationBus = AZ::EBus<TimeNotifications, TimeNotificationBusTraits>;

} // Loherangrin::Gems::DateTime
