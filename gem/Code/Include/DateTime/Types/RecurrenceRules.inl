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

#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Serialization/SerializeContext.h>

#include <DateTime/Constants/DateTimeConstants.hpp>
#include <DateTime/Types/Date.hpp>


namespace Loherangrin::Gems::DateTime::Internal
{
	template <typename t_Value>
	RecurrenceRule<t_Value>::RecurrenceRule(const t_Value& i_startValue)
		: m_startValue { i_startValue }
	{}

	template <typename t_Value>
	RecurrenceRule<t_Value>::~RecurrenceRule()
	{}

	template <typename t_Value>
	const t_Value& RecurrenceRule<t_Value>::GetStart() const
	{
		return m_startValue;
	}

	template <typename t_Value>
	typename RecurrenceRule<t_Value>::const_iterator_pointer RecurrenceRule<t_Value>::Begin() const
	{
		return MakeUniqueIterator(this, nullptr, true);
	}

	template <typename t_Value>
	typename RecurrenceRule<t_Value>::const_iterator_pointer RecurrenceRule<t_Value>::End() const
	{
		return MakeUniqueIterator(nullptr, nullptr, true);
	}

	template <typename t_Value>
	typename RecurrenceRule<t_Value>::const_iterator_pointer RecurrenceRule<t_Value>::From(const t_Value& i_value, bool i_ceiling) const
	{
		return MakeUniqueIterator(this, &i_value, i_ceiling);
	}

	template <typename t_Value>
	AZStd::unique_ptr<RecurrenceRule<t_Value>> RecurrenceRule<t_Value>::Clone() const
	{
		BaseClass* rawPointer = CreateCopy();
		auto uniquePointer = AZStd::unique_ptr<BaseClass>(rawPointer);

		return uniquePointer;
	}

	template <typename t_Value>
	typename RecurrenceRule<t_Value>::const_iterator_pointer RecurrenceRule<t_Value>::MakeUniqueIterator(const ThisClass* i_rule, const t_Value* i_fromValue, bool i_ceiling) const
	{
		const_iterator* rawPointer = CreateIterator(i_rule, i_fromValue, i_ceiling);
		const_iterator_pointer uniquePointer { rawPointer };

		return uniquePointer;
	}

	template <typename t_Value>
	RecurrenceRule<t_Value>::ConstIterator::ConstIterator(const ThisClass* i_rule, const t_Value* i_fromValue, bool i_ceiling)
		: m_currentValue {}
		, m_lastChange { static_cast<typename t_Value::Change>(0) }
	{
		if(i_rule && (!i_fromValue || *i_fromValue <= i_rule->m_startValue))
		{
			m_currentValue = i_rule->m_startValue;
		}
	}

	template <typename t_Value>
	RecurrenceRule<t_Value>::ConstIterator::~ConstIterator()
	{}

	template <typename t_Value>
	typename t_Value::Change RecurrenceRule<t_Value>::ConstIterator::GetLastChange() const
	{
		return m_lastChange;
	}

	template <typename t_Value>
	typename RecurrenceRule<t_Value>::ConstIterator::reference RecurrenceRule<t_Value>::ConstIterator::operator*() const
	{
		return m_currentValue;
	}

	template <typename t_Value>
	typename RecurrenceRule<t_Value>::ConstIterator::pointer RecurrenceRule<t_Value>::ConstIterator::operator->() const
	{
		return &(m_currentValue);
	}

	template <typename t_Value>
	bool RecurrenceRule<t_Value>::ConstIterator::operator==(const ConstIterator& i_rhs) const
	{
		return (m_currentValue == i_rhs.m_currentValue);
	}

	template <typename t_Value>
	bool RecurrenceRule<t_Value>::ConstIterator::operator!=(const ConstIterator& i_rhs) const
	{
		return !operator==(i_rhs);
	}

