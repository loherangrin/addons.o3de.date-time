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

#if !defined(Q_MOC_RUN)
#include <DateTime/Constants/DateTimeEditorConstants.hpp>

#include "BaseDateTimeEditWidget.hpp"
#endif


namespace Loherangrin::Gems::DateTime
{
	class DateEditWidget
		: public BaseDateTimeEditWidget
	{
	public:
		explicit DateEditWidget(QWidget* io_parent = nullptr);

		Date GetValue() const;
		void SetValue(const Date& i_date);

		void SetMinValue(const Date& i_minDate);
		void SetMaxValue(const Date& i_maxDate);

	private:
		void OnYearChanged(double i_newValue);
		void OnMonthChanged(double i_newValue);

		void ResizeDaysRange(Date::Year i_year, Date::Month i_month);

		Date::Year m_lastYear;
	};

	using DateEditWidgetHandler = BaseDateTimeEditWidgetHandler<Date, DateEditWidget, UIHandlers::DATE>;

} // Loherangrin::Gems::DateTime
