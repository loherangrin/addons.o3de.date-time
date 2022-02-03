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

#include <AzCore/Serialization/SerializeContext.h>

#include <DateTime/Constants/DateTimeEditorConstants.hpp>

#include "../Widgets/DateEditWidget.hpp"
#include "../Widgets/TimeEditWidget.hpp"

#include "DateTimeEditorSystemComponent.hpp"

using Loherangrin::Gems::DateTime::DateTimeEditorSystemComponent;


void DateTimeEditorSystemComponent::Reflect(AZ::ReflectContext* io_context)
{
	if(auto serializeContext = azrtti_cast<AZ::SerializeContext*>(io_context))
	{
		serializeContext->Class<DateTimeEditorSystemComponent, AZ::Component>()
			->Version(0);
	}
}

void DateTimeEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& io_provided)
{
	io_provided.push_back(EditorServices::SYSTEM);
}

void DateTimeEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& io_incompatible)
{
	io_incompatible.push_back(EditorServices::SYSTEM);
}

void DateTimeEditorSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& io_required)
{}

void DateTimeEditorSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& io_dependent)
{}

void DateTimeEditorSystemComponent::Activate()
{
	RegisterWidgetHandler<DateEditWidgetHandler>();
	RegisterWidgetHandler<TimeEditWidgetHandler>();
}

void DateTimeEditorSystemComponent::Deactivate()
{}
