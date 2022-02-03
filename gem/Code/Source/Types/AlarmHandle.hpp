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

#include <AzCore/std/smart_ptr/shared_ptr.h>

#include <DateTime/Types/Alarm.hpp>

namespace Loherangrin::Gems::DateTime
{
	class Alarm;
}


namespace Loherangrin::Gems::DateTime
{
	class AlarmHandle
	{
	public:
		static AlarmHandle ReWrapAlarm(AlarmHandle&& io_alarmHandle);

		AlarmHandle();

		AlarmId GetAlarmId() const;
		const Alarm* GetAlarm() const;
		Alarm* GetAlarm();

		AlarmComparison CompareTo(const AlarmHandle& i_other) const;

	private:
		using AlarmPointer = AZStd::shared_ptr<Alarm>;

		AlarmHandle(AlarmId i_alarmId);
		AlarmHandle(AlarmPointer&& io_alarm);

		AlarmId m_alarmId;
		AlarmPointer m_alarm;

		static inline AlarmId s_lastAlarmId = 0;

		friend Alarm;
	};

} // Loherangrin::Gems::DateTime
