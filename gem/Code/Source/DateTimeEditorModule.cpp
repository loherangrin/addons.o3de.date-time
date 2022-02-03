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

#include "EditorLevelComponents/DateTimeEditorLevelComponent.hpp"
#include "EditorSystemComponents/DateTimeEditorSystemComponent.hpp"

#include "DateTimeModuleInterface.hpp"


namespace Loherangrin::Gems::DateTime
{
	class DateTimeEditorModule
		: public DateTimeModuleInterface
	{
	public:
		AZ_RTTI(DateTimeEditorModule, "{4863673E-256F-4240-9356-17B0E1C163C9}", DateTimeModuleInterface);
		AZ_CLASS_ALLOCATOR(DateTimeEditorModule, AZ::SystemAllocator, 0);

		DateTimeEditorModule()
		{
			m_descriptors.insert(m_descriptors.end(),
			{
				DateTimeEditorLevelComponent::CreateDescriptor(),
				DateTimeEditorSystemComponent::CreateDescriptor()
			});
		}

		AZ::ComponentTypeList GetRequiredSystemComponents() const override
		{
			return AZ::ComponentTypeList
			{
				azrtti_typeid<DateTimeEditorSystemComponent>()
			};
		}
	};

} // Loherangrin::Gems::DateTime

AZ_DECLARE_MODULE_CLASS(Gem_Loherangrin_DateTime, Loherangrin::Gems::DateTime::DateTimeEditorModule)
