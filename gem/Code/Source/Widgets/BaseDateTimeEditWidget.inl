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

#if !defined(Q_MOC_RUN)
#include <AzCore/Serialization/EditContextConstants.inl>

#include <AzQtComponents/Components/Widgets/VectorInput.h>

#include "BaseDateTimeEditWidget.hpp"
#endif


namespace Loherangrin::Gems::DateTime
{
	template <void (QDoubleSpinBox::*c_setLimitMethod)(double)>
	void BaseDateTimeEditWidget::FieldsLimit::SetToFields(const FieldValuesList& i_limitValues, AZ::u8 i_fromIndex)
	{
		const QSignalBlocker blocker(m_widget);

		for(AZ::u8 i = i_fromIndex; i < i_limitValues.size(); ++i)
		{
			const double fieldValue = i_limitValues[i];

			AzQtComponents::VectorElement*& field = m_widget->m_fields[i];
			AzQtComponents::DoubleSpinBox* fieldBox = field->GetSpinBox();
			(fieldBox->*c_setLimitMethod)(fieldValue);
		}
	}

	template <void (QDoubleSpinBox::*c_setLimitMethod)(double)>
	void BaseDateTimeEditWidget::FieldsLimit::UpdateValues(AZ::u8 i_fromIndex)
	{
		if(!IsValid())
		{
			return;
		}

		FieldValuesList newLimitValues = { m_requiredValues[0], m_defaultValues[1], m_defaultValues[2] };
		for(AZ::u8 i = 0; i < newLimitValues.size() - 1; ++i)
		{
			const double fieldValue = m_widget->m_fields[i]->GetValue();
			if(!AZ::IsClose(fieldValue, m_requiredValues[i], static_cast<double>(AZ::Constants::FloatEpsilon)))
			{
				break;
			}

			const AZ::u8 nextIndex = i + 1;
			newLimitValues[nextIndex] = m_requiredValues[nextIndex];
		}

		m_currentValues = newLimitValues;
		SetToFields<c_setLimitMethod>(newLimitValues, i_fromIndex);
	}

	// ---
	
	template <typename t_Property, typename t_Widget, AZ::u32 c_crc>
	AZ::u32 BaseDateTimeEditWidgetHandler<t_Property, t_Widget, c_crc>::GetHandlerName() const
	{
		return c_crc;
	}

	template <typename t_Property, typename t_Widget, AZ::u32 c_crc>
	bool BaseDateTimeEditWidgetHandler<t_Property, t_Widget, c_crc>::IsDefaultHandler() const
	{
		return true;
	}

	template <typename t_Property, typename t_Widget, AZ::u32 c_crc>
	QWidget* BaseDateTimeEditWidgetHandler<t_Property, t_Widget, c_crc>::CreateGUI(QWidget* io_parent)
	{
		t_Widget* newWidget = new t_Widget(io_parent);

		connect(newWidget, &t_Widget::valueChanged, this, [newWidget]()
		{
			EBUS_EVENT(AzToolsFramework::PropertyEditorGUIMessages::Bus, RequestWrite, newWidget);
		});

		connect(newWidget, &t_Widget::editingFinished, this, [newWidget]()
		{
			AzToolsFramework::PropertyEditorGUIMessages::Bus::Broadcast(&AzToolsFramework::PropertyEditorGUIMessages::Bus::Handler::OnEditingFinished, newWidget);
		});

		return newWidget;
	}

	template <typename t_Property, typename t_Widget, AZ::u32 c_crc>
	void BaseDateTimeEditWidgetHandler<t_Property, t_Widget, c_crc>::ConsumeAttribute(t_Widget* io_GUI, AZ::u32 i_attributeId, AzToolsFramework::PropertyAttributeReader* i_attributeValue, const char* i_debugName)
	{
		if(i_attributeId == AZ::Edit::Attributes::Min)
		{
			t_Property value;
			if(i_attributeValue->Read<t_Property>(value))
			{
				io_GUI->SetMinValue(value);
			}
		}
		else if(i_attributeId == AZ::Edit::Attributes::Max)
		{
			t_Property value;
			if(i_attributeValue->Read<t_Property>(value))
			{
				io_GUI->SetMaxValue(value);
			}
		}
	}

	template <typename t_Property, typename t_Widget, AZ::u32 c_crc>
	bool BaseDateTimeEditWidgetHandler<t_Property, t_Widget, c_crc>::ReadValuesIntoGUI(size_t i_index, t_Widget* o_GUI, const t_Property& i_instance, AzToolsFramework::InstanceDataNode* i_node)
	{
		o_GUI->SetValue(i_instance);
		return false;
	}

	template <typename t_Property, typename t_Widget, AZ::u32 c_crc>
	void BaseDateTimeEditWidgetHandler<t_Property, t_Widget, c_crc>::WriteGUIValuesIntoProperty(size_t i_index, t_Widget* i_GUI, t_Property& o_instance, AzToolsFramework::InstanceDataNode* i_node)
	{
		o_instance = i_GUI->GetValue();
	}

	// ---

	template <typename t_Handler>
	void RegisterWidgetHandler()
	{
		EBUS_EVENT(AzToolsFramework::PropertyTypeRegistrationMessages::Bus, RegisterPropertyType, new t_Handler());
	}

} // Loherangrin::Gems::DateTime
