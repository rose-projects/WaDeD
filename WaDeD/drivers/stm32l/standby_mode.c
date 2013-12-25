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

#include "standby_mode.h"
/* WATCH DOG */
//IWDG base = 0x40003000 - 0x400033FF

//key register
//address offset 0x00
//reset value 0x00000000
#define IWDG_KR (*(volatile uint16_t*)(0x40003000))
//values
#define WD_RLOAD 0xAAAA
#define WD_START 0xCCCC
#define WD_ACCESS 0x5555

#define SET_WD_RLOAD (IWDG_KR = WD_RLOAD)
#define SET_WD_START (IWDG_KR = WD_START)
#define SET_WD_ACCESS (IWDG_KR = WD_ACCESS)

//prescaler register
//address offset 0x04
//reset value 0x00000000
#define IWDG_PR (*(volatile uint16_t*)(0x40003000 + 0x04))
//value
#define WD_DIV4 0
#define WD_DIV8 1
#define WD_DIV16 2
#define WD_DIV32 3
#define WD_DIV64 4
#define WD_DIV128 5
#define WD_DIV256 6

//reload register
//address offset 0x08
//reset value 0x00000FFF
#define IWDG_RLR (*(volatile uint16_t*)(0x40003000 + 0x08))

//status register
//address offset 0x0C
//reset value 0x00000000
#define IWDG_SR (*(volatile uint16_t*)(0x40003000 + 0x0C))
//macros
#define RVU (IWDG_SR & 1)
#define PVU (IWDG_SR & 2)

void wd_set(uint16_t sec){
    uint16_t time = sec * 145;
    if(time > 0xfff) time = 0xfff;
    SET_WD_ACCESS;
    while(RVU){}
    while(PVU){}
    IWDG_PR = WD_DIV256;
    IWDG_RLR = time;
    SET_WD_RLOAD;
    SET_WD_START;
}

/* CONSTROL/STATUS REGISTER */


/* POWER CONTROL REGISTERS */
//PWR base = 0x400073FF
//PWR power control register
//address offset: 0x00
//reset value 0x00001000
#define PWR_CR (*(volatile uint16_t*)(0x400073FF))
//set standby mode
//#define SET_PDDS (PWR_CR = (PWR_CR | 2))
#define SET_PDDS (PWR_CR += 2)
//set run deepsleep/sleep/low power mode
//#define SET_LPSDSR (PWR_CR = (PWR_CR | 1))
#define SET_LPSDSR (PWR_CR += 1)
//set low power mode run
//#define SET_LPRUN (PWR_CR = (PWR_CR | (1 << 14)))
#define SET_LPRUN (PWR_CR += (1 << 14))
//#define CLEAR_WUF (PWR_CR = (PWR_CR & 4))
//CORTEX M3 SYSTEM CONTROL REGISTER
#define ARM_SCR (*(volatile uint16_t*)(0xE000ED10))
#define SET_SLEEPDEEP (ARM_SCR = (ARM_SCR | 4))

//PWR power controll/status register
//add offset 0x04
//reset value 0x00000008
#define PWR_CSR (*(volatile uint16_t*)(0x400073FF + 0x04))
#define CLEAR_WUF (PWR_CSR = (PWR_CSR & ~1))
#define CLEAR_EWUP (PWR_CSR = (PWR_CSR & ~(7 << 8)))

void deep_sleep(uint16_t sec){
    wd_set(sec);
    SET_SLEEPDEEP;
    SET_PDDS;
    CLEAR_WUF;
    CLEAR_EWUP;
    SET_LPSDSR;
    SET_LPRUN;
    asm("WFI;");
}
