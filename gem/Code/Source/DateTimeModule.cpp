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

#include "DateTimeModuleInterface.hpp"


namespace Loherangrin::Gems::DateTime
{
    class DateTimeModule
        : public DateTimeModuleInterface
    {
    public:
        AZ_RTTI(DateTimeModule, "{654704BF-D615-4A1F-B712-FD772666CEBB}", DateTimeModuleInterface);
        AZ_CLASS_ALLOCATOR(DateTimeModule, AZ::SystemAllocator, 0);
    };

} // Loherangrin::Gems::DateTime

AZ_DECLARE_MODULE_CLASS(Gem_Loherangrin_DateTime, Loherangrin::Gems::DateTime::DateTimeModule)
