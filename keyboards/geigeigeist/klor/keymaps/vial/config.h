/* Copyright 2022 GEIST <@geigeigeist>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

/* Split handedness is read from each half's own EEPROM instead of being derived
 * from which half carries the USB cable (the MASTER_LEFT default, where any half
 * with a cable declares itself the left half and the board mirrors itself). The
 * side is written together with the firmware by the EEPROM image that the AVR
 * `dfu-split-left` / `dfu-split-right` targets bundle in.
 *
 * As with any split: one USB cable at a time. With both halves connected over
 * USB each becomes a master on the same half-duplex serial link. */
#define EE_HANDS

/* Upstream UID from GEIGEIGEIST/qmk-config-klor, so Vial keeps recognising the
 * keyboard as the same device and an existing EEPROM keymap stays valid. */
#define VIAL_KEYBOARD_UID {0xBA, 0x38, 0x22, 0x3F, 0x8C, 0x42, 0x0F, 0x44}
