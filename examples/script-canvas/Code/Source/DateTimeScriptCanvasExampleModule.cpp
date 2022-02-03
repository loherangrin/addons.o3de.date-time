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


namespace Loherangrin::Gems::DateTime
{
	class DateTimeScriptCanvasExampleModule
		: public AZ::Module
	{
	public:
		AZ_RTTI(DateTimeScriptCanvasExampleModule, "{720D6725-A8CA-4B56-9E0D-B83CC5018C51}", AZ::Module);
		AZ_CLASS_ALLOCATOR(DateTimeScriptCanvasExampleModule, AZ::SystemAllocator, 0);

		DateTimeScriptCanvasExampleModule()
			: AZ::Module()
		{
			m_descriptors.insert(m_descriptors.end(), {});
		}

		AZ::ComponentTypeList GetRequiredSystemComponents() const override
		{
			return AZ::ComponentTypeList {};
		}
	};

} // Loherangrin::Gems::DateTime

AZ_DECLARE_MODULE_CLASS(Gem_Loherangrin_DateTime_ScriptCanvasExample, Loherangrin::Gems::DateTime::DateTimeScriptCanvasExampleModule)
