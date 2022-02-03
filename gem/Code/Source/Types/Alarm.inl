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

#include <AzCore/std/smart_ptr/make_shared.h>

#include "Alarm.hpp"


namespace Loherangrin::Gems::DateTime
{
	template <typename... t_Arguments>
	AlarmHandle Alarm::CreateInstance(t_Arguments&&... io_args)
	{
		auto newAlarm = AZStd::make_shared<Alarm>(AZStd::forward<t_Arguments>(io_args)...);
		const AlarmHandle newAlarmHandle { AZStd::move(newAlarm) };

		return newAlarmHandle;
	}

	template <typename t_Action>
	const t_Action* Alarm::GetActionIfType() const
	{
		return AZStd::get_if<t_Action>(&m_action);
	}

} //Loherangrin::Gems::DateTime
