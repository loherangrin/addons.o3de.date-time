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

#include "AlarmComponentConfig.hpp"


namespace Loherangrin::Gems::DateTime
{
	namespace Internal
	{
		template
		<
			typename t_Value, AZ::u8 c_defaultField1, AZ::u8 c_defaultField2, AZ::u8 c_defaultField3,
			typename t_Mode, typename t_Step,
			typename t_Condition, typename t_Times
		>
		void RecurrenceRuleConfig<t_Value, c_defaultField1, c_defaultField2, c_defaultField3, t_Mode, t_Step, t_Condition, t_Times>::Reflect(AZ::ReflectContext* io_context, const AZStd::string& i_prefix)
		{
			if(auto serializeContext = azrtti_cast<AZ::SerializeContext*>(io_context))
			{
				serializeContext->Class<ThisClass>()
					->Version(0)
					->Field("Immediate", &ThisClass::m_hasNoStartValue)
					->Field("From", &ThisClass::m_startValue)
					->Field("Mode", &ThisClass::m_repeatMode)
					->Field("Interval", &ThisClass::m_repeatInterval)
					->Field("Condition", &ThisClass::m_endCondition)
					->Field("Repeats", &ThisClass::m_nRepeats)
					->Field("To", &ThisClass::m_endValue)
				;
			}

			if(auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(io_context))
			{
				behaviorContext->Class<ThisClass>((i_prefix + "RecurrenceRuleFactory").c_str())
					->Attribute(AZ::Script::Attributes::Module, ScriptAttributes::MODULE)
					->Attribute(AZ::Script::Attributes::Category, ScriptAttributes::CATEGORY)

					->Method("CreateRecurrenceRule", [](t_Value i_startValue, t_Mode i_repeatMode, t_Step i_repeatInterval, t_Condition i_endCondition, t_Times i_nRepeats, t_Value i_endValue) -> Internal::RecurrenceRuleBehaviorHandle<t_Value>
						{
							const ThisClass ruleConfig { false, i_startValue, i_repeatMode, i_repeatInterval, i_endCondition, i_nRepeats, i_endValue };
							AZStd::unique_ptr<RecurrenceRule<t_Value>> rule = ruleConfig.CalculateRecurrenceRule();

							const Internal::RecurrenceRuleBehaviorHandle<t_Value> ruleHandle { AZStd::move(rule) };

							return ruleHandle;
						})
				;
			}
		}

		template
		<
			typename t_Value, AZ::u8 c_defaultField1, AZ::u8 c_defaultField2, AZ::u8 c_defaultField3,
			typename t_Mode, typename t_Step,
			typename t_Condition, typename t_Times
		>
		bool RecurrenceRuleConfig<t_Value, c_defaultField1, c_defaultField2, c_defaultField3, t_Mode, t_Step, t_Condition, t_Times>::AreFieldsVisible() const
		{
			static_assert("Unsupported value type in a recurrence rule");
		}

		template
		<
			typename t_Value, AZ::u8 c_defaultField1, AZ::u8 c_defaultField2, AZ::u8 c_defaultField3,
			typename t_Mode, typename t_Step,
			typename t_Condition, typename t_Times
		>
		int RecurrenceRuleConfig<t_Value, c_defaultField1, c_defaultField2, c_defaultField3, t_Mode, t_Step, t_Condition, t_Times>::GetMaxInterval() const
		{
			static_assert("Unsupported value type in a recurrence rule");
		}

		template
		<
			typename t_Value, AZ::u8 c_defaultField1, AZ::u8 c_defaultField2, AZ::u8 c_defaultField3,
			typename t_Mode, typename t_Step,
			typename t_Condition, typename t_Times
		>
		t_Value RecurrenceRuleConfig<t_Value, c_defaultField1, c_defaultField2, c_defaultField3, t_Mode, t_Step, t_Condition, t_Times>::GetMinEnd() const
		{
			static_assert("Unsupported value type in a recurrence rule");
		}

		template
		<
			typename t_Value, AZ::u8 c_defaultField1, AZ::u8 c_defaultField2, AZ::u8 c_defaultField3,
			typename t_Mode, typename t_Step,
			typename t_Condition, typename t_Times
		>
		bool RecurrenceRuleConfig<t_Value, c_defaultField1, c_defaultField2, c_defaultField3, t_Mode, t_Step, t_Condition, t_Times>::IsStartVisible() const
		{
			if(!AreFieldsVisible())
			{
				return false;
			}

			return !m_hasNoStartValue;
		}

