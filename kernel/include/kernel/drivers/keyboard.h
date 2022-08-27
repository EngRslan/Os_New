#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <kernel/types.h>

#define KEY_UP_EVENT        0x0
#define KEY_DOWN_EVENT      0x1


#define KEY_NONE            0x00
#define KEY_LBUTTON         0x01
#define KEY_RBUTTON         0x02
#define KEY_CANCEL          0x03
#define KEY_MBUTTON         0x04
#define KEY_XBUTTON1        0x05
#define KEY_XBUTTON2        0x06
// Backspace
#define KEY_BACK            0x08
// TAB
#define KEY_TAB             0x09
//Line Feed
#define KEY_LINEFEED        0x0A
//Form Feed
#define KEY_CLEAR           0x0C
//Return Key
#define KEY_RETURN          0x0D
//Enter Key
#define KEY_ENTER           KEY_RETURN
// Shift Key
#define KEY_SHIFT           0x10
//Control Key
#define KEY_CONTROL         0x11
//ALT KEY
#define KEY_MENU            0x12
#define KEY_ALT             0x12
//PAUSE
#define KEY_PAUSE           0x13
// CAPS LOCK
#define KEY_CAPSLOCK        0x14

#define KEY_ESCAPE          0x1B
#define KEY_IME_CONVERT     0x1C
#define KEY_IME_NONCONVERT  0x1D
#define KEY_IME_ACCEPT      0x1E
#define KEY_IME_MODE_CHANGE 0x1F
#define KEY_SPACE           0x20
// Page UP
#define KEY_PRIOR           0x21
#define KEY_PAGEUP          KEY_PRIOR
// PAGE DOWN
#define KEY_NEXT            0x22
#define KEY_PAGEDOWN        KEY_NEXT
#define KEY_END             0x23
#define KEY_HOME            0x24
#define KEY_LEFT            0x25
#define KEY_UP              0x26
#define KEY_RIGHT           0x27
#define KEY_DOWN            0x28
#define KEY_SELECT          0x29
#define KEY_PRINT           0x2A
#define KEY_EXECUTE         0x2B
#define KEY_SNAPSHOT        0x2C
#define KEY_PRNINTSCREEN    KEY_SNAPSHOT
#define KEY_INSERT          0x2D
#define KEY_DELETE          0x2E
#define KEY_HELP            0x2F
#define KEY_D0              0x30
#define KEY_D1              0x31
#define KEY_D2              0x32
#define KEY_D3              0x33
#define KEY_D4              0x34
#define KEY_D5              0x35
#define KEY_D6              0x36
#define KEY_D7              0x37
#define KEY_D8              0x38
#define KEY_D9              0x39
#define KEY_A               0x41
#define KEY_B               0x42
#define KEY_C               0x43
#define KEY_D               0x44
#define KEY_E               0x45
#define KEY_F               0x46
#define KEY_G               0x47
#define KEY_H               0x48
#define KEY_I               0x49
#define KEY_J               0x4A
#define KEY_K               0x4B
#define KEY_L               0x4C
#define KEY_M               0x4D
#define KEY_N               0x4E
#define KEY_O               0x4F
#define KEY_P               0x50
#define KEY_Q               0x51
#define KEY_R               0x52
#define KEY_S               0x53
#define KEY_T               0x54
#define KEY_U               0x55
#define KEY_V               0x56
#define KEY_W               0x57
#define KEY_X               0x58
#define KEY_Y               0x59
#define KEY_Z               0x5A
#define KEY_LWIN            0x5B
#define KEY_RWIN            0x5c
#define KEY_APPS            0x5D
#define KEY_NUMPAD0         0x60
#define KEY_NUMPAD1         0x61
#define KEY_NUMPAD2         0x62
#define KEY_NUMPAD3         0x63
#define KEY_NUMPAD4         0x64
#define KEY_NUMPAD5         0x65
#define KEY_NUMPAD6         0x66
#define KEY_NUMPAD7         0x67
#define KEY_NUMPAD8         0x68
#define KEY_NUMPAD9         0x69
#define KEY_MULTIPLY        0x6A
#define KEY_ADD             0x6B
#define KEY_SEPARATOR       0x6c
#define KEY_SUBSTRACT       0x6D
#define KEY_DECIMAL         0x6E
#define KEY_DIVIDE          0x6F
#define KEY_F1              0x70
#define KEY_F2              0x71
#define KEY_F3              0x72
#define KEY_F4              0x73
#define KEY_F5              0x74
#define KEY_F6              0x75
#define KEY_F7              0x76
#define KEY_F8              0x77
#define KEY_F9              0x78
#define KEY_F10             0x79
#define KEY_F11             0x7A
#define KEY_F12             0x7B
#define KEY_F13             0x7C
#define KEY_F14             0x7D
#define KEY_F15             0x7E
#define KEY_F16             0x7F
#define KEY_F17             0x80
#define KEY_F18             0x81
#define KEY_F19             0x82
#define KEY_F20             0x83
#define KEY_F21             0x84
#define KEY_F22             0x85
#define KEY_F23             0x86
#define KEY_F24             0x87
#define KEY_NUMLOCK         0x90
#define KEY_SCROLL          0x91
#define KEY_LSHIFT          0xA0
#define KEY_RSHIFT          0xA1
#define KEY_LCTRL           0xA2
#define KEY_RCTRL           0xA3
#define KEY_LMENU           0xA4
#define KEY_RMENU           0xA5
#define KEY_BROWSER_BACK    0xA6
#define KEY_BROWSER_FORWARD 0xA7
#define KEY_BROWSER_REFRESH 0xA8
#define KEY_BROWSER_STOP    0xA9
#define KEY_BROWSER_SEARCH  0xAA
#define KEY_BROWSER_FAVORITES  0xAB
#define KEY_BROWSER_HOME    0xAC
#define KEY_VOLUME_MUTE    0xAD
#define KEY_VOLUME_DOWN    0xAE
#define KEY_VOLUME_UP      0xAF
#define KEY_MEDIA_NEXT_TRACK 0xB0
#define KEY_MEDIA_PREVIOUS_TRACK 0xB1
#define KEY_MEDIA_STOP      0xB2
#define KEY_MEDIA_PLAYPAUSE 0xB3
#define KEY_LAUNCH_EMAIL    0xB4
#define KEY_SELECT_MEDIA    0xB5
#define KEY_LAUNCH_APP1     0xB6
#define KEY_LAUNCH_APP2     0xB7
#define KEY_OEM_SEMICOLON   0xBA
#define KEY_OEM_PLUS        0xBB
#define KEY_OEM_COMMA       0xBC
#define KEY_OEM_MINUS       0xBD
#define KEY_OEM_PERIOD      0xBE
#define KEY_OEM_QUESTION    0xBF
#define KEY_OEM_TILDE       0xC0
#define KEY_OEM_OPEN_BRACKETS 0xDB
#define KEY_OEM_PIPE        0xDC
#define KEY_OEM_CLOSE_BRACKETS 0xDE
#define KEY_OEM_QUOTES      0xDF
#define KEY_OEM_BACKSLASH   0xE2
#define KEY_OEM_PROCESS     0xE5
#define KEY_ATTN            0xF6
#define KEY_CRSEL           0xF7
#define KEY_EXSEL           0xF8
#define KEY_ERASE_EOF       0xF9
#define KEY_PLAY            0xFA
#define KEY_ZOOM            0xFB
#define KEY_NONAME          0xFC
#define KEY_PA1             0xFD
#define KEY_OEM_CLEAR       0xFE
#define KEY_CODE            0x0000FFFF
#define SHIFT               0x00010000
#define CONTROL             0x00020000
#define ALT                 0x00040000
#define MODIFIERS           0xFFFF0000
#define IMEAccept           0x1E
#define KEY_OEM1            0xBA
#define KEY_OEM102          0xE2
#define KEY_OEM2            0xBF
#define KEY_OEM3            0xC0
#define KEY_OEM4            0xDB
#define KEY_OEM5            0xDC
#define KEY_OEM6            0xDD
#define KEY_OEM7            0xDE
#define KEY_PACKET          0xE7
#define KEY_SLEEP           0x5F


typedef struct keyboard_event
{
    char ascii;
    uint16_t key_code;
    uint8_t alt;
    uint8_t ctrl;
    uint8_t shift;

} keyboard_event_t;

typedef void (* keyboard_event_handler_t)(keyboard_event_t);

void keyboard_install();
void register_keyboard_event_handler(keyboard_event_handler_t _handler,uint8_t event_type);
#endif