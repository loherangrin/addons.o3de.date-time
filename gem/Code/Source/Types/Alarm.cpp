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

#include "Alarm.hpp"

using Loherangrin::Gems::DateTime::Alarm;
using Loherangrin::Gems::DateTime::AlarmAction;
using Loherangrin::Gems::DateTime::AlarmComparison;
using Loherangrin::Gems::DateTime::AlarmHandle;
using Loherangrin::Gems::DateTime::Date;
using Loherangrin::Gems::DateTime::Time;


Alarm::Alarm(const DateRecurrenceRule& i_dateRule, const TimeRecurrenceRule& i_timeRule, const AZ::EntityId& i_requesterId)
	: m_dateRule { (i_dateRule.GetStart().IsValid()) ? i_dateRule.Clone() : nullptr }
	, m_timeRule { i_timeRule.Clone() }
	, m_dateIterator { nullptr }
	, m_timeIterator { nullptr }
	, m_isCanceled { false }
	, m_isSilent { false }
	, m_action { i_requesterId }
{
	Reset();
}

Alarm::Alarm(AZStd::unique_ptr<DateRecurrenceRule>&& io_dateRule, AZStd::unique_ptr<TimeRecurrenceRule>&& io_timeRule, const AZ::EntityId& i_requesterId)
	: m_dateRule { (io_dateRule->GetStart().IsValid()) ? AZStd::move(io_dateRule) : nullptr } 
	, m_timeRule { AZStd::move(io_timeRule) }
	, m_dateIterator { nullptr }
	, m_timeIterator { nullptr }
	, m_isCanceled { false }
	, m_isSilent { false }
	, m_action { i_requesterId }
{
	Reset();
}

Alarm::Alarm(const DateRecurrenceRule& i_dateRule, const TimeRecurrenceRule& i_timeRule, const AlarmCallback& i_callback)
	: m_dateRule { (i_dateRule.GetStart().IsValid()) ? i_dateRule.Clone() : nullptr }
	, m_timeRule { i_timeRule.Clone() }
	, m_dateIterator { nullptr }
	, m_timeIterator { nullptr }
	, m_isCanceled { false }
	, m_isSilent { false }
	, m_action { i_callback }
{
	Reset();
}

Alarm::Alarm(AZStd::unique_ptr<DateRecurrenceRule>&& io_dateRule, AZStd::unique_ptr<TimeRecurrenceRule>&& io_timeRule, const AlarmCallback& i_callback)
	: m_dateRule { (io_dateRule->GetStart().IsValid()) ? AZStd::move(io_dateRule) : nullptr }
	, m_timeRule { AZStd::move(io_timeRule) }
	, m_dateIterator { nullptr }
	, m_timeIterator { nullptr }
	, m_isCanceled { false }
	, m_isSilent { false }
	, m_action { i_callback }
{
	Reset();
}

AlarmHandle Alarm::CreateEmpty(AlarmId i_alarmId)
{
	const AlarmHandle newEmptyHandle { i_alarmId };

	return newEmptyHandle;
}

const Date& Alarm::GetDate() const
{
	if(m_extendedDate.IsValid())
	{
		return m_extendedDate;
	}
	else if(m_dateIterator)
	{
		return **m_dateIterator;
	}
	else
	{
		return INVALID_DATE;
	}
}

const Time& Alarm::GetTime() const
{
	return (**m_timeIterator);
}

bool Alarm::IsCanceled() const
{
	return m_isCanceled;
}

bool Alarm::IsGoneOff(const Date& i_nowDate, const Time& i_nowTime, bool i_reverse) const
{
	const Date& alarmDate = GetDate();
	const DateComparison dateComparison = i_nowDate.CompareTo(alarmDate);

	bool isAlarmGreaterThanNow;
	if(dateComparison == DateComparison::EQUAL)
	{
		const Time& alarmTime = GetTime();
		const TimeComparison timeComparison = i_nowTime.CompareTo(alarmTime);
		if(timeComparison == TimeComparison::EQUAL)
		{
			return true;
		}

		isAlarmGreaterThanNow = (timeComparison == TimeComparison::GREATER_THAN);
	}
	else
	{
		isAlarmGreaterThanNow = (dateComparison == DateComparison::GREATER_THAN);
	}

	return ((i_reverse) ? !isAlarmGreaterThanNow : isAlarmGreaterThanNow);
}

bool Alarm::IsSilent() const
{
	return m_isSilent;
}

void Alarm::Cancel()
{
	m_isCanceled = true;
}

void Alarm::Silence(bool i_silent)
{
	m_isSilent = i_silent;
}

