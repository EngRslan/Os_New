#include <kernel/types.h>
#include <kernel/isr.h>
#include <stddef.h>
#include <ctype.h>
#include <kernel/drivers/keyboard.h>
#define KEY_CODE_CTRL_LEFT      0x1D
#define KEY_CODE_SHIFT_LEFT     0x2A
#define KEY_CODE_SHIFT_RIGHT    0x36
#define KEY_CODE_ALT_LEFT       0x38
#define KEY_CODE_CAPS_LOCK      0x3A
#define KEY_CODE_NUM_LOCK       0x45
#define KEY_CODE_SCROLL_LOCK    0x46

keyboard_event_handler_t key_down_handler = NULL;
keyboard_event_handler_t key_up_handler = NULL;

struct
{
    uint8_t alt;
    uint8_t ctrl;
    uint8_t shift;
    uint8_t caps_lock;
    uint8_t scroll_lock;
    uint8_t number_lock;
} scan_status = {0,0,0,0,0,0};

uint8_t qwerty_scan_table[128] = {
    0, // note used
    0x1B, // Escape
    '1', '2', '3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', // Enter Key
    0, // Left CTRL
    'a','s','d','f','g','h','j','k','l',';','\'','`',
    0, // LEFT SHIFT
    '\\','z','x','c','v','b','n','m',',','.','/',
    0, // Right Shift
    '*', // Keypad Astrisk
    0, // Left ALt
    ' ',
    0, // Caps Lock
    0/* F1 */,0/* F2 */,0/* F3 */,0/* F4 */,0/* F5 */,0/* F6 */,0/* F7 */,0/* F8 */,0/* F9 */,0/* F10 */, 
    0/* Num Lock */,0/* scroll Lock */,
    '7','8','9','-',        //-------
    '4','5','6','+',        //      | Num PAD
    '1','2','3'             //      |
    ,'0','.',               //-------
    0,0,
    0 /* F11 */,0 /* F12 */,
};

uint8_t is_control_code(int32_t scancode){
    switch (scancode)
    {
        case KEY_CODE_CTRL_LEFT:
        case KEY_CODE_ALT_LEFT:
        case KEY_CODE_SHIFT_LEFT:
        case KEY_CODE_SHIFT_RIGHT:
            return 1;
        default:

            return 0;
    }   
}
void set_function_key(uint32_t scancode,uint8_t value){
    switch (scancode)
    {
        case KEY_CODE_CTRL_LEFT:
            scan_status.ctrl = value;
            return;
        case KEY_CODE_ALT_LEFT:
            scan_status.alt = value;
            return;
        case KEY_CODE_SHIFT_LEFT:
        case KEY_CODE_SHIFT_RIGHT:
            scan_status.shift = value;
            return;
    }   
}
uint8_t is_toggle_key(uint32_t scancode){
    switch (scancode)
    {
        case KEY_CODE_CAPS_LOCK:
        case KEY_CODE_NUM_LOCK:
        case KEY_CODE_SCROLL_LOCK:
            return 1;
        default:

            return 0;
    }   
}
void set_toogle_key(uint32_t scancode,uint8_t value){
    switch (scancode)
    {
        case KEY_CODE_CAPS_LOCK:
            scan_status.caps_lock = value;
            return;
        case KEY_CODE_NUM_LOCK:
            scan_status.number_lock = value;
            return;
        case KEY_CODE_SCROLL_LOCK:
        scan_status.scroll_lock = value;
            return;
    }   
}
void map_and_fire(uint32_t scancode,keyboard_event_handler_t _event_handler){
    char ascii_char = qwerty_scan_table[scancode];
    if((scan_status.shift || scan_status.caps_lock) && isalpha(ascii_char)){
        ascii_char = toupper(ascii_char);
    }

    keyboard_event_t keyboard_event;
    keyboard_event.ascii = ascii_char;
    keyboard_event.alt = scan_status.alt;
    keyboard_event.ctrl = scan_status.ctrl;
    keyboard_event.alt = scan_status.alt;
    keyboard_event.shift = scan_status.shift;
    keyboard_event.key_code = ascii_char;
    if(_event_handler){
        _event_handler(keyboard_event);
    }
}
void handle_scan_code(int32_t scancode){
    if(scancode & 0x80) {
        if(is_control_code(scancode & 0x7F)){
            set_function_key(scancode & 0x7F,0);
            return;
        }
        if(is_toggle_key(scancode & 0x7F)){
            set_toogle_key(scancode & 0x7F,scan_status.caps_lock?0:1);
            return;
        }
        
        map_and_fire(scancode & 0x7F,key_up_handler);
    }
    else 
    {
        if(is_control_code(scancode)){
            set_function_key(scancode,1);
            return;
        }
        map_and_fire(scancode,key_down_handler);
    }

}
void keyboard_handler(register_t * r){
    int32_t i, scancode;
  

    for(i = 1000; i > 0; i++) { 
        // Check if scan code is ready
        if((inportb(0x64) & 1) == 0) continue;
        // Get the scan code
        scancode = inportb(0x60);
        break;
    }
    if(i > 0) {
        handle_scan_code(scancode);
    }
}
void keyboard_install(){
    register_interrupt_handler(IRQ_BASE + 1,keyboard_handler);
}

void register_keyboard_event_handler(keyboard_event_handler_t _handler,uint8_t event_type){
    if(event_type == KEY_UP_EVENT)
        key_up_handler = _handler;
    else if(event_type == KEY_DOWN_EVENT)
        key_down_handler = _handler;
}