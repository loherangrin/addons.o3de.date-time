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

#include <AzCore/RTTI/TypeInfoSimple.h>

#include <DateTime/Types/Date.hpp>


namespace Loherangrin::Gems::DateTime
{
	using RecurrenceRuleRepeat = AZ::u16;

	namespace Internal
	{
		template <typename t_Value>
		class RecurrenceRule
		{
		private:
			using ThisClass = RecurrenceRule;

		protected:
			using BaseClass = RecurrenceRule;

		public:
			virtual ~RecurrenceRule();

			const t_Value& GetStart() const;

			class ConstIterator
			{
			public:
				using value_type = const t_Value;
				using pointer = const value_type*;
				using reference = const value_type&;

				virtual ~ConstIterator();

				typename t_Value::Change GetLastChange() const;

				reference operator*() const;
				pointer operator->() const;

				bool operator==(const ConstIterator& i_rhs) const;
				bool operator!=(const ConstIterator& i_rhs) const;

				ConstIterator& operator++();
				ConstIterator& operator--();

			protected:
				ConstIterator(const ThisClass* i_rule, const t_Value* i_fromValue, bool i_ceiling);

				virtual bool Next() = 0;
				virtual bool Previous() = 0;

				t_Value m_currentValue;
				typename t_Value::Change m_lastChange;
			};
		
			using const_iterator = ConstIterator;
			using const_iterator_pointer = AZStd::unique_ptr<const_iterator>;

			const_iterator_pointer Begin() const;
			const_iterator_pointer End() const;
			const_iterator_pointer From(const t_Value& i_value, bool i_ceiling = true) const;

			AZStd::unique_ptr<BaseClass> Clone() const;

		protected:
			RecurrenceRule(const t_Value& i_startValue);

			virtual const_iterator* CreateIterator(const BaseClass* i_rule, const t_Value* i_fromValue, bool i_ceiling) const = 0;
			virtual BaseClass* CreateCopy() const = 0;

			t_Value m_startValue;

		private:
			const_iterator_pointer MakeUniqueIterator(const ThisClass* i_rule, const t_Value* i_fromValue, bool i_ceiling) const;
		};

		// ---

		#define LOHERANGRIN_GEMS_DATETIME_RECURRENCERULES_CREATE_METHODS()														\
			const_iterator* CreateIterator(const BaseClass* i_rule, const t_Value* i_fromValue, bool i_ceiling) const override	\
			{																													\
				return new const_iterator(static_cast<const ThisClass*>(i_rule), i_fromValue, i_ceiling);						\
			}																													\
																																\
			BaseClass* CreateCopy() const override																				\
			{																													\
				return new ThisClass(*this);																					\
			}

		// ---

		template <typename t_Value>
		class NoRepeatRule
			: public RecurrenceRule<t_Value>
		{
		private:
			using ThisClass = NoRepeatRule;
			using ParentClass = RecurrenceRule<t_Value>;

		protected:
			using BaseClass = typename ParentClass::BaseClass;

		public:
			NoRepeatRule(const t_Value& i_when);

			class ConstIterator
				: public ParentClass::ConstIterator
			{
			protected:
				ConstIterator(const ThisClass* i_rule, const t_Value* i_fromValue, bool i_ceiling);

			private:
				bool Next() override;
				bool Previous() override;

				friend ThisClass;
			};

			using const_iterator = ConstIterator;

		private:
			LOHERANGRIN_GEMS_DATETIME_RECURRENCERULES_CREATE_METHODS();
		};

		// ---

		template <typename t_Value, bool c_isInfinite>
		class RepeatNTimesRule;

		template <typename t_Value>
		class RepeatNTimesRule<t_Value, true>
			: public RecurrenceRule<t_Value>
		{
		private:
			using ThisClass = RepeatNTimesRule<t_Value, true>;
			using ParentClass = RecurrenceRule<t_Value>;

		protected:
			using BaseClass = typename ParentClass::BaseClass;

		public:
			RepeatNTimesRule(const t_Value& i_from);

			class ConstIterator
				: public ParentClass::ConstIterator
			{
			protected:
				ConstIterator(const ThisClass* i_rule, const t_Value* i_fromValue, bool i_ceiling);

				virtual void IncreaseFields() = 0;
				virtual void DecreaseFields() = 0;

			private:
				bool Next() override;
				bool Previous() override;
			};
		};