		template
		<
			typename t_Value, AZ::u8 c_defaultField1, AZ::u8 c_defaultField2, AZ::u8 c_defaultField3,
			typename t_Mode, typename t_Step,
			typename t_Condition, typename t_Times
		>
		bool RecurrenceRuleConfig<t_Value, c_defaultField1, c_defaultField2, c_defaultField3, t_Mode, t_Step, t_Condition, t_Times>::IsIntervalVisible() const
		{
			if(!AreFieldsVisible())
			{
				return false;
			}

			return (m_repeatMode != t_Mode::ONLY_ONCE);
		}

		template
		<
			typename t_Value, AZ::u8 c_defaultField1, AZ::u8 c_defaultField2, AZ::u8 c_defaultField3,
			typename t_Mode, typename t_Step,
			typename t_Condition, typename t_Times
		>
		bool RecurrenceRuleConfig<t_Value, c_defaultField1, c_defaultField2, c_defaultField3, t_Mode, t_Step, t_Condition, t_Times>::IsConditionVisible() const
		{
			if(!AreFieldsVisible())
			{
				return false;
			}

			return (m_repeatMode != t_Mode::ONLY_ONCE);
		}

		template
		<
			typename t_Value, AZ::u8 c_defaultField1, AZ::u8 c_defaultField2, AZ::u8 c_defaultField3,
			typename t_Mode, typename t_Step,
			typename t_Condition, typename t_Times
		>
		bool RecurrenceRuleConfig<t_Value, c_defaultField1, c_defaultField2, c_defaultField3, t_Mode, t_Step, t_Condition, t_Times>::IsNRepeatsVisible() const
		{
			if(!IsConditionVisible())
			{
				return false;
			}

			return (m_endCondition == t_Condition::N_TIMES);
		}

		template
		<
			typename t_Value, AZ::u8 c_defaultField1, AZ::u8 c_defaultField2, AZ::u8 c_defaultField3,
			typename t_Mode, typename t_Step,
			typename t_Condition, typename t_Times
		>
		bool RecurrenceRuleConfig<t_Value, c_defaultField1, c_defaultField2, c_defaultField3, t_Mode, t_Step, t_Condition, t_Times>::IsEndVisible() const
		{
			if(!IsConditionVisible())
			{
				return false;
			}

			return (m_endCondition == t_Condition::NOT_AFTER);
		}

		template
		<
			typename t_Value, AZ::u8 c_defaultField1, AZ::u8 c_defaultField2, AZ::u8 c_defaultField3,
			typename t_Mode, typename t_Step,
			typename t_Condition, typename t_Times
		>
		AZStd::unique_ptr<RecurrenceRule<t_Value>> RecurrenceRuleConfig<t_Value, c_defaultField1, c_defaultField2, c_defaultField3, t_Mode, t_Step, t_Condition, t_Times>::CalculateRecurrenceRule() const
		{
			static_assert("Unsupported value type in a recurrence rule");
			return nullptr;
		}

