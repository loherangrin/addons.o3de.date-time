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

#include <DateTime/Types/Date.hpp>


namespace Loherangrin::Gems::DateTime
{
	class DateRequests
	{
	public:
		AZ_RTTI(DateRequests, "{31088C06-83CA-4C3D-9437-086EA56629F3}");
		virtual ~DateRequests() = default;

		virtual Date GetCurrentDate() const = 0;
		virtual void SetCurrentDate(const Date& i_date) = 0;
	};
	
	class DateRequestBusTraits
		: public AZ::EBusTraits
	{
	public:
		// EBusTraits
		static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
		static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
	};

	using DateRequestBus = AZ::EBus<DateRequests, DateRequestBusTraits>;

	// ---

    class DateNotifications
    {
    public:
        AZ_RTTI(DateNotifications, "{E3BDB981-A874-48D5-9732-1A041D6F5763}");
        virtual ~DateNotifications() = default;

		virtual void OnDateYearChanged(const Date& i_nowDate){}
		virtual void OnDateMonthChanged(const Date& i_nowDate){}
		virtual void OnDateDayChanged(const Date& i_nowDate){}

		virtual void OnDateReset(const Date& i_startDate){}
    };
    
    class DateNotificationBusTraits
        : public AZ::EBusTraits
    {
    public:
		// EBusTraits
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
    };

    using DateNotificationBus = AZ::EBus<DateNotifications, DateNotificationBusTraits>;

} // Loherangrin::Gems::DateTime
