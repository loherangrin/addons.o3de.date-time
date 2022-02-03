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

#include <AzCore/Component/EntityId.h>
#include <AzCore/std/containers/variant.h>
#include <AzCore/std/smart_ptr/unique_ptr.h>

#include <DateTime/Types/Alarm.hpp>
#include <DateTime/Types/Date.hpp>
#include <DateTime/Types/RecurrenceRules.hpp>
#include <DateTime/Types/Time.hpp>

#include "AlarmHandle.hpp"

namespace AZStd
{
	template <typename T>
	class shared_ptr;

	template <typename T, typename... Args >
	shared_ptr<T> make_shared(Args&&... io_args);
}


namespace Loherangrin::Gems::DateTime
{
	class Alarm
	{
	public:
		AZ_DISABLE_COPY_MOVE(Alarm);

		static AlarmHandle CreateEmpty(AlarmId m_alarmId);

		template <typename... t_Arguments>
		static AlarmHandle CreateInstance(t_Arguments&&... io_arguments);

		const Date& GetDate() const;
		const Time& GetTime() const;

		bool IsCanceled() const;
		bool IsGoneOff(const Date& i_nowDate, const Time& i_nowTime, bool i_reverse = false) const;
		bool IsSilent() const;

		void Cancel();
		void Silence(bool i_silent);

		bool NextRepeat(bool i_reverse = false);
		bool GoToRepeatAt(const Date& i_date, const Time& i_time, bool i_reverse = false);
		void Reset();

		AlarmAction GetActionType() const;

		template <typename t_Action>
		const t_Action* GetActionIfType() const;

		AlarmComparison CompareTo(const Alarm& i_other) const;

		// Internal use for AlarmComponent. Do not use outside of it!
		void ClearExtension();

	private:
		Alarm(const DateRecurrenceRule& i_dateRule, const TimeRecurrenceRule& i_timeRule, const AZ::EntityId& i_requesterId);
		Alarm(AZStd::unique_ptr<DateRecurrenceRule>&& io_dateRule, AZStd::unique_ptr<TimeRecurrenceRule>&& io_timeRule, const AZ::EntityId& i_requesterId);

		Alarm(const DateRecurrenceRule& i_dateRule, const TimeRecurrenceRule& i_timeRule, const AlarmCallback& i_callback);
		Alarm(AZStd::unique_ptr<DateRecurrenceRule>&& io_dateRule, AZStd::unique_ptr<TimeRecurrenceRule>&& io_timeRule, const AlarmCallback& i_callback);

		AZStd::unique_ptr<DateRecurrenceRule> m_dateRule;
		AZStd::unique_ptr<TimeRecurrenceRule> m_timeRule;

		DateRecurrenceRule::const_iterator_pointer m_dateIterator;
		TimeRecurrenceRule::const_iterator_pointer m_timeIterator;

		Date m_extendedDate;

		bool m_isCanceled;
		bool m_isSilent;

		AZStd::variant<AlarmCallback, AZ::EntityId> m_action;

		static inline const Date INVALID_DATE {};

		template <typename T, typename... Args >
		friend AZStd::shared_ptr<T> AZStd::make_shared(Args&&... io_args);
	};
	
} // Loherangrin::Gems::DateTime

#include "Alarm.inl"
