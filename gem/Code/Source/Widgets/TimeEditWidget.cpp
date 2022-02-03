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

#include "TimeEditWidget.hpp"

using Loherangrin::Gems::DateTime::Time;
using Loherangrin::Gems::DateTime::TimeEditWidget;


TimeEditWidget::TimeEditWidget(QWidget* io_parent)
	: BaseDateTimeEditWidget(io_parent, FieldDefinitionsList
		{{
			{ UILabels::HOUR, 0.0, static_cast<double>(Time::HOURS_IN_DAY - 1) },
			{ UILabels::MINUTE, 0.0, static_cast<double>(Time::MINUTES_IN_HOUR - 1) },
			{ UILabels::SECOND, 0.0, static_cast<double>(Time::SECONDS_IN_MINUTE - 1) }
		}})
{}

Time TimeEditWidget::GetValue() const
{
	const auto hours = static_cast<Time::Hour>(m_fields[0]->GetValue());
	const auto minutes = static_cast<Time::Minute>(m_fields[1]->GetValue());
	const auto seconds = static_cast<Time::Second>(m_fields[2]->GetValue());

	return Time { hours, minutes, seconds };
}

void TimeEditWidget::SetValue(const Time& i_time)
{
	if(!i_time.IsValid())
	{
		AZ_Assert(false, "Unable to set the widget to an invalid time");
		return;
	}

	const QSignalBlocker blocker(this);

	m_fields[0]->SetValue(i_time.GetHours());
	m_fields[1]->SetValue(i_time.GetMinutes());
	m_fields[2]->SetValue(i_time.GetSeconds());
}

void TimeEditWidget::SetMinValue(const Time& i_minTime)
{
	if(!i_minTime.IsValid())
	{
		AZ_Assert(false, "Unable to set the minimum value to an invalid time");
		return;
	}

	SetMinLimit(
	{
		static_cast<double>(i_minTime.GetHours()),
		static_cast<double>(i_minTime.GetMinutes()),
		static_cast<double>(i_minTime.GetSeconds())
	});
}

void TimeEditWidget::SetMaxValue(const Time& i_maxTime)
{
	if(!i_maxTime.IsValid())
	{
		AZ_Assert(false, "Unable to set the maximum value to an invalid time");
		return;
	}

	SetMaxLimit(
	{
		static_cast<double>(i_maxTime.GetHours()),
		static_cast<double>(i_maxTime.GetMinutes()),
		static_cast<double>(i_maxTime.GetSeconds())
	});
}
