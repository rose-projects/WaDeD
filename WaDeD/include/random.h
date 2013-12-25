/*
 *  WaDeD - Short messages mesh network
 *
 *  Copyright (C) 2013 WaDeD-ROSE <waded-rose@googlegroups.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file  random.h
 * @brief Function generating random numbers.
 */

#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <stdint.h>

#define rand_mult 16807
#define rand_mod 2147483647

static uint32_t random_value;

/**
 * @brief Initialise the random number generator.
 *
 * @param seed  The initialiseing value of the generator.
 */
void rand_init(uint32_t seed)
{
    random_value = seed;
}

/**
 * @brief Get a random value.
 *
 * @return A pseudo random value.
 */
uint32_t random(void)
{
    return random_value = (random_value * rand_mult) % rand_mod;
}

#endif // __RANDOM_H__
