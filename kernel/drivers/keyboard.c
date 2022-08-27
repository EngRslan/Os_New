#include <kernel/types.h>
#include <kernel/isr.h>
#include <stddef.h>
#include <ctype.h>
#include <logger.h>
#include <kernel/drivers/keyboard.h>
#define KEY_PRESSED     0x1
#define KEY_RELEASED    0x2

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
    uint8_t winkey;
    uint8_t e0;
    uint8_t type;
} scan_status = {0,0,0,0,0,0,0,0,0};

uint8_t qwerty_scan_table[128] = {
    0, 
    KEY_ESCAPE, KEY_D1, KEY_D2, KEY_D3, KEY_D4, KEY_D5, KEY_D6, KEY_D7, KEY_D8, KEY_D9, KEY_D0,KEY_OEM_MINUS,KEY_OEM_PLUS,KEY_BACK,
    KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_OEM_OPEN_BRACKETS, KEY_OEM_CLOSE_BRACKETS, KEY_ENTER,
    KEY_LCTRL, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_OEM_SEMICOLON, KEY_OEM_QUOTES, KEY_OEM_TILDE,
    KEY_LSHIFT,KEY_OEM_PIPE,KEY_Z,KEY_X,KEY_C,KEY_V,KEY_B,KEY_N,KEY_M,KEY_OEM_COMMA,KEY_OEM_PERIOD,KEY_OEM_BACKSLASH, KEY_RSHIFT,
    KEY_MULTIPLY,
    KEY_LMENU/*LALT*/,
    KEY_SPACE,
    KEY_CAPSLOCK,
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, 
    KEY_NUMLOCK,KEY_SCROLL,
    KEY_NUMPAD7,KEY_NUMPAD8,KEY_NUMPAD9,KEY_SUBSTRACT,
    KEY_NUMPAD4,KEY_NUMPAD5,KEY_NUMPAD6,KEY_ADD,
    KEY_NUMPAD1,KEY_NUMPAD2,KEY_NUMPAD3,
    KEY_NUMPAD0,KEY_DECIMAL,
    0,0,
    KEY_F11,KEY_F12
};
uint8_t qwerty_scan_table_E0[128] = {
    [0x10]=KEY_MEDIA_PREVIOUS_TRACK,
    [0x19]=KEY_MEDIA_NEXT_TRACK,
    [0x1C]=KEY_ENTER,
    [0x1D]=KEY_RCTRL,
    [0x20]=KEY_VOLUME_MUTE,
    [0x21]=KEY_LAUNCH_APP2,
    [0x22]=KEY_MEDIA_PLAYPAUSE,
    [0x2E]=KEY_VOLUME_DOWN,
    [0x30]=KEY_VOLUME_UP,
    [0x32]=KEY_BROWSER_HOME,
    [0x35]=KEY_DIVIDE,
    [0x38]=KEY_ALT,
    [0x47]=KEY_HOME,
    [0x48]=KEY_UP,
    [0x49]=KEY_PAGEUP,
    [0x4B]=KEY_LEFT,
    [0x4D]=KEY_RIGHT,
    [0x4F]=KEY_END,
    [0x50]=KEY_DOWN,
    [0x51]=KEY_PAGEDOWN,
    [0x52]=KEY_INSERT,
    [0x53]=KEY_DELETE,
    [0x5B]=KEY_LWIN,
    [0x5C]=KEY_RWIN,
    [0x5D]=KEY_APPS,
    [0x5E]=0,//(ACPI) power pressed,
    [0x5F]=KEY_SLEEP,
    [0x63]=0,//(ACPI) wake pressed
    [0x65]=KEY_BROWSER_SEARCH,
    [0x66]=KEY_BROWSER_FAVORITES,
    [0x67]=KEY_BROWSER_REFRESH,
    [0x68]=KEY_BROWSER_STOP,
    [0x69]=KEY_BROWSER_FORWARD,
    [0x6A]=KEY_BROWSER_BACK,
    [0x6B]=0,//(multimedia) my computer pressed
    [0x6C]=KEY_LAUNCH_EMAIL,
    [0x6D]=KEY_SELECT_MEDIA
} ;
struct {
 char normal;
 char shift;
 char caps;
 char numlock;   
} ASCII_KEY_MAP[] = {
    [KEY_ESCAPE]                = {0x1b,0   ,0   ,0   },
    [KEY_OEM_TILDE]             = {'`' ,'~' ,0   ,0   },
    [KEY_D1]                    = {'1' ,'!' ,0   ,0   },
    [KEY_D2]                    = {'2' ,'@' ,0   ,0   },
    [KEY_D3]                    = {'3' ,'#' ,0   ,0   },
    [KEY_D4]                    = {'4' ,'$' ,0   ,0   },
    [KEY_D5]                    = {'5' ,'%' ,0   ,0   },
    [KEY_D6]                    = {'6' ,'^' ,0   ,0   },
    [KEY_D7]                    = {'7' ,'&' ,0   ,0   },
    [KEY_D8]                    = {'8' ,'*' ,0   ,0   },
    [KEY_D9]                    = {'9' ,'(' ,0   ,0   },
    [KEY_D0]                    = {'0' ,')' ,0   ,0   },
    [KEY_OEM_MINUS]             = {'-' ,'_' ,0   ,0   },
    [KEY_OEM_PLUS]              = {'=' ,'+' ,0   ,0   },
    [KEY_BACK]                  = {'\b',0   ,0   ,0   },
    [KEY_TAB]                   = {'\t',0   ,0   ,0   },
    [KEY_Q]                     = {'q' ,'Q' ,'Q' ,0   },
    [KEY_W]                     = {'w' ,'W' ,'W' ,0   },
    [KEY_E]                     = {'e' ,'E' ,'E' ,0   },
    [KEY_R]                     = {'r' ,'R' ,'R' ,0   },
    [KEY_T]                     = {'t' ,'T' ,'T' ,0   },
    [KEY_Y]                     = {'y' ,'Y' ,'Y' ,0   },
    [KEY_U]                     = {'u' ,'U' ,'U' ,0   },
    [KEY_I]                     = {'i' ,'I' ,'I' ,0   },
    [KEY_O]                     = {'o' ,'O' ,'O' ,0   },
    [KEY_P]                     = {'p' ,'P' ,'P' ,0   },
    [KEY_OEM_OPEN_BRACKETS]     = {'[' ,'{' ,0   ,0   },
    [KEY_OEM_CLOSE_BRACKETS]    = {']' ,'}' ,0   ,0   },
    [KEY_OEM_PIPE]              = {'\\','|' ,0   ,0   },
    [KEY_A]                     = {'a' ,'A' ,'A' ,0   },
    [KEY_S]                     = {'s' ,'S' ,'S' ,0   }, 
    [KEY_D]                     = {'d' ,'D' ,'D' ,0   }, 
    [KEY_F]                     = {'f' ,'F' ,'F' ,0   }, 
    [KEY_G]                     = {'g' ,'G' ,'G' ,0   }, 
    [KEY_H]                     = {'h' ,'H' ,'H' ,0   }, 
    [KEY_J]                     = {'j' ,'J' ,'J' ,0   }, 
    [KEY_K]                     = {'k' ,'K' ,'K' ,0   }, 
    [KEY_L]                     = {'l' ,'L' ,'L' ,0   }, 
    [KEY_OEM_SEMICOLON]         = {';' ,':' ,0   ,0   }, 
    [KEY_OEM_QUOTES]            = {'\'','"' ,0   ,0   }, 
    [KEY_ENTER]                 = {'\n',0   ,0   ,0   },
    [KEY_Z]                     = {'z' ,'Z' ,'Z' ,0   }, 
    [KEY_X]                     = {'x' ,'X' ,'X' ,0   }, 
    [KEY_C]                     = {'c' ,'C' ,'C' ,0   }, 
    [KEY_V]                     = {'v' ,'V' ,'V' ,0   }, 
    [KEY_B]                     = {'b' ,'B' ,'B' ,0   }, 
    [KEY_N]                     = {'n' ,'N' ,'N' ,0   }, 
    [KEY_M]                     = {'m' ,'M' ,'M' ,0   }, 
    [KEY_OEM_COMMA]             = {',' ,'<' ,0   ,0   }, 
    [KEY_OEM_PERIOD]            = {'.' ,'>' ,0   ,0   }, 
    [KEY_OEM_BACKSLASH]         = {'/' ,'?' ,0   ,0   },
    [KEY_SPACE]                 = {' ' ,0   ,0   ,0   },
    [KEY_DIVIDE]                = {'/' ,0   ,0   ,0   },
    [KEY_MULTIPLY]              = {'*' ,0   ,0   ,0   },
    [KEY_SUBSTRACT]             = {'-' ,0   ,0   ,0   },
    [KEY_ADD]                   = {'+' ,0   ,0   ,0   },
    [KEY_NUMPAD7]               = {0   ,0   ,0   ,'7' },
    [KEY_NUMPAD8]               = {0   ,0   ,0   ,'8' },
    [KEY_NUMPAD9]               = {0   ,0   ,0   ,'9' },
    [KEY_NUMPAD4]               = {0   ,0   ,0   ,'4' },
    [KEY_NUMPAD5]               = {0   ,0   ,0   ,'5' },
    [KEY_NUMPAD6]               = {0   ,0   ,0   ,'6' },
    [KEY_NUMPAD1]               = {0   ,0   ,0   ,'1' },
    [KEY_NUMPAD2]               = {0   ,0   ,0   ,'2' },
    [KEY_NUMPAD3]               = {0   ,0   ,0   ,'3' },
    [KEY_NUMPAD0]               = {0   ,0   ,0   ,'0' },
    [KEY_DECIMAL]               = {0   ,0   ,0   ,'.' },

};

