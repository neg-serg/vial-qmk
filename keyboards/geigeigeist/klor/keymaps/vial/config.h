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

/* Split handedness: the half that carries the USB cable is the left half, which
 * is QMK's default for the KLOR. The same firmware image therefore goes onto
 * both halves, with no per-half EEPROM image to write. */
#define MASTER_LEFT

/* Master/slave detection: with plain VBUS detection both halves can consider
 * themselves master (and then nothing crosses the half-duplex serial link),
 * so delegate by an actually established USB connection instead. */
#define SPLIT_USB_DETECT

/* The AVR bit-bang serial is timing tuned for the compiler QMK ships with; with
 * a much newer avr-gcc (15.x here) the default ~137 kbps setting leaves very
 * little timing margin, so run the link one step slower (~75 kbps). */
#define SELECT_SOFT_SERIAL_SPEED 2


/* Upstream UID from GEIGEIGEIST/qmk-config-klor, so Vial keeps recognising the
 * keyboard as the same device and an existing EEPROM keymap stays valid. */
#define VIAL_KEYBOARD_UID {0xBA, 0x38, 0x22, 0x3F, 0x8C, 0x42, 0x0F, 0x44}

/* Space savers for the 28 KiB ATmega32U4 flash budget. */
#undef LOCKING_SUPPORT_ENABLE
#undef LOCKING_RESYNC_ENABLE
#define NO_ACTION_ONESHOT
#define NO_MUSIC_MODE

/* Dynamic keymap layers. The ATmega32U4 has 1 KiB of EEPROM; the dynamic keymap
 * starts at VIA_EEPROM_CONFIG_END (41 B), each layer costs
 * MATRIX_ROWS*MATRIX_COLS*2 = 96 B plus NUM_ENCODERS*4 = 8 B for encoders, and
 * Vial requires >=100 B left over for dynamic macros (ceiling: 923 B). */
#define DYNAMIC_KEYMAP_LAYER_COUNT 7

/* Vial defaults each dynamic table to 8 entries because the EEPROM is >1000 B;
 * four is enough here and keeps the per-layer EEPROM budget balanced. */
#define VIAL_TAP_DANCE_ENTRIES 4
#define VIAL_COMBO_ENTRIES 4
#define VIAL_KEY_OVERRIDE_ENTRIES 4
#define VIAL_ALT_REPEAT_KEY_ENTRIES 4
