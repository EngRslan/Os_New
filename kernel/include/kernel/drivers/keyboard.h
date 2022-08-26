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
//PAUSE
#define KEY_PAUSE           0x13
// CAPS LOCK
#define KEY_CAPITAL         0x14
#define KEY_CAPSLOCK        KEY_CAPSLOCK

#define KEY_ESCAPE          0x1B
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