uint8_t get_key_event(uint8_t scan_code){
    if(scan_status.e0)
    {
        return qwerty_scan_table_E0[scan_code];
    }
    else
    {
        return qwerty_scan_table[scan_code];
    }
}
char get_char(uint8_t key){
    char ascii_key;
    if(scan_status.number_lock){
        ascii_key = ASCII_KEY_MAP[key].numlock;
        if(ascii_key)
        return ascii_key;
    }

    if(scan_status.caps_lock && !scan_status.shift){
        ascii_key = ASCII_KEY_MAP[key].caps;
        if(ascii_key)
        return ascii_key;
    }
        
    if(scan_status.caps_lock && scan_status.shift){
        ascii_key = ASCII_KEY_MAP[key].caps;
        if(ascii_key)
        return ASCII_KEY_MAP[key].normal;
    }

    if(scan_status.shift){
        ascii_key = ASCII_KEY_MAP[key].shift;
        if(ascii_key)
        return ascii_key;
    }

    ascii_key = ASCII_KEY_MAP[key].normal;
    return ascii_key;
}
void handle_code(uint8_t key_code){
    switch (key_code)
    {
    case KEY_LCTRL:
    case KEY_RCTRL:
            scan_status.ctrl = scan_status.type == KEY_RELEASED?0:1;
        break;
    case KEY_ALT:
    case KEY_LMENU:
        scan_status.alt = scan_status.type == KEY_RELEASED?0:1;
        break;
    case KEY_LSHIFT:
    case KEY_RSHIFT:
        scan_status.shift = scan_status.type == KEY_RELEASED?0:1;
        break;
    case KEY_LWIN:
    case KEY_RWIN:
        scan_status.winkey = scan_status.type == KEY_RELEASED?0:1;
        break;
    case KEY_CAPSLOCK:
        if(scan_status.type == KEY_RELEASED){
            scan_status.caps_lock = scan_status.caps_lock>0?0:1;
        }
        break;
    case KEY_NUMLOCK:
        if(scan_status.type == KEY_RELEASED){
            scan_status.number_lock = scan_status.number_lock>0?0:1;
        }
        break;
    case KEY_SCROLL:
        if(scan_status.type == KEY_RELEASED){
            scan_status.scroll_lock = scan_status.scroll_lock>0?0:1;
        }
        break;
    default:
        break;
    }

    keyboard_event_t keyboard_event;
    keyboard_event.ascii = get_char(key_code);
    keyboard_event.alt = scan_status.alt;
    keyboard_event.ctrl = scan_status.ctrl;
    keyboard_event.alt = scan_status.alt;
    keyboard_event.shift = scan_status.shift;
    keyboard_event.key_code = key_code;
    if(scan_status.type == KEY_RELEASED && key_up_handler){
        key_up_handler(keyboard_event);
    }else if(scan_status.type == KEY_PRESSED && key_down_handler){
        key_down_handler(keyboard_event);
    }
}
void handle_scan(uint8_t scan_code){
    uint8_t key_code;
    if(scan_code == 0xE0){
        // Extension
        scan_status.e0=1;
        log_trace("Keyboard handle extension waiting next interrupt value");
        return;
    }
    else if(scan_code & 0x80)
    {
        // KEY RELEASED
        key_code = get_key_event(scan_code & 0x7F);
        scan_status.type = KEY_RELEASED;
        scan_status.e0=0;
    } else {
        // KEY PRESSED
        key_code = get_key_event(scan_code);
        scan_status.type = KEY_PRESSED;
        scan_status.e0=0;
    }

    log_trace("Keyboard (%s) interrupt scan code: 0x%x mapped to key code: 0x%x, extenstion status:%d",scan_status.type==KEY_RELEASED?"Released":"Preseed",(uint32_t)scan_code,(uint32_t)key_code,(uint32_t)scan_status.e0);
    handle_code(key_code);
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
        handle_scan(scancode);
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