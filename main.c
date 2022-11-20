
#include "segments.h"
#include "si7021.h"

/* tmr0 goes off every second, update display this often*/
#define TEMP_UPDATE_RATE_TICKS (30)

static volatile uint8_t ticks = 0;
static volatile bool parse_next = false;
static volatile bool dirty = false;


static void on_tmr0(void) {
    ticks++;
    dirty = true;
}

uint8_t temp_scalar;

uint8_t digit1_val;
uint8_t digit2_val;
uint8_t digit3_val;

uint8_t disps[4];
uint8_t shiftcnt = 0;

static void req_parse_temp(void) {
    /* turn off display incase display interrupt goes off while doing heavy float */
    //shiftcnt = 0; 
    float temp_c = si7021_get_temp_postrh();
    float temp_f = (temp_c * (9.0f/5.0f)) + 32.0f;
    temp_scalar = (uint8_t) (temp_f);
    uint8_t remain = (uint8_t)((temp_f - temp_scalar) * 10);
    digit2_val = (uint8_t)temp_scalar / 10 % 10;
    digit3_val = (uint8_t)temp_scalar % 10;
    
    /*display enable bits go from lsb msb so put right most display first*/
    disps[0] = seg_nums[remain];
    disps[1] = seg_nums[digit3_val] | seg_dp;
    disps[2] = seg_nums[digit2_val];
    disps[3] = seg_nums[digit1_val];
    
    if (temp_scalar > 99) {
        digit1_val = 1;
        shiftcnt = 4;
    } else {
        shiftcnt = 3;        
    }
}

static uint8_t digit_cathode_shift = 0;
static void on_display_timer(void) {
    /* mux the pins in a very fast timer so they all appear visible
     at once to the human eye. also dont do this in the main logic loop 
     or else it will stutter on i2c updates etc */
    if (shiftcnt) {
        LATD = (uint8_t)~(((uint8_t)1) << digit_cathode_shift);
        LATC = disps[digit_cathode_shift];
        if (++digit_cathode_shift >= shiftcnt) {
            digit_cathode_shift = 0;
        }
    }
}

void main(void) {
    SYSTEM_Initialize();
    
    /* common cathode on port d, turn all digits off */
    LATD = 0xff;
    LATC = 0x00;
    
    TMR0_SetInterruptHandler(on_tmr0);   
    TMR3_SetInterruptHandler(on_display_timer);
    
    
    /* enable interrupts before trying i2c */
    INTERRUPT_GlobalInterruptLowEnable();
    INTERRUPT_GlobalInterruptHighEnable();
    
    printf("resetting sensor..\r\n");
    
    si7021_soft_reset();
    
    printf("done, starting loop\r\n");
    
    while (1) {
        /* always do this, we basic spin loop driving the display */
        //update_segment_mux();
        
        /* every 1 second tick run logic */
        if (dirty) {
            if (parse_next) {
                req_parse_temp();
            }
            
            /* always refresh reading */
            si7021_meas_rh_nohold();
            dirty = false;                        
        }
        
        if (shiftcnt == 0 || ticks >= TEMP_UPDATE_RATE_TICKS) {
            parse_next = true;
            ticks = 0;
            continue;
        }      
    }
}