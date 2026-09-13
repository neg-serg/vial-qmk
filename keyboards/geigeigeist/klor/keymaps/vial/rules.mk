# Vial keymap for the KLOR (ATmega32U4 / Elite-C class controllers).
#
# Optional Vial features are trimmed to keep the build inside the flash budget:
# the keyboard already pulls in OLED, encoder, RGB matrix, haptic feedback and
# extrakey through keyboard.json, and VIAL_ENABLE adds the Vial core.
VIA_ENABLE = yes
VIAL_ENABLE = yes
VIAL_INSECURE = yes

QMK_SETTINGS = no
TAP_DANCE_ENABLE = no
COMBO_ENABLE = no
KEY_OVERRIDE_ENABLE = no
CAPS_WORD_ENABLE = no
LAYER_LOCK_ENABLE = no
REPEAT_KEY_ENABLE = no