		template <typename t_Value>
		class RepeatNTimesRule<t_Value, false>
			: public RecurrenceRule<t_Value>
		{
		private:
			using ThisClass = RepeatNTimesRule<t_Value, false>;
			using ParentClass = RecurrenceRule<t_Value>;

		protected:
			using BaseClass = typename ParentClass::BaseClass;

		public:
			RepeatNTimesRule(const t_Value& i_from, RecurrenceRuleRepeat i_nRepeats);

			class ConstIterator
				: public ParentClass::ConstIterator
			{
			protected:			
				ConstIterator(const ThisClass* i_rule, const t_Value* i_fromValue, bool i_ceiling);

				virtual void IncreaseFields() = 0;
				virtual void DecreaseFields() = 0;

				RecurrenceRuleRepeat m_nMaxValues;
				RecurrenceRuleRepeat m_nRemainingValues;

			private:
				bool Next() override;
				bool Previous() override;
			};

		protected:
			RecurrenceRuleRepeat m_nRepeats;
		};

		// ---

		template
		<
			typename t_Value, bool c_isInfinite,
			typename t_Value::Change (t_Value::*c_incrementMethod)(),
			typename t_Value::Change (t_Value::*c_decrementMethod)(),
			typename t_Value::Difference (t_Value::*c_countMethod)(const t_Value&) const = nullptr,
			typename t_Field = bool,
			t_Field (t_Value::*c_constrainedFieldGetter)() const = nullptr,
			void (t_Value::*c_constrainedFieldSetter)(t_Field) = nullptr
		>
		class RepeatAndIncrementFieldRule
			: public RepeatNTimesRule<t_Value, c_isInfinite>
		{
		private:
			using ThisClass = RepeatAndIncrementFieldRule;
			using ParentClass = RepeatNTimesRule<t_Value, c_isInfinite>;

		protected:
			using BaseClass = typename ParentClass::BaseClass;

		public:
			using ParentClass::ParentClass;

			template <bool c_condition = c_isInfinite, typename = AZStd::enable_if_t<!c_condition>>
			RepeatAndIncrementFieldRule(const t_Value& i_from, const t_Value& i_to)
				: ParentClass(i_from, 0)
			{
				if constexpr(c_countMethod != nullptr)
				{
					if(!i_to.IsValid())
					{
						AZ_Assert(false, "'To' parameter must be a valid value");
						return;
					}

					const typename t_Value::Difference nRepeats = (i_from.*c_countMethod)(i_to);
					if(nRepeats < 0)
					{
						AZ_Assert(false, "'To' parameter cannot be before 'From'");
						return;
					}

					this->m_nRepeats = static_cast<RecurrenceRuleRepeat>(nRepeats);
				}
			}

			class ConstIterator
				: public ParentClass::ConstIterator
			{
			protected:
				ConstIterator(const ThisClass* i_rule, const t_Value* i_fromValue, bool i_ceiling);

			private:
				void IncreaseFields() override;
				void DecreaseFields() override;

				t_Field m_startConstrainedField;

				friend ThisClass;
			};

			using const_iterator = ConstIterator;

		private:
			LOHERANGRIN_GEMS_DATETIME_RECURRENCERULES_CREATE_METHODS();
		};

		// ---

