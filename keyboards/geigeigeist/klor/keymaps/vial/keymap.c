// Copyright 2026 Sergey Miroshnichenko
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Keymap-level OLED screen for the KLOR.
//
// Only the left (master) half draws anything; the right half keeps its display
// off.  On startup the master plays a short "terminal" intro (memory dump ->
// boot log -> keystore decryption -> ACCESS GRANTED) and then switches to a
// live text status: layer, Vial layout size, modifiers, host locks, WPM and
// uptime.  Everything is generated in code, so there are no stored frames.

#include QMK_KEYBOARD_H

#ifdef OLED_ENABLE

#    define INTRO_LENGTH_MS 3900
#    define DRAW_TICK_MS 70
#    define STATUS_TICK_MS 200

static uint32_t last_draw   = 0;
static uint32_t intro_start = 0;
static bool     intro_shown = false;
static uint32_t rng_state   = 0x4B4C4F52; // "KLOR"

// ┌───────────────────────────────────────────────────────────┐
// │ s m a l l   h e l p e r s                                 │
// └───────────────────────────────────────────────────────────┘

static uint32_t rng(void) {
    rng_state ^= rng_state << 13;
    rng_state ^= rng_state >> 17;
    rng_state ^= rng_state << 5;
    return rng_state;
}

static const char hex_digit[] = "0123456789ABCDEF";

static void put(uint8_t col, uint8_t line, const char *text) {
    oled_set_cursor(col, line);
    oled_write(text, false);
}

// The modifier or lock is shown as reverse video while it is active.  Only the
// label itself is inverted: the padding space stays normal so that neighbouring
// flags do not merge into a single solid block.
static void put_flag(uint8_t col, uint8_t line, const char *text, bool active) {
    uint8_t len = 0;
    while (text[len]) {
        len++;
    }
    while (len && text[len - 1] == ' ') {
        len--;
    }

    oled_set_cursor(col, line);
    for (uint8_t i = 0; i < len; i++) {
        oled_write_char(text[i], active);
    }
    oled_write(&text[len], false);
}

// Print a value right-aligned to right_col, so its ones column does not move
// when the value grows.
static void put_num(uint8_t right_col, uint8_t line, uint8_t value) {
    char    buf[4];
    uint8_t digits = value >= 100 ? 3 : (value >= 10 ? 2 : 1);

    for (uint8_t i = 0; i < digits; i++) {
        buf[digits - 1 - i] = '0' + value % 10;
        value /= 10;
    }
    buf[digits] = '\0';

    oled_set_cursor(right_col + 1 - digits, line);
    oled_write(buf, false);
}

static void two_digits(uint8_t value, char *out) {
    out[0] = '0' + value / 10;
    out[1] = '0' + value % 10;
    out[2] = '\0';
}

// ┌───────────────────────────────────────────────────────────┐
// │ i n t r o                                                 │
// └───────────────────────────────────────────────────────────┘

static void intro_screen(uint32_t t) {
    oled_clear();

    if (t < 700) {
        // Memory dump: every line is a fresh line of random hex.
        for (uint8_t line = 0; line < 8; line++) {
            char    buf[21];
            uint8_t i = 0;
            for (uint8_t group = 0; group < 7; group++) {
                uint32_t r = rng();
                buf[i++]   = hex_digit[(r >> 4) & 0x0F];
                buf[i++]   = hex_digit[r & 0x0F];
                if (i < 20) {
                    buf[i++] = ' ';
                }
            }
            buf[i] = '\0';
            put(0, line, buf);
        }
        return;
    }

    if (t < 2100) {
        static const char *log_lines[] = {
            "KLOR BIOS 1.0",
            "CPU ATMEGA32U4 16MHZ",
            "RAM 2560B ROM 28672B",
            "MATRIX 8X6 SPLIT",
            "EEPROM KEYMAP OK",
            "USB HID LINK OK",
        };
        for (uint8_t line = 0; line < ARRAY_SIZE(log_lines); line++) {
            if (t >= 700 + line * 220) {
                put(0, line, log_lines[line]);
            }
        }
        return;
    }

    if (t < 2900) {
        uint8_t percent = (uint8_t)((t - 2100) * 100 / 800);
        char    bar[13];
        put(0, 2, "DECRYPT KEYSTORE");
        for (uint8_t i = 0; i < 12; i++) {
            bar[i] = ((i + 1) * 100 / 12) <= percent ? '#' : ' ';
        }
        bar[12] = '\0';
        put(1, 4, "[");
        put(2, 4, bar);
        put(14, 4, "] ");
        char pct[3];
        two_digits(percent, pct);
        put(17, 4, pct);
        put(19, 4, "%");
        return;
    }

    // ACCESS GRANTED, blinking
    if (((t - 2900) / 250) % 2 == 0) {
        put(3, 3, "ACCESS GRANTED");
    }
    put(0, 5, "> _");
}

