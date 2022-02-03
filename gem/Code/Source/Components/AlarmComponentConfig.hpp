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
#include <AzCore/RTTI/TypeInfoSimple.h>
#include <AzCore/std/containers/vector.h>

#include <DateTime/Types/Alarm.hpp>
#include <DateTime/Types/Date.hpp>
#include <DateTime/Types/RecurrenceRules.hpp>
#include <DateTime/Types/Time.hpp>


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
		struct RecurrenceRuleConfig
		{
		private:
			using ThisClass = RecurrenceRuleConfig<t_Value, c_defaultField1, c_defaultField2, c_defaultField3, t_Mode, t_Step, t_Condition, t_Times>;

		public:
			using Value = t_Value;
			using Mode = t_Mode;
			using Step = t_Step;
			using Condition = t_Condition;
			using Times = t_Times;

			static void Reflect(AZ::ReflectContext* io_context, const AZStd::string& i_prefix);
			// See end of the file for external AZ_TYPE_INFO

			bool AreFieldsVisible() const;

			bool IsStartVisible() const;
			bool IsIntervalVisible() const;
			bool IsConditionVisible() const;
			bool IsNRepeatsVisible() const;
			bool IsEndVisible() const;

			int GetMaxInterval() const;
			t_Value GetMinEnd() const;

			AZStd::unique_ptr<RecurrenceRule<t_Value>> CalculateRecurrenceRule() const;

			bool m_hasNoStartValue { true };
			t_Value m_startValue { c_defaultField1, c_defaultField2, c_defaultField3 };

			t_Mode m_repeatMode { static_cast<t_Mode>(0) };
			t_Step m_repeatInterval { 1 };

			t_Condition m_endCondition { static_cast<t_Condition>(0) };
			t_Times m_nRepeats { 1 };
			t_Value m_endValue { c_defaultField1, c_defaultField2, c_defaultField3 };

		private:
			template
			<
				t_Mode c_mode0, typename t_OnceRule,
				typename t_Field1, t_Mode c_mode1, typename t_FiniteIncrementRule1, typename FiniteSumRule1, typename t_InfiniteIncrementRule1, typename t_InfiniteSumRule1,
				typename t_Field2, t_Mode c_mode2, typename t_FiniteIncrementRule2, typename FiniteSumRule2, typename t_InfiniteIncrementRule2, typename t_InfiniteSumRule2,
				typename t_Field3, t_Mode c_mode3, typename t_FiniteIncrementRule3, typename FiniteSumRule3, typename t_InfiniteIncrementRule3, typename t_InfiniteSumRule3
			>
			AZStd::unique_ptr<RecurrenceRule<t_Value>> MakeUniqueRecurrenceRule() const;

			template
			<
				typename t_Field,
				typename t_FiniteIncrementRule, typename t_FiniteSumRule,
				typename t_InfiniteIncrementRule, typename t_InfiniteSumRule
			>
			AZStd::unique_ptr<RecurrenceRule<t_Value>> MakeUniqueRecurrenceRule() const;
		};
	}

	struct AlarmConfig
	{
		using DateRepeatInterval = Date::Year /* > Date::Month, Date::Day */;
		using TimeRepeatInterval = Time::Hour /* = Time::Minute, Time::Second */;

		using AlarmActionType = AlarmAction;

		enum class DateRepeatMode : AZ::u8
		{
			ONLY_ONCE = 0,
			EVERY_DAYS,
			EVERY_MONTHS,
			EVERY_YEARS
		};

		enum class TimeRepeatMode : AZ::u8
		{
			ONLY_ONCE = 0,
			EVERY_SECONDS,
			EVERY_MINUTES,
			EVERY_HOURS
		};

		enum class EndCondition : AZ::u8
		{
			INFINITE = 0,
			N_TIMES,
			NOT_AFTER
		};

		using DateRecurrenceRuleConfig = Internal::RecurrenceRuleConfig
		<
			Date, 1, 1, 1,
			DateRepeatMode, DateRepeatInterval,
			EndCondition, RecurrenceRuleRepeat
		>;

		using TimeRecurrenceRuleConfig = Internal::RecurrenceRuleConfig
		<
			Time, 0, 0, 0,
			TimeRepeatMode, TimeRepeatInterval,
			EndCondition, RecurrenceRuleRepeat
		>;

		AZ_TYPE_INFO(AlarmConfig, "{6B3994CE-F0CC-49E3-922A-42B2DFA86401}");
		static void Reflect(AZ::ReflectContext* io_context);
	
		bool IsActionNotifyRequesterVisible() const;

		DateRecurrenceRuleConfig m_dateRule {};
		TimeRecurrenceRuleConfig m_timeRule {};

		bool m_isSilent { false };

		AlarmActionType m_action { AlarmActionType::NOTIFY_REQUESTER };
		AZ::EntityId m_actionRequesterId {};

		static inline bool s_hasDate { false };
	};
	
	struct AlarmComponentConfig
	{
		AZ_TYPE_INFO(AlarmComponentConfig, "{61D0B6A8-C60D-4D94-851D-BD9E90AC5EC9}");
		static void Reflect(AZ::ReflectContext* io_context);

		bool m_isEnabled { false };

		AZStd::vector<AlarmConfig> m_alarms {};
	};

} // Loherangrin::Gems::DateTime

namespace AZ
{
	AZ_TYPE_INFO_SPECIALIZE(Loherangrin::Gems::DateTime::AlarmConfig::DateRecurrenceRuleConfig, "{9DB7DD38-61F9-4403-964B-2BA98573C890}");
	AZ_TYPE_INFO_SPECIALIZE(Loherangrin::Gems::DateTime::AlarmConfig::TimeRecurrenceRuleConfig, "{84B78F03-B179-4CA4-8702-9D04F4DA1FEB}");

} // AZ

#include "AlarmComponentConfig.inl"