		template
		<
			typename t_Value, bool c_isInfinite,
			typename t_Step,
			typename t_Value::Change (t_Value::*c_sumMethod)(t_Step),
			typename t_Value::Change (t_Value::*c_differenceMethod)(t_Step),
			typename t_Value::Difference (t_Value::*c_countMethod)(const t_Value&) const = nullptr,
			typename t_Field = bool,
			t_Field (t_Value::*c_constrainedFieldGetter)() const = nullptr,
			void (t_Value::*c_constrainedFieldSetter)(t_Field) = nullptr
		>
		class RepeatAndSumFieldRule
			: public RepeatNTimesRule<t_Value, c_isInfinite>
		{
		private:
			using ThisClass = RepeatAndSumFieldRule;
			using ParentClass = RepeatNTimesRule<t_Value, c_isInfinite>;

		protected:
			using BaseClass = typename ParentClass::BaseClass;

		public:
			template <bool c_condition = c_isInfinite, typename = AZStd::enable_if_t<c_condition>>
			RepeatAndSumFieldRule(t_Step i_step, const t_Value& i_from)
				: ParentClass(i_from)
				, m_valueStep { i_step }
			{}

			template <bool c_condition = c_isInfinite, typename = AZStd::enable_if_t<!c_condition>>
			RepeatAndSumFieldRule(t_Step i_step, const t_Value& i_from, const t_Value& i_to)
				: ParentClass(i_from, 0)
				, m_valueStep { i_step }
			{
				if constexpr(c_countMethod != nullptr)
				{
					if(!i_to.IsValid())
					{
						AZ_Assert(false, "'To' parameter must be a valid value");
						return;
					}

					const typename t_Value::Difference maxRepeats = (i_from.*c_countMethod)(i_to);
					if(maxRepeats < 0)
					{
						AZ_Assert(false, "'To' parameter cannot be before 'From'");
						return;
					}

					const auto nRepeats = AZStd::floor(maxRepeats / static_cast<typename t_Value::Difference>(i_step));
					this->m_nRepeats = static_cast<RecurrenceRuleRepeat>(nRepeats);
				}
			}

			template <bool c_condition = c_isInfinite, typename = AZStd::enable_if_t<!c_condition>>
			RepeatAndSumFieldRule(t_Step i_step, const t_Value& i_from, RecurrenceRuleRepeat i_nRepeats)
				: ParentClass(i_from, i_nRepeats)
				, m_valueStep { i_step }
			{}

			class ConstIterator
				: public ParentClass::ConstIterator
			{
			protected:
				ConstIterator(const ThisClass* i_rule, const t_Value* i_fromValue, bool i_ceiling);

			private:
				void IncreaseFields() override;
				void DecreaseFields() override;

				t_Field m_startConstrainedField;
				t_Step m_valueStep;

				friend ThisClass;
			};

			using const_iterator = ConstIterator;

		private:
			LOHERANGRIN_GEMS_DATETIME_RECURRENCERULES_CREATE_METHODS();

			t_Step m_valueStep;
		};

		// ---

		#undef LOHERANGRIN_GEMS_DATETIME_RECURRENCERULES_CREATE_METHODS

		// ---

		template <typename t_Value>
		class RecurrenceRuleBehaviorHandle
		{
		private:
			using ThisClass = RecurrenceRuleBehaviorHandle<t_Value>;

		public:
			static void Reflect(AZ::ReflectContext* io_context, const AZStd::string& i_prefix);
			// See external AZ_TYPE_INFO definition at end of the file

			RecurrenceRuleBehaviorHandle();
			RecurrenceRuleBehaviorHandle(AZStd::unique_ptr<RecurrenceRule<t_Value>>&& io_rule);

			RecurrenceRuleBehaviorHandle(const RecurrenceRuleBehaviorHandle& i_other);
			RecurrenceRuleBehaviorHandle(RecurrenceRuleBehaviorHandle&& io_other);

			RecurrenceRuleBehaviorHandle& operator=(const RecurrenceRuleBehaviorHandle& i_other);
			RecurrenceRuleBehaviorHandle& operator=(RecurrenceRuleBehaviorHandle&& io_other);

			const RecurrenceRule<t_Value>* GetRule() const;
			RecurrenceRule<t_Value>* GetRule();

			bool IsValid() const;

		private:
			AZStd::unique_ptr<RecurrenceRule<t_Value>> m_rule;
		};
	}

	using DateRecurrenceRule = Internal::RecurrenceRule<Date>;
	using DateRecurrenceRuleBehaviorHandle = Internal::RecurrenceRuleBehaviorHandle<Date>;

	namespace DateRecurrenceRules
	{
		using EveryNDaysFrom = Internal::RepeatAndSumFieldRule<Date, true, Date::Day, &Date::AddDays, &Date::SubtractDays, &Date::CountDaysTo>;
		using EveryNMonthsFrom = Internal::RepeatAndSumFieldRule<Date, true, Date::Month, &Date::AddMonths, &Date::SubtractMonths, &Date::CountMonthsTo, Date::Day, &Date::GetDayNumber, &Date::SetDay>;
		using EveryNYearsFrom = Internal::RepeatAndSumFieldRule<Date, true, Date::Year, &Date::AddYears, &Date::SubtractYears, &Date::CountYearsTo, Date::Day, &Date::GetDayNumber, &Date::SetDay>;

		using EveryNDaysInRange = Internal::RepeatAndSumFieldRule<Date, false, Date::Day, &Date::AddDays, &Date::SubtractDays, &Date::CountDaysTo>;
		using EveryNMonthsInRange = Internal::RepeatAndSumFieldRule<Date, false, Date::Month, &Date::AddMonths, &Date::SubtractMonths, &Date::CountMonthsTo, Date::Day, &Date::GetDayNumber, &Date::SetDay>;
		using EveryNYearsInRange = Internal::RepeatAndSumFieldRule<Date, false, Date::Year, &Date::AddYears, &Date::SubtractYears, &Date::CountYearsTo, Date::Day, &Date::GetDayNumber, &Date::SetDay>;