	template <typename t_Value>
	typename RecurrenceRule<t_Value>::ConstIterator& RecurrenceRule<t_Value>::ConstIterator::operator++()
	{
		if(!m_currentValue.IsValid())
		{
			return *this;
		}

		const bool moved = Next();
		if(!moved)
		{
			m_currentValue = t_Value {};
		}

		return *this;
	}

	template <typename t_Value>
	typename RecurrenceRule<t_Value>::ConstIterator& RecurrenceRule<t_Value>::ConstIterator::operator--()
	{
		if(!m_currentValue.IsValid())
		{
			return *this;
		}

		const bool moved = Previous();
		if(!moved)
		{
			m_currentValue = t_Value {};
		}

		return *this;
	}

	// ---

	template <typename t_Value>
	NoRepeatRule<t_Value>::NoRepeatRule(const t_Value& i_when)
		: ParentClass(i_when)
	{}

	template <typename t_Value>
	NoRepeatRule<t_Value>::ConstIterator::ConstIterator(const ThisClass* i_rule, const t_Value* i_fromValue, bool i_ceiling)
		: ParentClass::ConstIterator(i_rule, i_fromValue, i_ceiling)
	{}

	template <typename t_Value>
	bool NoRepeatRule<t_Value>::ConstIterator::Next()
	{
		return false;
	}

	template <typename t_Value>
	bool NoRepeatRule<t_Value>::ConstIterator::Previous()
	{
		return false;
	}

	// ---

	template <typename t_Value>
	RepeatNTimesRule<t_Value, true>::RepeatNTimesRule(const t_Value& i_from)
		: ParentClass(i_from)
	{}

	template <typename t_Value>
	RepeatNTimesRule<t_Value, true>::ConstIterator::ConstIterator(const ThisClass* i_rule, const t_Value* i_fromValue, bool i_ceiling)
		: ParentClass::ConstIterator(i_rule, i_fromValue, i_ceiling)
	{}

	template <typename t_Value>
	bool RepeatNTimesRule<t_Value, true>::ConstIterator::Next()
	{
		IncreaseFields();
		return true;
	}

	template <typename t_Value>
	bool RepeatNTimesRule<t_Value, true>::ConstIterator::Previous()
	{
		DecreaseFields();
		return true;
	}


	template <typename t_Value>
	RepeatNTimesRule<t_Value, false>::RepeatNTimesRule(const t_Value& i_from, RecurrenceRuleRepeat i_nRepeats)
		: ParentClass(i_from)
		, m_nRepeats { i_nRepeats }
	{}

	template <typename t_Value>
	RepeatNTimesRule<t_Value, false>::ConstIterator::ConstIterator(const ThisClass* i_rule, const t_Value* i_fromValue, bool i_ceiling)
		: ParentClass::ConstIterator(i_rule, i_fromValue, i_ceiling)
	{
		if(i_rule)
		{
			m_nMaxValues = i_rule->m_nRepeats;
			m_nRemainingValues = i_rule->m_nRepeats;
		}
	}

	template <typename t_Value>
	bool RepeatNTimesRule<t_Value, false>::ConstIterator::Next()
	{
		if(m_nRemainingValues == 0)
		{
			return false;
		}

		--m_nRemainingValues;
		IncreaseFields();

		return true;
	}

	template <typename t_Value>
	bool RepeatNTimesRule<t_Value, false>::ConstIterator::Previous()
	{
		if(m_nRemainingValues == m_nMaxValues)
		{
			return false;
		}

		++m_nRemainingValues;
		DecreaseFields();

		return true;
	}

	// ---

