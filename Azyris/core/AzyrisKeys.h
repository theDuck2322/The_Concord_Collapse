#pragma once

#include <SDL3/SDL_stdinc.h>

typedef enum AZ_Scancode
{
    AZ_UNKNOWN = 0,

    /**
     *  \name Usage page 0x07
     *
     *  These values are from usage page 0x07 (USB keyboard page).
     */
    /* @{ */

    AZ_A = 4,
    AZ_B = 5,
    AZ_C = 6,
    AZ_D = 7,
    AZ_E = 8,
    AZ_F = 9,
    AZ_G = 10,
    AZ_H = 11,
    AZ_I = 12,
    AZ_J = 13,
    AZ_K = 14,
    AZ_L = 15,
    AZ_M = 16,
    AZ_N = 17,
    AZ_O = 18,
    AZ_P = 19,
    AZ_Q = 20,
    AZ_R = 21,
    AZ_S = 22,
    AZ_T = 23,
    AZ_U = 24,
    AZ_V = 25,
    AZ_W = 26,
    AZ_X = 27,
    AZ_Y = 28,
    AZ_Z = 29,

    AZ_1 = 30,
    AZ_2 = 31,
    AZ_3 = 32,
    AZ_4 = 33,
    AZ_5 = 34,
    AZ_6 = 35,
    AZ_7 = 36,
    AZ_8 = 37,
    AZ_9 = 38,
    AZ_0 = 39,

    AZ_RETURN = 40,
    AZ_ESCAPE = 41,
    AZ_BACKSPACE = 42,
    AZ_TAB = 43,
    AZ_SPACE = 44,

    AZ_MINUS = 45,
    AZ_EQUALS = 46,
    AZ_LEFTBRACKET = 47,
    AZ_RIGHTBRACKET = 48,
    AZ_BACKSLASH = 49, /**< Located at the lower left of the return
                        *   key on ISO keyboards and at the right end
                        *   of the QWERTY row on ANSI keyboards.
                        *   Produces REVERSE SOLIDUS (backslash) and
                        *   VERTICAL LINE in a US layout, REVERSE
                        *   SOLIDUS and VERTICAL LINE in a UK Mac
                        *   layout, NUMBER SIGN and TILDE in a UK
                        *   Windows layout, DOLLAR SIGN and POUND SIGN
                        *   in a Swiss German layout, NUMBER SIGN and
                        *   APOSTROPHE in a German layout, GRAVE
                        *   ACCENT and POUND SIGN in a French Mac
                        *   layout, and ASTERISK and MICRO SIGN in a
                        *   French Windows layout.
                        */
    AZ_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                        *   instead of 49 for the same key, but all
                        *   OSes I've seen treat the two codes
                        *   identically. So, as an implementor, unless
                        *   your keyboard generates both of those
                        *   codes and your OS treats them differently,
                        *   you should generate SDL_SCANCODE_BACKSLASH
                        *   instead of this code. As a user, you
                        *   should not rely on this code because SDL
                        *   will never generate it with most (all?)
                        *   keyboards.
                        */
    AZ_SEMICOLON = 51,
    AZ_APOSTROPHE = 52,
    AZ_GRAVE = 53, /**< Located in the top left corner (on both ANSI
                    *   and ISO keyboards). Produces GRAVE ACCENT and
                    *   TILDE in a US Windows layout and in US and UK
                    *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                    *   and NOT SIGN in a UK Windows layout, SECTION
                    *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                    *   layouts on ISO keyboards, SECTION SIGN and
                    *   DEGREE SIGN in a Swiss German layout (Mac:
                    *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                    *   DEGREE SIGN in a German layout (Mac: only on
                    *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                    *   French Windows layout, COMMERCIAL AT and
                    *   NUMBER SIGN in a French Mac layout on ISO
                    *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                    *   SIGN in a Swiss German, German, or French Mac
                    *   layout on ANSI keyboards.
                    */
    AZ_COMMA = 54,
    AZ_PERIOD = 55,
    AZ_SLASH = 56,

    AZ_CAPSLOCK = 57,

    AZ_F1 = 58,
    AZ_F2 = 59,
    AZ_F3 = 60,
    AZ_F4 = 61,
    AZ_F5 = 62,
    AZ_F6 = 63,
    AZ_F7 = 64,
    AZ_F8 = 65,
    AZ_F9 = 66,
    AZ_F10 = 67,
    AZ_F11 = 68,
    AZ_F12 = 69,

    AZ_PRINTSCREEN = 70,
    AZ_SCROLLLOCK = 71,
    AZ_PAUSE = 72,
    AZ_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                   does send code 73, not 117) */
    AZ_HOME = 74,
    AZ_PAGEUP = 75,
    AZ_DELETE = 76,
    AZ_END = 77,
    AZ_PAGEDOWN = 78,
    AZ_RIGHT = 79,
    AZ_LEFT = 80,
    AZ_DOWN = 81,
    AZ_UP = 82,

    AZ_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                           */
    AZ_KP_DIVIDE = 84,
    AZ_KP_MULTIPLY = 85,
    AZ_KP_MINUS = 86,
    AZ_KP_PLUS = 87,
    AZ_KP_ENTER = 88,
    AZ_KP_1 = 89,
    AZ_KP_2 = 90,
    AZ_KP_3 = 91,
    AZ_KP_4 = 92,
    AZ_KP_5 = 93,
    AZ_KP_6 = 94,
    AZ_KP_7 = 95,
    AZ_KP_8 = 96,
    AZ_KP_9 = 97,
    AZ_KP_0 = 98,
    AZ_KP_PERIOD = 99,

    AZ_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                              *   keyboards have over ANSI ones,
                              *   located between left shift and Z.
                              *   Produces GRAVE ACCENT and TILDE in a
                              *   US or UK Mac layout, REVERSE SOLIDUS
                              *   (backslash) and VERTICAL LINE in a
                              *   US or UK Windows layout, and
                              *   LESS-THAN SIGN and GREATER-THAN SIGN
                              *   in a Swiss German, German, or French
                              *   layout. */
    AZ_APPLICATION = 101,    /**< windows contextual menu, compose */
    AZ_POWER = 102,          /**< The USB document says this is a status flag,
                              *   not a physical key - but some Mac keyboards
                              *   do have a power key. */
    AZ_KP_EQUALS = 103,
    AZ_F13 = 104,
    AZ_F14 = 105,
    AZ_F15 = 106,
    AZ_F16 = 107,
    AZ_F17 = 108,
    AZ_F18 = 109,
    AZ_F19 = 110,
    AZ_F20 = 111,
    AZ_F21 = 112,
    AZ_F22 = 113,
    AZ_F23 = 114,
    AZ_F24 = 115,
    AZ_EXECUTE = 116,
    AZ_HELP = 117, /**< AL Integrated Help Center */
    AZ_MENU = 118, /**< Menu (show menu) */
    AZ_SELECT = 119,
    AZ_STOP = 120,  /**< AC Stop */
    AZ_AGAIN = 121, /**< AC Redo/Repeat */
    AZ_UNDO = 122,  /**< AC Undo */
    AZ_CUT = 123,   /**< AC Cut */
    AZ_COPY = 124,  /**< AC Copy */
    AZ_PASTE = 125, /**< AC Paste */
    AZ_FIND = 126,  /**< AC Find */
    AZ_MUTE = 127,
    AZ_VOLUMEUP = 128,
    AZ_VOLUMEDOWN = 129,
    /* not sure whether there's a reason to enable these */
    /*     SDL_SCANCODE_LOCKINGCAPSLOCK = 130,  */
    /*     SDL_SCANCODE_LOCKINGNUMLOCK = 131, */
    /*     SDL_SCANCODE_LOCKINGSCROLLLOCK = 132, */
    AZ_KP_COMMA = 133,
    AZ_KP_EQUALSAS400 = 134,

    AZ_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                            footnotes in USB doc */
    AZ_INTERNATIONAL2 = 136,
    AZ_INTERNATIONAL3 = 137, /**< Yen */
    AZ_INTERNATIONAL4 = 138,
    AZ_INTERNATIONAL5 = 139,
    AZ_INTERNATIONAL6 = 140,
    AZ_INTERNATIONAL7 = 141,
    AZ_INTERNATIONAL8 = 142,
    AZ_INTERNATIONAL9 = 143,
    AZ_LANG1 = 144, /**< Hangul/English toggle */
    AZ_LANG2 = 145, /**< Hanja conversion */
    AZ_LANG3 = 146, /**< Katakana */
    AZ_LANG4 = 147, /**< Hiragana */
    AZ_LANG5 = 148, /**< Zenkaku/Hankaku */
    AZ_LANG6 = 149, /**< reserved */
    AZ_LANG7 = 150, /**< reserved */
    AZ_LANG8 = 151, /**< reserved */
    AZ_LANG9 = 152, /**< reserved */

    AZ_ALTERASE = 153, /**< Erase-Eaze */
    AZ_SYSREQ = 154,
    AZ_CANCEL = 155, /**< AC Cancel */
    AZ_CLEAR = 156,
    AZ_PRIOR = 157,
    AZ_RETURN2 = 158,
    AZ_SEPARATOR = 159,
    AZ_OUT = 160,
    AZ_OPER = 161,
    AZ_CLEARAGAIN = 162,
    AZ_CRSEL = 163,
    AZ_EXSEL = 164,

    AZ_KP_00 = 176,
    AZ_KP_000 = 177,
    AZ_THOUSANDSSEPARATOR = 178,
    AZ_DECIMALSEPARATOR = 179,
    AZ_CURRENCYUNIT = 180,
    AZ_CURRENCYSUBUNIT = 181,
    AZ_KP_LEFTPAREN = 182,
    AZ_KP_RIGHTPAREN = 183,
    AZ_KP_LEFTBRACE = 184,
    AZ_KP_RIGHTBRACE = 185,
    AZ_KP_TAB = 186,
    AZ_KP_BACKSPACE = 187,
    AZ_KP_A = 188,
    AZ_KP_B = 189,
    AZ_KP_C = 190,
    AZ_KP_D = 191,
    AZ_KP_E = 192,
    AZ_KP_F = 193,
    AZ_KP_XOR = 194,
    AZ_KP_POWER = 195,
    AZ_KP_PERCENT = 196,
    AZ_KP_LESS = 197,
    AZ_KP_GREATER = 198,
    AZ_KP_AMPERSAND = 199,
    AZ_KP_DBLAMPERSAND = 200,
    AZ_KP_VERTICALBAR = 201,
    AZ_KP_DBLVERTICALBAR = 202,
    AZ_KP_COLON = 203,
    AZ_KP_HASH = 204,
    AZ_KP_SPACE = 205,
    AZ_KP_AT = 206,
    AZ_KP_EXCLAM = 207,
    AZ_KP_MEMSTORE = 208,
    AZ_KP_MEMRECALL = 209,
    AZ_KP_MEMCLEAR = 210,
    AZ_KP_MEMADD = 211,
    AZ_KP_MEMSUBTRACT = 212,
    AZ_KP_MEMMULTIPLY = 213,
    AZ_KP_MEMDIVIDE = 214,
    AZ_KP_PLUSMINUS = 215,
    AZ_KP_CLEAR = 216,
    AZ_KP_CLEARENTRY = 217,
    AZ_KP_BINARY = 218,
    AZ_KP_OCTAL = 219,
    AZ_KP_DECIMAL = 220,
    AZ_KP_HEXADECIMAL = 221,

    AZ_LCTRL = 224,
    AZ_LSHIFT = 225,
    AZ_LALT = 226, /**< alt, option */
    AZ_LGUI = 227, /**< windows, command (apple), meta */
    AZ_RCTRL = 228,
    AZ_RSHIFT = 229,
    AZ_RALT = 230, /**< alt gr, option */
    AZ_RGUI = 231, /**< windows, command (apple), meta */

    AZ_MODE = 257, /**< I'm not sure if this is really not covered
                    *   by any of the above, but since there's a
                    *   special SDL_KMOD_MODE for it I'm adding it here
                    */

    /* @} */ /* Usage page 0x07 */

    /**
     *  \name Usage page 0x0C
     *
     *  These values are mapped from usage page 0x0C (USB consumer page).
     *
     *  There are way more keys in the spec than we can represent in the
     *  current scancode range, so pick the ones that commonly come up in
     *  real world usage.
     */
    /* @{ */

    AZ_SLEEP = 258, /**< Sleep */
    AZ_WAKE = 259,  /**< Wake */

    AZ_CHANNEL_INCREMENT = 260, /**< Channel Increment */
    AZ_CHANNEL_DECREMENT = 261, /**< Channel Decrement */

    AZ_MEDIA_PLAY = 262,           /**< Play */
    AZ_MEDIA_PAUSE = 263,          /**< Pause */
    AZ_MEDIA_RECORD = 264,         /**< Record */
    AZ_MEDIA_FAST_FORWARD = 265,   /**< Fast Forward */
    AZ_MEDIA_REWIND = 266,         /**< Rewind */
    AZ_MEDIA_NEXT_TRACK = 267,     /**< Next Track */
    AZ_MEDIA_PREVIOUS_TRACK = 268, /**< Previous Track */
    AZ_MEDIA_STOP = 269,           /**< Stop */
    AZ_MEDIA_EJECT = 270,          /**< Eject */
    AZ_MEDIA_PLAY_PAUSE = 271,     /**< Play / Pause */
    AZ_MEDIA_SELECT = 272,         /* Media Select */

    AZ_AC_NEW = 273,        /**< AC New */
    AZ_AC_OPEN = 274,       /**< AC Open */
    AZ_AC_CLOSE = 275,      /**< AC Close */
    AZ_AC_EXIT = 276,       /**< AC Exit */
    AZ_AC_SAVE = 277,       /**< AC Save */
    AZ_AC_PRINT = 278,      /**< AC Print */
    AZ_AC_PROPERTIES = 279, /**< AC Properties */

    AZ_AC_SEARCH = 280,    /**< AC Search */
    AZ_AC_HOME = 281,      /**< AC Home */
    AZ_AC_BACK = 282,      /**< AC Back */
    AZ_AC_FORWARD = 283,   /**< AC Forward */
    AZ_AC_STOP = 284,      /**< AC Stop */
    AZ_AC_REFRESH = 285,   /**< AC Refresh */
    AZ_AC_BOOKMARKS = 286, /**< AC Bookmarks */

    /* @} */ /* Usage page 0x0C */

    /**
     *  \name Mobile keys
     *
     *  These are values that are often used on mobile phones.
     */
    /* @{ */

    AZ_SOFTLEFT = 287,  /**< Usually situated below the display on phones and
                                       used as a multi-function feature key for selecting
                                       a software defined function shown on the bottom left
                                       of the display. */
    AZ_SOFTRIGHT = 288, /**< Usually situated below the display on phones and
                                       used as a multi-function feature key for selecting
                                       a software defined function shown on the bottom right
                                       of the display. */
    AZ_CALL = 289,      /**< Used for accepting phone calls. */
    AZ_ENDCALL = 290,   /**< Used for rejecting phone calls. */

    /* @} */ /* Mobile keys */

    /* Add any other keys here. */

    AZ_RESERVED = 400, /**< 400-500 reserved for dynamic keycodes */

    AZ_COUNT = 512 /**< not a key, just marks the number of scancodes for array bounds */

} AZ_Scancode;