		using EveryDayFrom = Internal::RepeatAndIncrementFieldRule<Date, true, &Date::AddDay, &Date::SubtractDay>;
		using EveryMonthFrom = Internal::RepeatAndIncrementFieldRule<Date, true, &Date::AddMonth, &Date::SubtractMonth, nullptr, Date::Day, &Date::GetDayNumber, &Date::SetDay>;
		using EveryYearFrom = Internal::RepeatAndIncrementFieldRule<Date, true, &Date::AddYear, &Date::SubtractYear, nullptr, Date::Day, &Date::GetDayNumber, &Date::SetDay>;

		using EveryDayInRange = Internal::RepeatAndIncrementFieldRule<Date, false, &Date::AddDay, &Date::SubtractDay, &Date::CountDaysTo>;
		using EveryMonthInRange = Internal::RepeatAndIncrementFieldRule<Date, false, &Date::AddMonth, &Date::SubtractMonth, &Date::CountMonthsTo, Date::Day, &Date::GetDayNumber, &Date::SetDay>;
		using EveryYearInRange = Internal::RepeatAndIncrementFieldRule<Date, false, &Date::AddYear, &Date::SubtractYear, &Date::CountYearsTo, Date::Day, &Date::GetDayNumber, &Date::SetDay>;

		using OnlyOnce = Internal::NoRepeatRule<Date>;
	}

	using TimeRecurrenceRule = Internal::RecurrenceRule<Time>;
	using TimeRecurrenceRuleBehaviorHandle = Internal::RecurrenceRuleBehaviorHandle<Time>;

	namespace TimeRecurrenceRules
	{
		using EveryNHoursFrom = Internal::RepeatAndSumFieldRule<Time, true, Time::Hour, &Time::AddHours, &Time::SubtractHours, &Time::CountHoursTo>;
		using EveryNMinutesFrom = Internal::RepeatAndSumFieldRule<Time, true, Time::Minute, &Time::AddMinutes, &Time::SubtractMinutes, &Time::CountMinutesTo>;
		using EveryNSecondsFrom = Internal::RepeatAndSumFieldRule<Time, true, Time::Second, &Time::AddSeconds, &Time::SubtractSeconds, &Time::CountSecondsTo>;

		using EveryNHoursInRange = Internal::RepeatAndSumFieldRule<Time, false, Time::Hour, &Time::AddHours, &Time::SubtractHours, &Time::CountHoursTo>;
		using EveryNMinutesInRange = Internal::RepeatAndSumFieldRule<Time, false, Time::Minute, &Time::AddMinutes, &Time::SubtractMinutes, &Time::CountMinutesTo>;
		using EveryNSecondsInRange = Internal::RepeatAndSumFieldRule<Time, false, Time::Second, &Time::AddSeconds, &Time::SubtractSeconds, &Time::CountSecondsTo>;

		using EveryHourFrom = Internal::RepeatAndIncrementFieldRule<Time, true, &Time::AddHour, &Time::SubtractHour>;
		using EveryMinuteFrom = Internal::RepeatAndIncrementFieldRule<Time, true, &Time::AddMinute, &Time::SubtractMinute>;
		using EverySecondFrom = Internal::RepeatAndIncrementFieldRule<Time, true, &Time::AddSecond, &Time::SubtractSecond>;

		using EveryHourInRange = Internal::RepeatAndIncrementFieldRule<Time, false, &Time::AddHour, &Time::SubtractHour, &Time::CountHoursTo>;
		using EveryMinuteInRange = Internal::RepeatAndIncrementFieldRule<Time, false, &Time::AddMinute, &Time::SubtractMinute, &Time::CountMinutesTo>;
		using EverySecondInRange = Internal::RepeatAndIncrementFieldRule<Time, false, &Time::AddSecond, &Time::SubtractSecond, &Time::CountSecondsTo>;

		using OnlyOnce = Internal::NoRepeatRule<Time>;
	}

} // Loherangrin::Gems::DateTime::Rules

namespace AZ
{
	AZ_TYPE_INFO_SPECIALIZE(Loherangrin::Gems::DateTime::DateRecurrenceRuleBehaviorHandle, "{91EDFF90-F0F2-47B4-84B2-A91E3AA70C92}");
	AZ_TYPE_INFO_SPECIALIZE(Loherangrin::Gems::DateTime::TimeRecurrenceRuleBehaviorHandle, "{65D17BDF-34FD-4970-BA76-6AB5B0BE2409}");

} // AZ

#include "RecurrenceRules.inl"
