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
 * @file  global.h
 * @brief Define global define to change the compilation mode.
 */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "ch.h"

#ifdef NDEBUG
#define assert(x) (void) x;
#else
static inline void assert(int condition)
{
    if (!condition)
        chDbgPanic("assert failed");
}
#endif

#endif // __GLOBAL_H__