	template
	<
		typename t_Value, bool c_isInfinite,
		typename t_Value::Change (t_Value::*c_incrementMethod)(),
		typename t_Value::Change (t_Value::*c_decrementMethod)(),
		typename t_Value::Difference (t_Value::*c_countMethod)(const t_Value&) const,
		typename t_Field,
		t_Field (t_Value::*c_constrainedFieldGetter)() const,
		void (t_Value::*c_constrainedFieldSetter)(t_Field)
	>
	RepeatAndIncrementFieldRule<t_Value, c_isInfinite, c_incrementMethod, c_decrementMethod, c_countMethod, t_Field, c_constrainedFieldGetter, c_constrainedFieldSetter>::ConstIterator::ConstIterator(const ThisClass* i_rule, const t_Value* i_fromValue, bool i_ceiling)
		: ParentClass::ConstIterator(i_rule, i_fromValue, i_ceiling)
	{
		if(i_rule)
		{
			if constexpr((c_constrainedFieldGetter != nullptr) && (c_constrainedFieldSetter != nullptr))
			{
				m_startConstrainedField = (i_rule->m_startValue.*c_constrainedFieldGetter)();
			}

			if(i_fromValue && (*i_fromValue > i_rule->m_startValue))
			{
				if constexpr(c_isInfinite)
				{
					this->m_currentValue = *i_fromValue;
				}
				else
				{
					static_assert(c_countMethod != nullptr, "A count method is required");

					typename t_Value::Difference offset = (i_rule->m_startValue.*c_countMethod)(*i_fromValue);
					AZ_Assert(offset >= 0, "Offset cannot be negative since fromValue is in the future");

					auto remainingValues = static_cast<typename t_Value::Difference>(this->m_nRemainingValues);
					if(offset > remainingValues)
					{
						this->m_currentValue = t_Value {};
						this->m_nRemainingValues = 0;

						return;
					}

					this->m_nRemainingValues = remainingValues - offset;

					this->m_currentValue = i_rule->m_startValue;
					for(; offset > 0; --offset)
					{
						(this->m_currentValue.*c_incrementMethod)();
					}

					if(i_ceiling && this->m_currentValue < *i_fromValue)
					{
						--(this->m_nRemainingValues);

						(this->m_currentValue.*c_incrementMethod)();
					}
				}
			}
		}
	}

	template
	<
		typename t_Value, bool c_isInfinite,
		typename t_Value::Change (t_Value::*c_incrementMethod)(),
		typename t_Value::Change (t_Value::*c_decrementMethod)(),
		typename t_Value::Difference (t_Value::*c_countMethod)(const t_Value&) const,
		typename t_Field,
		t_Field (t_Value::*c_constrainedFieldGetter)() const,
		void (t_Value::*c_constrainedFieldSetter)(t_Field)
	>
	void RepeatAndIncrementFieldRule<t_Value, c_isInfinite, c_incrementMethod, c_decrementMethod, c_countMethod, t_Field, c_constrainedFieldGetter, c_constrainedFieldSetter>::ConstIterator::IncreaseFields()
	{
		if constexpr((c_constrainedFieldGetter != nullptr) && (c_constrainedFieldSetter != nullptr))
		{
			if((this->m_currentValue.*c_constrainedFieldGetter)() != m_startConstrainedField)
			{
				(this->m_currentValue.*c_constrainedFieldSetter)(m_startConstrainedField);
			}
		}

		this->m_lastChange = (this->m_currentValue.*c_incrementMethod)();
	}

	template
	<
		typename t_Value, bool c_isInfinite,
		typename t_Value::Change (t_Value::*c_incrementMethod)(),
		typename t_Value::Change (t_Value::*c_decrementMethod)(),
		typename t_Value::Difference (t_Value::*c_countMethod)(const t_Value&) const,
		typename t_Field,
		t_Field (t_Value::*c_constrainedFieldGetter)() const,
		void (t_Value::*c_constrainedFieldSetter)(t_Field)
	>
	void RepeatAndIncrementFieldRule<t_Value, c_isInfinite, c_incrementMethod, c_decrementMethod, c_countMethod, t_Field, c_constrainedFieldGetter, c_constrainedFieldSetter>::ConstIterator::DecreaseFields()
	{
		if constexpr((c_constrainedFieldGetter != nullptr) && (c_constrainedFieldSetter != nullptr))
		{
			if((this->m_currentValue.*c_constrainedFieldGetter)() != m_startConstrainedField)
			{
				(this->m_currentValue.*c_constrainedFieldSetter)(m_startConstrainedField);
			}
		}

		this->m_lastChange = (this->m_currentValue.*c_decrementMethod)();
	}

