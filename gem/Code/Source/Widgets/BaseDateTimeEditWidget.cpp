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

#include <QHBoxLayout>

#include "DateEditWidget.hpp"


namespace Loherangrin::Gems::DateTime
{
	BaseDateTimeEditWidget::BaseDateTimeEditWidget(QWidget* io_parent, const FieldDefinitionsList& i_fieldDefinitions)
		: QWidget(io_parent)
		, m_fields {{ nullptr, nullptr, nullptr }}
		, m_minLimit { this, true }
		, m_maxLimit { this, false }
	{
		AZ_Assert(i_fieldDefinitions.size() == m_fields.size(), "Size of field definitions must be equal to the number of storable fields");

		QHBoxLayout* mainLayout = new QHBoxLayout(this);

		for(AZ::u8 i = 0; i < i_fieldDefinitions.size(); ++i)
		{
			const FieldDefinition& fieldDefinition = i_fieldDefinitions[i];

			CreateField(*mainLayout, i, fieldDefinition.m_label, fieldDefinition.m_minValue, fieldDefinition.m_maxValue, fieldDefinition.m_onValueChanged);
		}

		setLayout(mainLayout);
	}

	QWidget* BaseDateTimeEditWidget::GetFirstInTabOrder()
	{
		return m_fields[0]->GetSpinBox();
	}

	QWidget* BaseDateTimeEditWidget::GetLastInTabOrder()
	{
		return m_fields[2]->GetSpinBox();
	}

	void BaseDateTimeEditWidget::UpdateTabOrder()
	{
		setTabOrder(m_fields[0]->GetSpinBox(), m_fields[1]->GetSpinBox());
		setTabOrder(m_fields[1]->GetSpinBox(), m_fields[2]->GetSpinBox());
	}

	void BaseDateTimeEditWidget::SetMinLimit(const FieldValuesList& i_minLimitValues)
	{
		m_minLimit.SetRequired(i_minLimitValues);
		m_minLimit.Apply();
	}

	void BaseDateTimeEditWidget::SetMaxLimit(const FieldValuesList& i_maxLimitValues)
	{
		m_maxLimit.SetRequired(i_maxLimitValues);
		m_maxLimit.Apply();
	}

	void BaseDateTimeEditWidget::CreateField(QHBoxLayout& io_mainLayout, AZ::u8 i_index, const char* i_label, double i_minValue, double i_maxValue, const FieldCallback& i_onValueChanged)
	{
		AZ_Assert(i_index < m_fields.size(), "Index cannot exceed the number of fields");

		const auto styleAsCoordinate = static_cast<AzQtComponents::VectorElement::Coordinate>(i_index % 4);
		
		AzQtComponents::VectorElement*& field = m_fields[i_index];
		field = new AzQtComponents::VectorElement(this);
		field->setCoordinate(styleAsCoordinate);
		field->setLabel(i_label);

		AzQtComponents::DoubleSpinBox* fieldBox = field->GetSpinBox();
		fieldBox->setDecimals(0);
		fieldBox->setMinimum(i_minValue);
		fieldBox->setMaximum(i_maxValue);
		fieldBox->setSingleStep(1.0);

		m_minLimit.SetDefault(i_index, i_minValue);
		m_maxLimit.SetDefault(i_index, i_maxValue);

		connect(field->GetSpinBox(), QOverload<double>::of(&AzQtComponents::DoubleSpinBox::valueChanged), this, [this, i_index, i_onValueChanged](double i_newValue)
		{
			Q_EMIT valueChanged(i_newValue);
			Q_EMIT valueAtIndexChanged(i_index, i_newValue);

			if(i_onValueChanged)
			{
				i_onValueChanged(i_newValue);
			}

			if(i_index < this->N_FIELDS - 1)
			{
				this->m_minLimit.Apply(i_index + 1);
				this->m_maxLimit.Apply(i_index + 1);			
			}
		});
		connect(field, &AzQtComponents::VectorElement::editingFinished, this, &DateEditWidget::editingFinished);

		io_mainLayout.addWidget(field);
	}

	// ---

	BaseDateTimeEditWidget::FieldsLimit::FieldsLimit(BaseDateTimeEditWidget* io_widget, bool i_isMin)
		: m_isMin { i_isMin }
		, m_isValid { false }
		, m_defaultValues {{ 0, 0, 0 }}
		, m_requiredValues { m_defaultValues }
		, m_currentValues { m_defaultValues }
		, m_widget { io_widget }
	{}

	void BaseDateTimeEditWidget::FieldsLimit::Apply(AZ::u8 i_fromIndex)
	{
		if(m_isMin)
		{
			UpdateValues<&AzQtComponents::DoubleSpinBox::setMinimum>(i_fromIndex);
		}
		else
		{
			UpdateValues<&AzQtComponents::DoubleSpinBox::setMaximum>(i_fromIndex);
		}
	}

	bool BaseDateTimeEditWidget::FieldsLimit::IsValid() const
	{
		return m_isValid;
	}

	void BaseDateTimeEditWidget::FieldsLimit::SetDefault(AZ::u8 i_index, double i_value)
	{
		m_defaultValues[i_index] = i_value;
	}

	void BaseDateTimeEditWidget::FieldsLimit::SetRequired(const FieldValuesList& i_requiredValues)
	{
		m_isValid = true;
		m_requiredValues = i_requiredValues;
	}

};
