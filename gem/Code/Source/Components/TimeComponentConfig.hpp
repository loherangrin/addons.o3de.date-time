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

#include <AzCore/Math/Crc.h>
#include <AzCore/RTTI/TypeInfoSimple.h>

#include <DateTime/Types/Time.hpp>


namespace Loherangrin::Gems::DateTime
{
	struct TimeComponentConfig
	{
		AZ_TYPE_INFO(TimeComponentConfig, "{D4CEB62F-D176-4988-9ACD-18EFEE88331C}");
		static void Reflect(AZ::ReflectContext* io_context);

		bool m_isEnabled { true };

		bool m_hasDate { false };

		float m_timeScale { 1.0f };
		float m_timeSpeed { 1.0f };

		Time m_nowTime { 0, 0, 0 };
	};

} // Loherangrin::Gems::DateTime