	// ---

	template
	<
		typename t_Value, bool c_isInfinite, typename t_Step,
		typename t_Value::Change (t_Value::*c_sumMethod)(t_Step),
		typename t_Value::Change (t_Value::*c_differenceMethod)(t_Step),
		typename t_Value::Difference (t_Value::*c_countMethod)(const t_Value&) const,
		typename t_Field,
		t_Field (t_Value::*c_constrainedFieldGetter)() const,
		void (t_Value::*c_constrainedFieldSetter)(t_Field)
	>
	RepeatAndSumFieldRule<t_Value, c_isInfinite, t_Step, c_sumMethod, c_differenceMethod, c_countMethod, t_Field, c_constrainedFieldGetter, c_constrainedFieldSetter>::ConstIterator::ConstIterator(const ThisClass* i_rule, const t_Value* i_fromValue, bool i_ceiling)
		: ParentClass::ConstIterator(i_rule, i_fromValue, i_ceiling)
	{
		if(i_rule)
		{
			if constexpr((c_constrainedFieldGetter != nullptr) && (c_constrainedFieldSetter != nullptr))
			{
				m_startConstrainedField = (i_rule->m_startValue.*c_constrainedFieldGetter)();
			}

			m_valueStep = i_rule->m_valueStep;

			if(i_fromValue && (*i_fromValue > i_rule->m_startValue))
			{
				static_assert(c_countMethod != nullptr, "A count method is required");

				typename t_Value::Difference offset = (i_rule->m_startValue.*c_countMethod)(*i_fromValue);
				offset = AZStd::floor(offset / static_cast<typename t_Value::Difference>(m_valueStep));
				AZ_Assert(offset >= 0, "Offset cannot be negative since fromValue is in the future");

				if constexpr(!c_isInfinite)
				{
					auto remainingValues = static_cast<typename t_Value::Difference>(this->m_nRemainingValues);
					if(offset > remainingValues)
					{
						this->m_currentValue = t_Value {};
						this->m_nRemainingValues = 0;

						return;
					}

					this->m_nRemainingValues = remainingValues - offset;
				}

				this->m_currentValue = i_rule->m_startValue;
				for(; offset > 0; --offset)
				{
					(this->m_currentValue.*c_sumMethod)(m_valueStep);
				}

				if(i_ceiling && this->m_currentValue < *i_fromValue)
				{
					if constexpr(!c_isInfinite)
					{
						--(this->m_nRemainingValues);
					}

					(this->m_currentValue.*c_sumMethod)(m_valueStep);
				}
			}
		}
	}

	template
	<
		typename t_Value, bool c_isInfinite, typename t_Step,
		typename t_Value::Change (t_Value::*c_sumMethod)(t_Step),
		typename t_Value::Change (t_Value::*c_differenceMethod)(t_Step),
		typename t_Value::Difference (t_Value::*c_countMethod)(const t_Value&) const,
		typename t_Field,
		t_Field (t_Value::*c_constrainedFieldGetter)() const,
		void (t_Value::*c_constrainedFieldSetter)(t_Field)
	>
	void RepeatAndSumFieldRule<t_Value, c_isInfinite, t_Step, c_sumMethod, c_differenceMethod, c_countMethod, t_Field, c_constrainedFieldGetter, c_constrainedFieldSetter>::ConstIterator::IncreaseFields()
	{
		if constexpr((c_constrainedFieldGetter != nullptr) && (c_constrainedFieldSetter != nullptr))
		{
			if((this->m_currentValue.*c_constrainedFieldGetter)() != m_startConstrainedField)
			{
				(this->m_currentValue.*c_constrainedFieldSetter)(m_startConstrainedField);
			}
		}

		this->m_lastChange = (this->m_currentValue.*c_sumMethod)(m_valueStep);
	}

