# Vial keymap for the KLOR (ATmega32U4 / Elite-C class controllers).
#
# The ATmega32U4 leaves 28 KiB of usable flash, so this build is trimmed to fit.
# OLED, encoder, haptic feedback, extrakey and the RGB matrix are pulled in via
# keyboard.json; VIAL_ENABLE adds the Vial core on top of that.
VIA_ENABLE = yes
VIAL_ENABLE = yes
VIAL_INSECURE = yes

# The controller on this board runs the Arduino/Caterina bootloader (it enumerates
# as 2341:0037), not the Atmel factory DFU one, so QK_BOOT has to use the
# Caterina magic key to stay in the bootloader. With the default atmel-dfu jump
# the bootloader only flashes past and boots the application again.
BOOTLOADER = caterina

# The status screen reports the typing speed.
WPM_ENABLE = yes


# The RGB matrix is off, so the keyboard definition ships with
# "lighting": "none" and Vial skips the VialRGB handshake that would otherwise
# abort the connection with "Unsupported VIALRGB protocol (0)".
VIALRGB_ENABLE = no

# Per-key RGB is disabled for now: the definition lights "none" and the WS2812
# driver is kept out of the way of the AVR bit-bang split serial. Enable both
# this and VIALRGB_ENABLE to get Vial's lighting tab back.
RGB_MATRIX_ENABLE = no

# Features that are not worth their flash cost on this board.
MOUSEKEY_ENABLE = no

# The DRV2605L haptic driver blocks on I2C for every keypress when the module is
# not populated, which shows up as laggy typing.
HAPTIC_ENABLE = no
BOOTMAGIC_ENABLE = no

# Vial modules disabled to save flash.
QMK_SETTINGS = no
TAP_DANCE_ENABLE = no
COMBO_ENABLE = no
KEY_OVERRIDE_ENABLE = no
CAPS_WORD_ENABLE = no
LAYER_LOCK_ENABLE = no
REPEAT_KEY_ENABLE = no

# QMK defaults that are dead weight here.
SPACE_CADET_ENABLE = no
GRAVE_ESC_ENABLE = no
MAGIC_ENABLE = no
CONSOLE_ENABLE = no
COMMAND_ENABLE = no
