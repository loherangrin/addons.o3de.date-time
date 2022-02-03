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
	class TimeEditWidget
		: public BaseDateTimeEditWidget
	{
	public:
		explicit TimeEditWidget(QWidget* io_parent = nullptr);

		Time GetValue() const;
		void SetValue(const Time& i_time);

		void SetMinValue(const Time& i_minTime);
		void SetMaxValue(const Time& i_maxTime);
	};

	using TimeEditWidgetHandler = BaseDateTimeEditWidgetHandler<Time, TimeEditWidget, UIHandlers::TIME>;

} // Loherangrin::Gems::DateTime
