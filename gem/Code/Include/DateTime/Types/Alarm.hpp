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

#include <AzCore/base.h>
#include <AzCore/std/function/function_template.h>

namespace Loherangrin::Gems::DateTime
{
	class Date;
	class Time;
}


namespace Loherangrin::Gems::DateTime
{
	enum class AlarmAction
	{
		NONE = 0,
		EXECUTE_CALLBACK,
		NOTIFY_REQUESTER
	};

	enum class AlarmComparison : AZ::s8
	{
		LESS_THAN = -1,
		EQUAL,
		GREATER_THAN,
		INVALID
	};

	using AlarmId = AZ::u32;
	using AlarmCallback = AZStd::function<void(const Date&, const Time&)>;

} // Loherangrin::Gems::DateTime