// ┌───────────────────────────────────────────────────────────┐
// │ s t a t u s                                               │
// └───────────────────────────────────────────────────────────┘

// NULL for layers a Vial user added on the fly: the number is still shown.
static const char *layer_name(uint8_t layer) {
    switch (layer) {
        case 0:
            return "BASE";
        case 1:
            return "RAISE";
        case 2:
            return "LOWER";
        case 3:
            return "ADJUST";
    }
    return NULL;
}

static const char *layout_name(uint8_t option) {
    switch (option & 0x03) {
        case 1:
            return "KONRAD";
        case 2:
            return "YUBITSUME";
        case 3:
            return "SAEGEWERK";
    }
    return "POLYDACTYL";
}

static void status_screen(void) {
    uint8_t layer  = get_highest_layer(layer_state | default_layer_state);
    uint8_t mods   = get_mods();
    led_t   leds   = host_keyboard_led_state();
    uint8_t layout = via_get_layout_options();

    oled_clear();

    put(0, 0, "KLOR // ONLINE");
    put(20, 0, is_keyboard_master() ? "L" : "R");
    put(0, 1, "=====================");

    const char *name = layer_name(layer);
    put(0, 2, "LYR ");
    oled_write_char('0' + (layer % 10), false);
    oled_write(" ", false);
    if (name) {
        oled_write(name, false);
    }
    put(17, 2, keymap_config.swap_lctl_lgui ? "MAC" : "WIN");

    put(0, 3, "MODE ");
    oled_write(layout_name(layout), false);

    put_flag(0, 4, "CTRL ", mods & MOD_MASK_CTRL);
    put_flag(5, 4, "SHIFT ", mods & MOD_MASK_SHIFT);
    put_flag(11, 4, "ALT ", mods & MOD_MASK_ALT);
    put_flag(15, 4, "GUI", mods & MOD_MASK_GUI);

    put_flag(0, 5, "NUM ", leds.num_lock);
    put_flag(4, 5, "CAPS ", leds.caps_lock);
    put_flag(9, 5, "SCR", leds.scroll_lock);

    // get_current_wpm() is a uint8_t, so the value needs up to three digits.
    uint8_t wpm = get_current_wpm();
    put(0, 6, "WPM ");
    if (wpm) {
        put_num(6, 6, wpm);
    } else {
        put(5, 6, "--");
    }

    // Uptime, hh:mm:ss since power on.
    uint32_t secs = timer_read32() / 1000;
    char     up[9];
    two_digits((secs / 3600) % 100, up);
    up[2] = ':';
    two_digits((secs / 60) % 60, up + 3);
    up[5] = ':';
    two_digits(secs % 60, up + 6);
    up[8] = '\0';
    put(0, 7, "UP ");
    put(3, 7, up);
}

// ┌───────────────────────────────────────────────────────────┐
// │ e n t r y   p o i n t                                     │
// └───────────────────────────────────────────────────────────┘

bool oled_task_user(void) {
    // The left half is the master (MASTER_LEFT); keep the other display dark.
    if (!is_keyboard_master()) {
        oled_off();
        return false;
    }

    uint32_t now = timer_read32();
    if (!intro_shown) {
        intro_shown = true;
        intro_start = now;
    }

    if (now - intro_start < INTRO_LENGTH_MS) {
        if (timer_elapsed32(last_draw) < DRAW_TICK_MS) {
            return false;
        }
        last_draw = now;
        intro_screen(now - intro_start);
        return false;
    }

    if (timer_elapsed32(last_draw) < STATUS_TICK_MS) {
        return false;
    }
    last_draw = now;
    status_screen();
    return false;
}

#endif
