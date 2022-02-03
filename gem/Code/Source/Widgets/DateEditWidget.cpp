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

#include <AzCore/std/limits.h>

#include "DateEditWidget.hpp"

using Loherangrin::Gems::DateTime::Date;
using Loherangrin::Gems::DateTime::DateEditWidget;


DateEditWidget::DateEditWidget(QWidget* io_parent)
	: BaseDateTimeEditWidget(io_parent, FieldDefinitionsList
		{{
			{ UILabels::YEAR, static_cast<double>(AZStd::numeric_limits<Date::Year>::min()), static_cast<double>(AZStd::numeric_limits<Date::Year>::max()), AZStd::bind(&DateEditWidget::OnYearChanged, this, AZStd::placeholders::_1) },
			{ UILabels::MONTH, 1.0, static_cast<double>(Date::Calendar::MONTHS_IN_YEAR), AZStd::bind(&DateEditWidget::OnMonthChanged, this, AZStd::placeholders::_1) },
			{ UILabels::DAY, 1.0, static_cast<double>(Date::Calendar::GetLastMonthDay(1, 1)) }
		}})
{}

Date DateEditWidget::GetValue() const
{
	const auto year = static_cast<Date::Year>(m_fields[0]->GetValue());
	const auto month = static_cast<Date::Month>(m_fields[1]->GetValue());
	const auto day = static_cast<Date::Day>(m_fields[2]->GetValue());

	return Date { year, month, day };
}

void DateEditWidget::SetValue(const Date& i_date)
{
	if(!i_date.IsValid())
	{
		AZ_Assert(false, "Unable to set the widget to an invalid date");
		return;
	}

	const QSignalBlocker blocker(this);

	m_fields[0]->SetValue(i_date.GetYear());
	m_fields[1]->SetValue(i_date.GetMonthNumber());
	m_fields[2]->SetValue(i_date.GetDayNumber());

	m_lastYear = i_date.GetYear();
}

void DateEditWidget::SetMinValue(const Date& i_minDate)
{
	if(!i_minDate.IsValid())
	{
		AZ_Assert(false, "Unable to set the minimum value to an invalid date");
		return;
	}

	SetMinLimit(
	{
		static_cast<double>(i_minDate.GetYear()),
		static_cast<double>(i_minDate.GetMonthNumber()),
		static_cast<double>(i_minDate.GetDayNumber())
	});
}

void DateEditWidget::SetMaxValue(const Date& i_maxDate)
{
	if(!i_maxDate.IsValid())
	{
		AZ_Assert(false, "Unable to set the maximum value to an invalid date");
		return;
	}

	SetMaxLimit(
	{
		static_cast<double>(i_maxDate.GetYear()),
		static_cast<double>(i_maxDate.GetMonthNumber()),
		static_cast<double>(i_maxDate.GetDayNumber())
	});
}

void DateEditWidget::OnYearChanged(double i_newValue)
{
	const auto newYear = static_cast<Date::Year>(i_newValue);
	if(newYear == 0)
	{
		AzQtComponents::VectorElement& yearField = (*m_fields[0]);
		AzQtComponents::DoubleSpinBox* yearBox = yearField.GetSpinBox();

		const double targetYear = (m_lastYear > 0) ? -1.0 : 1.0;

		const QSignalBlocker blocker(yearBox);
		yearField.setValue(targetYear);
		yearBox->setValue(targetYear);
		OnYearChanged(targetYear);

		return;
	}
	m_lastYear = newYear;

	const auto month = static_cast<Date::Month>(m_fields[1]->GetValue());

	ResizeDaysRange(newYear, month);
}

void DateEditWidget::OnMonthChanged(double i_newValue)
{
	const auto year = static_cast<Date::Year>(m_fields[0]->GetValue());
	const auto newMonth = static_cast<Date::Month>(i_newValue);

	ResizeDaysRange(year, newMonth);
}

void DateEditWidget::ResizeDaysRange(Date::Year i_year, Date::Month i_month)
{
	const Date::Day newLastMonthDay = Date::Calendar::GetLastMonthDay(i_month, i_year);

	AzQtComponents::VectorElement& dayField = (*m_fields[2]);
	AzQtComponents::DoubleSpinBox* dayBox = dayField.GetSpinBox();
	const auto lastMonthDay = static_cast<Date::Day>(dayBox->maximum());
	if(lastMonthDay == newLastMonthDay)
	{
		return;
	}

	const auto day = static_cast<Date::Day>(dayField.GetValue());
	if(day > newLastMonthDay)
	{
		dayField.SetValue(static_cast<double>(newLastMonthDay));
	}

	const QSignalBlocker blocker(dayBox);
	dayBox->setMaximum(static_cast<double>(newLastMonthDay));
}