	template
	<
		typename t_Value, bool c_isInfinite, typename t_Step,
		typename t_Value::Change (t_Value::*c_sumMethod)(t_Step),
		typename t_Value::Change (t_Value::*c_differenceMethod)(t_Step),
		typename t_Value::Difference (t_Value::*c_countMethod)(const t_Value&) const,
		typename t_Field,
		t_Field (t_Value::*c_constrainedFieldGetter)() const,
		void (t_Value::*c_constrainedFieldSetter)(t_Field)
	>
	void RepeatAndSumFieldRule<t_Value, c_isInfinite, t_Step, c_sumMethod, c_differenceMethod, c_countMethod, t_Field, c_constrainedFieldGetter, c_constrainedFieldSetter>::ConstIterator::DecreaseFields()
	{
		if constexpr((c_constrainedFieldGetter != nullptr) && (c_constrainedFieldSetter != nullptr))
		{
			if((this->m_currentValue.*c_constrainedFieldGetter)() != m_startConstrainedField)
			{
				(this->m_currentValue.*c_constrainedFieldSetter)(m_startConstrainedField);
			}
		}

		this->m_lastChange = (this->m_currentValue.*c_differenceMethod)(m_valueStep);
	}

	// ---

	template <typename t_Value>
	void RecurrenceRuleBehaviorHandle<t_Value>::Reflect(AZ::ReflectContext* io_context, const AZStd::string& i_prefix)
	{
		if(auto serializeContext = azrtti_cast<AZ::SerializeContext*>(io_context))
		{
			serializeContext->Class<ThisClass>()
				->Version(0)
			;
		}

		if(auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(io_context))
		{
			behaviorContext->Class<ThisClass>((i_prefix + "RecurrenceRule").c_str());
		}
	}

	template <typename t_Value>
	RecurrenceRuleBehaviorHandle<t_Value>::RecurrenceRuleBehaviorHandle()
		: m_rule { nullptr }
	{}

	template <typename t_Value>
	RecurrenceRuleBehaviorHandle<t_Value>::RecurrenceRuleBehaviorHandle(AZStd::unique_ptr<RecurrenceRule<t_Value>>&& io_rule)
		: m_rule { AZStd::move(io_rule) }
	{}

	template <typename t_Value>
	RecurrenceRuleBehaviorHandle<t_Value>::RecurrenceRuleBehaviorHandle(const RecurrenceRuleBehaviorHandle& i_other)
		: m_rule { (i_other.m_rule) ? i_other.m_rule->Clone() : nullptr }
	{}

	template <typename t_Value>
	RecurrenceRuleBehaviorHandle<t_Value>::RecurrenceRuleBehaviorHandle(RecurrenceRuleBehaviorHandle&& io_other)
		: m_rule { AZStd::move(io_other.m_rule) }
	{}

	template <typename t_Value>
	RecurrenceRuleBehaviorHandle<t_Value>& RecurrenceRuleBehaviorHandle<t_Value>::operator=(const RecurrenceRuleBehaviorHandle& i_other)
	{
		m_rule = (i_other.m_rule) ? i_other.m_rule->Clone() : nullptr;
		
		return *this;
	}

	template <typename t_Value>
	RecurrenceRuleBehaviorHandle<t_Value>& RecurrenceRuleBehaviorHandle<t_Value>::operator=(RecurrenceRuleBehaviorHandle&& io_other)
	{
		m_rule = AZStd::move(io_other.m_rule);

		return *this;
	}

	template <typename t_Value>
	const RecurrenceRule<t_Value>* RecurrenceRuleBehaviorHandle<t_Value>::GetRule() const
	{
		return m_rule.get();
	}

	template <typename t_Value>
	RecurrenceRule<t_Value>* RecurrenceRuleBehaviorHandle<t_Value>::GetRule()
	{
		return m_rule.get();
	}

	template <typename t_Value>
	bool RecurrenceRuleBehaviorHandle<t_Value>::IsValid() const
	{
		return (m_rule != nullptr);
	}

} // Loherangrin::Gems::DateTime::Rules::Internal
