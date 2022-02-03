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
#include <AzCore/std/containers/array.h>
#include <AzCore/std/function/function_template.h>

#include <AzToolsFramework/UI/PropertyEditor/PropertyEditorAPI.h>

#include <AzQtComponents/Components/Widgets/SpinBox.h>

#include <DateTime/Types/Date.hpp>

#include <QWidget>
#endif

class QDoubleSpinBox;
class QHBoxLayout;

namespace AzQtComponents
{
	class VectorElement;
}


namespace Loherangrin::Gems::DateTime
{
	class BaseDateTimeEditWidget
		: public QWidget
	{
		Q_OBJECT

	Q_SIGNALS:
		void valueChanged(double newValue);
		void valueAtIndexChanged(int index, double newValue);
		void editingFinished();

	public Q_SLOTS:
		QWidget* GetFirstInTabOrder();
		QWidget* GetLastInTabOrder();
		void UpdateTabOrder();

	protected:
		using FieldCallback = AZStd::function<void(double)>;

		struct FieldDefinition
		{
			const char* m_label;
			double m_minValue;
			double m_maxValue;
			FieldCallback m_onValueChanged = nullptr;
		};

		static constexpr size_t N_FIELDS = 3;
		using FieldDefinitionsList = AZStd::array<FieldDefinition, N_FIELDS>;
		using FieldValuesList = AZStd::array<double, N_FIELDS>;
		using FieldsList = AZStd::array<AzQtComponents::VectorElement*, N_FIELDS>;

		BaseDateTimeEditWidget(QWidget* io_parent, const FieldDefinitionsList& i_fieldDefinitions);

		void SetMinLimit(const FieldValuesList& i_minLimitValues);
		void SetMaxLimit(const FieldValuesList& i_maxLimitValues);

		FieldsList m_fields;

	private:
		class FieldsLimit
		{
		public:
			FieldsLimit(BaseDateTimeEditWidget* io_widget, bool i_isMin);

			void Apply(AZ::u8 i_fromIndex = 0);

			bool IsValid() const;

			void SetDefault(AZ::u8 i_index, double i_value);
			void SetRequired(const FieldValuesList& i_requiredValues);

		private:
			template <void (QDoubleSpinBox::*c_setLimitMethod)(double)>
			void SetToFields(const FieldValuesList& i_limitValues, AZ::u8 i_fromIndex);

			template <void (QDoubleSpinBox::*c_setLimitMethod)(double)>
			void UpdateValues(AZ::u8 i_fromIndex);

			bool m_isMin;
			bool m_isValid;

			FieldValuesList m_defaultValues;
			FieldValuesList m_requiredValues;

			FieldValuesList m_currentValues;

			BaseDateTimeEditWidget* m_widget;
		};

		void CreateField(QHBoxLayout& io_mainLayout, AZ::u8 i_index, const char* i_label, double i_minValue, double i_maxValue, const FieldCallback& i_onValueChanged);

		FieldsLimit m_minLimit;
		FieldsLimit m_maxLimit;
	};
	
	// ---

	template <typename t_Property, typename t_Widget, AZ::u32 c_crc>
	class BaseDateTimeEditWidgetHandler
		: QObject
		, public AzToolsFramework::PropertyHandler<t_Property, t_Widget>
	{
		public:
			AZ::u32 GetHandlerName() const override;
			bool IsDefaultHandler() const override;

			QWidget* CreateGUI(QWidget* io_parent) override;
			void ConsumeAttribute(t_Widget* io_GUI, AZ::u32 i_attributeId, AzToolsFramework::PropertyAttributeReader* i_attributeValue, const char* i_debugName) override;

			bool ReadValuesIntoGUI(size_t i_index, t_Widget* o_GUI, const t_Property& i_instance, AzToolsFramework::InstanceDataNode* i_node) override;
			void WriteGUIValuesIntoProperty(size_t i_index, t_Widget* i_GUI, t_Property& o_instance, AzToolsFramework::InstanceDataNode* i_node) override;
	};

	template <typename t_Handler>
	void RegisterWidgetHandler();

} // Loherangrin::Gems::DateTime

#include "BaseDateTimeEditWidget.inl"
