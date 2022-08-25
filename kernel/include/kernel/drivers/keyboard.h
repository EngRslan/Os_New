#ifndef KEYBOARD_H
#define KEYBOARD_H
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
#define KEY_    0x0
#define KEY_    0x0
#define KEY_    0x0
#define KEY_    0x0
#define KEY_    0x0
#define KEY_    0x0
#define KEY_    0x0
#define KEY_    0x0
#define KEY_    0x0
#define KEY_    0x0
#define KEY_    0x0
#define KEY_    0x0
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
void register_keyboard_event_handler(keyboard_event_handler_t);
#endif