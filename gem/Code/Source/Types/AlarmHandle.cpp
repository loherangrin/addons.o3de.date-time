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

#include "AlarmHandle.hpp"

using Loherangrin::Gems::DateTime::Alarm;
using Loherangrin::Gems::DateTime::AlarmHandle;
using Loherangrin::Gems::DateTime::AlarmId;


AlarmHandle::AlarmHandle()
	: AlarmHandle(0)
{}

AlarmHandle::AlarmHandle(AlarmId i_alarmId)
	: m_alarmId { i_alarmId }
	, m_alarm { nullptr }
{}

AlarmHandle::AlarmHandle(AlarmPointer&& io_alarm)
	: m_alarmId { 0 }
	, m_alarm { AZStd::move(io_alarm) }
{}

AlarmHandle AlarmHandle::ReWrapAlarm(AlarmHandle&& io_alarmHandle)
{
	AlarmHandle newAlarmHandle = AZStd::move(io_alarmHandle);

	while(newAlarmHandle.m_alarmId == 0)
	{
		newAlarmHandle.m_alarmId = ++s_lastAlarmId;
	}

	return newAlarmHandle;
}

AlarmId AlarmHandle::GetAlarmId() const
{
	return m_alarmId;
}

const Alarm* AlarmHandle::GetAlarm() const
{
	return ((m_alarm) ? m_alarm.get() : nullptr);
}

Alarm* AlarmHandle::GetAlarm()
{
	return ((m_alarm) ? m_alarm.get() : nullptr);
}

AlarmComparison AlarmHandle::CompareTo(const AlarmHandle& i_other) const
{
	if((m_alarmId == i_other.m_alarmId) && (m_alarmId != 0))
	{
		return AlarmComparison::EQUAL;
	}

	return m_alarm->CompareTo(*(i_other.m_alarm));	
}
