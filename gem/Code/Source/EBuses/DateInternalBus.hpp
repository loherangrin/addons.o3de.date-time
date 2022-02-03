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


namespace Loherangrin::Gems::DateTime
{
	class DateInternalRequests
	{
	public:
		AZ_RTTI(DateInternalRequests, "{3BAFC59B-7332-40BB-A71F-246F9438B2C3}");

		virtual ~DateInternalRequests() = default;

		virtual void AddDayToDate() = 0;
		virtual void SubtractDayToDate() = 0;

		virtual float MoveDateBySeconds(float i_seconds) = 0;
	};
	
	class DateInternalRequestBusTraits
		: public AZ::EBusTraits
	{
	public:
		// EBusTraits
		static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
		static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
	};

	using DateInternalRequestBus = AZ::EBus<DateInternalRequests, DateInternalRequestBusTraits>;

} // Loherangrin::Gems::DateTime