		template
		<
			typename t_Value, AZ::u8 c_defaultField1, AZ::u8 c_defaultField2, AZ::u8 c_defaultField3,
			typename t_Mode, typename t_Step,
			typename t_Condition, typename t_Times
		>
		template
		<
			t_Mode c_mode0, typename t_OnceRule,
			typename t_Field1, t_Mode c_mode1, typename t_FiniteIncrementRule1, typename t_FiniteSumRule1, typename t_InfiniteIncrementRule1, typename t_InfiniteSumRule1,
			typename t_Field2, t_Mode c_mode2, typename t_FiniteIncrementRule2, typename t_FiniteSumRule2, typename t_InfiniteIncrementRule2, typename t_InfiniteSumRule2,
			typename t_Field3, t_Mode c_mode3, typename t_FiniteIncrementRule3, typename t_FiniteSumRule3, typename t_InfiniteIncrementRule3, typename t_InfiniteSumRule3
		>
		AZStd::unique_ptr<RecurrenceRule<t_Value>> RecurrenceRuleConfig<t_Value, c_defaultField1, c_defaultField2, c_defaultField3, t_Mode, t_Step, t_Condition, t_Times>::MakeUniqueRecurrenceRule() const
		{
			AZStd::unique_ptr<RecurrenceRule<t_Value>> rule;
			
			switch(m_repeatMode)
			{
				case c_mode0:
				{
					rule = AZStd::make_unique<t_OnceRule>(m_startValue);
				}
				break;

				case c_mode1:
				{
					rule = MakeUniqueRecurrenceRule<t_Field1, t_FiniteIncrementRule1, t_FiniteSumRule1, t_InfiniteIncrementRule1, t_InfiniteSumRule1>();
				}
				break;

				case c_mode2:
				{
					rule = MakeUniqueRecurrenceRule<t_Field2, t_FiniteIncrementRule2, t_FiniteSumRule2, t_InfiniteIncrementRule2, t_InfiniteSumRule2>();
				}
				break;

				case c_mode3:
				{
					rule = MakeUniqueRecurrenceRule<t_Field3, t_FiniteIncrementRule3, t_FiniteSumRule3, t_InfiniteIncrementRule3, t_InfiniteSumRule3>();
				}
				break;

				default:
				{
					AZ_Assert(false, "Unsupported repeat mode: %d", m_repeatMode);
				}
			}

			return rule;
		}

		template
		<
			typename t_Value, AZ::u8 c_defaultField1, AZ::u8 c_defaultField2, AZ::u8 c_defaultField3,
			typename t_Mode, typename t_Step,
			typename t_Condition, typename t_Times
		>
		template
		<
			typename t_Field,
			typename t_FiniteIncrementRule, typename t_FiniteSumRule,
			typename t_InfiniteIncrementRule, typename t_InfiniteSumRule
		>
		AZStd::unique_ptr<RecurrenceRule<t_Value>> RecurrenceRuleConfig<t_Value, c_defaultField1, c_defaultField2, c_defaultField3, t_Mode, t_Step, t_Condition, t_Times>::MakeUniqueRecurrenceRule() const
		{
			AZStd::unique_ptr<RecurrenceRule<t_Value>> rule;

			switch(m_endCondition)
			{
				case t_Condition::INFINITE:
				{
					if(m_repeatInterval == 1)
					{
						rule = AZStd::make_unique<t_InfiniteIncrementRule>(m_startValue);
					}
					else
					{
						rule = AZStd::make_unique<t_InfiniteSumRule>(static_cast<t_Field>(m_repeatInterval), m_startValue);
					}
				}
				break;

				case t_Condition::N_TIMES:
				{
					if(m_repeatInterval == 1)
					{
						rule = AZStd::make_unique<t_FiniteIncrementRule>(m_startValue, m_nRepeats);
					}
					else
					{
						rule = AZStd::make_unique<t_FiniteSumRule>(static_cast<t_Field>(m_repeatInterval), m_startValue, m_nRepeats);
					}
				}
				break;

				case t_Condition::NOT_AFTER:
				{
					if(m_repeatInterval == 1)
					{
						rule = AZStd::make_unique<t_FiniteIncrementRule>(m_startValue, m_endValue);
					}
					else
					{
						rule = AZStd::make_unique<t_FiniteSumRule>(static_cast<t_Field>(m_repeatInterval), m_startValue, m_endValue);
					}
				}
				break;

				default:
				{
					AZ_Assert(false, "Unsupported end condition: %d", m_endCondition);
				}
			}

			return rule;
		}

	} // Internal

	// --- 

	template <>
	inline AZStd::unique_ptr<DateRecurrenceRule> AlarmConfig::DateRecurrenceRuleConfig::CalculateRecurrenceRule() const
	{
		return MakeUniqueRecurrenceRule
			<
				AlarmConfig::DateRepeatMode::ONLY_ONCE, DateRecurrenceRules::OnlyOnce,
				Date::Day, AlarmConfig::DateRepeatMode::EVERY_DAYS, DateRecurrenceRules::EveryDayInRange, DateRecurrenceRules::EveryNDaysInRange, DateRecurrenceRules::EveryDayFrom, DateRecurrenceRules::EveryNDaysFrom,
				Date::Month, AlarmConfig::DateRepeatMode::EVERY_MONTHS,DateRecurrenceRules::EveryMonthInRange, DateRecurrenceRules::EveryNMonthsInRange, DateRecurrenceRules::EveryMonthFrom, DateRecurrenceRules::EveryNMonthsFrom,
				Date::Year, AlarmConfig::DateRepeatMode::EVERY_YEARS, DateRecurrenceRules::EveryYearInRange, DateRecurrenceRules::EveryNYearsInRange, DateRecurrenceRules::EveryYearFrom, DateRecurrenceRules::EveryNYearsFrom
			>
			();
	}

