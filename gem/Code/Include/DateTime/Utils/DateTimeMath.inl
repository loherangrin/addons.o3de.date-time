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

#include <AzCore/std/math.h>

#include "DateTimeMath.hpp"


namespace Loherangrin::Gems::DateTime::Utils
{
	AZ_FORCE_INLINE float CalculateQuotient(float i_dividend, float i_divisor)
	{
		const float quotient = AZStd::trunc(i_dividend / i_divisor);

		return quotient;
	}

	AZ_FORCE_INLINE float CalculateRemainder(float i_dividend, float i_divisor)
	{
		const float quotient = CalculateQuotient(i_dividend, i_divisor);
		const float remainder = CalculateRemainder(i_dividend, i_divisor, quotient);

		return remainder;
	}

	AZ_FORCE_INLINE float CalculateRemainder(float i_dividend, float i_divisor, float i_quotient)
	{
		const float remainder = i_dividend - (i_quotient * i_divisor);

		return remainder;
	}

} // Loherangrin::Gems::DateTime::Utils
