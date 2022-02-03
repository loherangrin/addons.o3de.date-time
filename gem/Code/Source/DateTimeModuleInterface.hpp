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

#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include "Components/AlarmComponent.hpp"
#include "Components/DateComponent.hpp"
#include "Components/TimeComponent.hpp"


namespace Loherangrin::Gems::DateTime
{
	class DateTimeModuleInterface
		: public AZ::Module
	{
	public:
		AZ_RTTI(DateTimeModuleInterface, "{740BFC10-D392-4F4F-9FDD-013C653E621D}", AZ::Module);
		AZ_CLASS_ALLOCATOR(DateTimeModuleInterface, AZ::SystemAllocator, 0);

		DateTimeModuleInterface()
		{
			m_descriptors.insert(m_descriptors.end(),
			{
				DateComponent::CreateDescriptor(),
				TimeComponent::CreateDescriptor(),

				// It must be the LAST entry since it depends on classes that
				// are serialized/reflected by the previous components
				AlarmComponent::CreateDescriptor()
			});
		}

		AZ::ComponentTypeList GetRequiredSystemComponents() const override
		{
			return AZ::ComponentTypeList {};
		}
	};

} // Loherangrin::Gems::DateTime