bool Alarm::NextRepeat(bool i_reverse)
{
	AZ_Assert(m_timeIterator != nullptr, "TimeIterator cannot be null");

	if(!(*m_timeIterator)->IsValid())
	{
		return false;
	}

	if(i_reverse)
	{
		--(*m_timeIterator);
	}
	else
	{
		++(*m_timeIterator);
	}

	const bool isTimeValid = (*m_timeIterator)->IsValid();
	if(isTimeValid && (m_timeIterator->GetLastChange() == TimeChange::DAY))
	{
		if(m_dateIterator)
		{
			if(!m_extendedDate.IsValid())
			{
				m_extendedDate = **m_dateIterator;
			}

			m_extendedDate.AddDay();
		}
		else
		{
			m_extendedDate = Date { 1, 1, 1};
		}
	}
	else if(!isTimeValid)
	{
		if(!m_dateIterator)
		{
			return false;
		}

		if(i_reverse)
		{
			--(*m_dateIterator);
		}
		else
		{
			++(*m_dateIterator);
		}

		if(!(*m_dateIterator)->IsValid())
		{
			return false;
		}

		m_timeIterator = m_timeRule->Begin();

		if(m_extendedDate.IsValid())
		{
			m_extendedDate = INVALID_DATE;
		}
	}

	return true;
}

bool Alarm::GoToRepeatAt(const Date& i_date, const Time& i_time, bool i_reverse)
{
	AZ_Assert(m_timeIterator != nullptr, "TimeIterator cannot be null");

	m_dateIterator = (m_dateRule) ? m_dateRule->From(i_date, !i_reverse) : nullptr;
	m_timeIterator = m_timeRule->From(i_time, !i_reverse);

	return (*m_timeIterator)->IsValid();
}

void Alarm::Reset()
{
	m_dateIterator = (m_dateRule) ? m_dateRule->Begin() : nullptr;
	m_timeIterator = m_timeRule->Begin();
}

AlarmAction Alarm::GetActionType() const
{
	if(AZStd::holds_alternative<AlarmCallback>(m_action))
	{
		return AlarmAction::EXECUTE_CALLBACK;
	}
	else if(AZStd::holds_alternative<AZ::EntityId>(m_action))
	{
		return AlarmAction::NOTIFY_REQUESTER;
	}
	else
	{
		AZ_Assert(false, "Unable to determine the action type");
		return AlarmAction::NONE;
	}
}

AlarmComparison Alarm::CompareTo(const Alarm& i_other) const
{
	AZ_Assert(m_timeIterator != nullptr, "TimeIterator cannot be null");

	const DateComparison dateComparison = GetDate().CompareTo(i_other.GetDate());
	switch(dateComparison)
	{
		case DateComparison::GREATER_THAN:
			return AlarmComparison::GREATER_THAN;

		case DateComparison::LESS_THAN:
			return AlarmComparison::LESS_THAN;

		case DateComparison::EQUAL:
		{
			const TimeComparison timeComparison = GetTime().CompareTo(i_other.GetTime());
			switch(timeComparison)
			{
				case TimeComparison::GREATER_THAN:
					return AlarmComparison::GREATER_THAN;

				case TimeComparison::LESS_THAN:
					return AlarmComparison::LESS_THAN;

				case TimeComparison::EQUAL:
				{
					const AlarmAction thisActionType = GetActionType();
					const AlarmAction otherActionType = GetActionType();

					if(thisActionType == otherActionType)
					{
						switch(thisActionType)
						{
							case AlarmAction::NONE:
							{
								return AlarmComparison::EQUAL;
							}
							break;

							case AlarmAction::EXECUTE_CALLBACK:
							{
								const auto thisCallback = GetActionIfType<AlarmCallback>();
								const auto otherCallback = i_other.GetActionIfType<AlarmCallback>();

								const auto thisCallbackAddress = &(*thisCallback);
								const auto otherCallbackAddress = &(*otherCallback);

								if(thisCallbackAddress == otherCallbackAddress)
								{
									return AlarmComparison::EQUAL;
								}
								else if(thisCallbackAddress > otherCallbackAddress)
								{
									return AlarmComparison::GREATER_THAN;
								}
								else
								{
									return AlarmComparison::LESS_THAN;
								}
							}
							break;

							case AlarmAction::NOTIFY_REQUESTER:
							{
								const AZ::EntityId* const thisRequesterId = GetActionIfType<AZ::EntityId>();
								const AZ::EntityId* const otherRequesterId = i_other.GetActionIfType<AZ::EntityId>();
								
								if(thisRequesterId == otherRequesterId)
								{
									return AlarmComparison::EQUAL;
								}
								else if(thisRequesterId > otherRequesterId)
								{
									return AlarmComparison::GREATER_THAN;
								}
								else
								{
									return AlarmComparison::LESS_THAN;
								}
							}
							break;

							default:
							{
								AZ_Assert(false, "Invalid action type: %d");
							}
						}
					}
					else if(thisActionType > otherActionType)
					{
						return AlarmComparison::GREATER_THAN;
					}
					else
					{
						return AlarmComparison::LESS_THAN;
					}
				}

				default:
				{
					AZ_Assert(false, "Invalid time comparison result: %d", timeComparison);
				}
			}
		}

		default:
		{
			AZ_Assert(false, "Invalid date comparison result: %d", dateComparison);
		}
	}

	return AlarmComparison::INVALID;
}

void Alarm::ClearExtension()
{
	if(m_dateIterator)
	{
		return;
	}

	m_extendedDate = INVALID_DATE;
}