typedef enum AZ_GamepadButton
{
    AZ_GPAD_BUTTON_INVALID = -1,
    AZ_GPAD_BUTTON_SOUTH, /**< Bottom face button (e.g. Xbox A button) */
    AZ_GPAD_BUTTON_EAST,  /**< Right face button (e.g. Xbox B button) */
    AZ_GPAD_BUTTON_WEST,  /**< Left face button (e.g. Xbox X button) */
    AZ_GPAD_BUTTON_NORTH, /**< Top face button (e.g. Xbox Y button) */
    AZ_GPAD_BUTTON_BACK,
    AZ_GPAD_BUTTON_GUIDE,
    AZ_GPAD_BUTTON_START,
    AZ_GPAD_BUTTON_LEFT_STICK,
    AZ_GPAD_BUTTON_RIGHT_STICK,
    AZ_GPAD_BUTTON_LEFT_SHOULDER,
    AZ_GPAD_BUTTON_RIGHT_SHOULDER,
    AZ_GPAD_BUTTON_DPAD_UP,
    AZ_GPAD_BUTTON_DPAD_DOWN,
    AZ_GPAD_BUTTON_DPAD_LEFT,
    AZ_GPAD_BUTTON_DPAD_RIGHT,
    AZ_GPAD_BUTTON_MISC1,         /**< Additional button (e.g. Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button, Google Stadia capture button) */
    AZ_GPAD_BUTTON_RIGHT_PADDLE1, /**< Upper or primary paddle, under your right hand (e.g. Xbox Elite paddle P1) */
    AZ_GPAD_BUTTON_LEFT_PADDLE1,  /**< Upper or primary paddle, under your left hand (e.g. Xbox Elite paddle P3) */
    AZ_GPAD_BUTTON_RIGHT_PADDLE2, /**< Lower or secondary paddle, under your right hand (e.g. Xbox Elite paddle P2) */
    AZ_GPAD_BUTTON_LEFT_PADDLE2,  /**< Lower or secondary paddle, under your left hand (e.g. Xbox Elite paddle P4) */
    AZ_GPAD_BUTTON_TOUCHPAD,      /**< PS4/PS5 touchpad button */
    AZ_GPAD_BUTTON_MISC2,         /**< Additional button */
    AZ_GPAD_BUTTON_MISC3,         /**< Additional button */
    AZ_GPAD_BUTTON_MISC4,         /**< Additional button */
    AZ_GPAD_BUTTON_MISC5,         /**< Additional button */
    AZ_GPAD_BUTTON_MISC6,         /**< Additional button */
    AZ_GPAD_BUTTON_COUNT
} AZ_GamepadButton;

typedef enum AZ_GamepadAxis
{
    AZ_GPAD_AXIS_INVALID = -1,
    AZ_GPAD_AXIS_LEFTX,
    AZ_GPAD_AXIS_LEFTY,
    AZ_GPAD_AXIS_RIGHTX,
    AZ_GPAD_AXIS_RIGHTY,
    AZ_GPAD_AXIS_LEFT_TRIGGER,
    AZ_GPAD_AXIS_RIGHT_TRIGGER,
    AZ_GPAD_AXIS_COUNT
} AZ_GamepadAxis;