	template <>
	inline bool AlarmConfig::DateRecurrenceRuleConfig::AreFieldsVisible() const
	{
		return AlarmConfig::s_hasDate;
	}

	template <>
	inline int AlarmConfig::DateRecurrenceRuleConfig::GetMaxInterval() const
	{
		switch(m_repeatMode)
		{
			case AlarmConfig::DateRepeatMode::EVERY_DAYS:
			{
				return static_cast<int>(Date::Calendar::DAYS_IN_SHORT_MONTH);
			}

			case AlarmConfig::DateRepeatMode::EVERY_MONTHS:
			{
				return static_cast<int>(Date::Calendar::MONTHS_IN_YEAR);
			}

			case AlarmConfig::DateRepeatMode::EVERY_YEARS:
			{
				return static_cast<int>(AZStd::numeric_limits<Date::Year>::max());
			}

			default:
			{
				return 0;
			}
		}
	}

	template <>
	inline Date AlarmConfig::DateRecurrenceRuleConfig::GetMinEnd() const
	{
		if(!m_startValue.IsValid() || m_hasNoStartValue)
		{
			return Date { AZStd::numeric_limits<Date::Year>::min(), 1, 1 };
		}

		return m_startValue;
	}

	// ---

	template <>
	inline AZStd::unique_ptr<TimeRecurrenceRule> AlarmConfig::TimeRecurrenceRuleConfig::CalculateRecurrenceRule() const
	{
		return MakeUniqueRecurrenceRule
			<
				AlarmConfig::TimeRepeatMode::ONLY_ONCE, TimeRecurrenceRules::OnlyOnce,
				Time::Second, AlarmConfig::TimeRepeatMode::EVERY_SECONDS, TimeRecurrenceRules::EverySecondInRange, TimeRecurrenceRules::EveryNSecondsInRange, TimeRecurrenceRules::EverySecondFrom, TimeRecurrenceRules::EveryNSecondsFrom,
				Time::Minute, AlarmConfig::TimeRepeatMode::EVERY_MINUTES, TimeRecurrenceRules::EveryMinuteInRange, TimeRecurrenceRules::EveryNMinutesInRange, TimeRecurrenceRules::EveryMinuteFrom, TimeRecurrenceRules::EveryNMinutesFrom,
				Time::Hour, AlarmConfig::TimeRepeatMode::EVERY_HOURS, TimeRecurrenceRules::EveryHourInRange, TimeRecurrenceRules::EveryNHoursInRange, TimeRecurrenceRules::EveryHourFrom, TimeRecurrenceRules::EveryNHoursFrom
			>
			();
	}

	template <>
	inline bool AlarmConfig::TimeRecurrenceRuleConfig::AreFieldsVisible() const
	{
		return true;
	}

	template <>
	inline int AlarmConfig::TimeRecurrenceRuleConfig::GetMaxInterval() const
	{
		switch(m_repeatMode)
		{
			case AlarmConfig::TimeRepeatMode::EVERY_HOURS:
			{
				return static_cast<int>(Time::HOURS_IN_DAY - 1);
			}

			case AlarmConfig::TimeRepeatMode::EVERY_MINUTES:
			{
				return static_cast<int>(Time::MINUTES_IN_HOUR - 1);
			}

			case AlarmConfig::TimeRepeatMode::EVERY_SECONDS:
			{
				return static_cast<int>(Time::SECONDS_IN_MINUTE - 1);
			}

			default:
			{
				return 0;
			}
		}
	}

	template <>
	inline Time AlarmConfig::TimeRecurrenceRuleConfig::GetMinEnd() const
	{
		if(!m_startValue.IsValid() || m_hasNoStartValue)
		{
			return Time { 0, 0, 1 };
		}

		Time minEndTime = m_startValue;
		minEndTime.AddSecond();

		return minEndTime;
	}

} // Loherangrin::Gems::DateTime
