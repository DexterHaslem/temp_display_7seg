
#include "segments.h"
#include "si7021.h"

/* tmr0 goes off every second, update display this often*/
#define TEMP_UPDATE_RATE_TICKS (5)

static volatile uint8_t ticks = 0;
static volatile bool parse_next = false;
static volatile bool dirty = false;
static uint8_t temp_scalar;
static uint8_t digit_vals[3];
static uint8_t disps[4];
static uint8_t shiftcnt = 0;

static void on_tmr0(void) {
    ticks++;
    dirty = true;
}

static void req_parse_temp(void) {
    float temp_c = si7021_get_temp_postrh();
    float temp_f = (temp_c * (9.0f/5.0f)) + 32.0f;
    temp_scalar = (uint8_t) (temp_f);
    uint8_t remain = (uint8_t)((temp_f - temp_scalar) * 10);
    digit_vals[1] = (uint8_t)temp_scalar / 10 % 10;
    digit_vals[2] = (uint8_t)temp_scalar % 10;
    
    //printf("F = %f\r\n", temp_f);
    
    
    /*display enable bits go from lsb msb so put right most display first*/
    disps[0] = seg_nums[remain];
    disps[1] = seg_nums[digit_vals[2]] | seg_dp;
    disps[2] = seg_nums[digit_vals[1]];
    disps[3] = seg_nums[digit_vals[0]];
    
    if (temp_scalar > 99) {
        digit_vals[0] = 1;
        /* if temp got to 200f we have bigger problems */
        shiftcnt = 4;
    } else {
        shiftcnt = 3;        
    }
}

static uint8_t digit_cathode_shift = 0;
static void on_display_timer(void) {
    //IO_RA0_Toggle();
#if 1
    /* mux the pins in a very fast timer so they all appear visible
     at once to the human eye. also dont do this in the main logic loop 
     or else it will stutter on i2c updates etc */
    if (shiftcnt) {
        LATD = (uint8_t)~(((uint8_t)1) << digit_cathode_shift);
        LATC = disps[digit_cathode_shift];
//        if (++digit_cathode_shift > 4) {
//            digit_cathode_shift = 0;
//        }
        
        if (++digit_cathode_shift >= shiftcnt) {
            digit_cathode_shift = 0;
        }
    }
#endif
}

void main(void) {
    SYSTEM_Initialize();
    
    /* common cathode on port d, turn all digits off */
    LATD = 0xff;
    LATC = 0x00;
           
    TMR0_SetInterruptHandler(on_tmr0);   
    TMR1_SetInterruptHandler(on_display_timer);
    //TMR3_StartTimer();
    //INTERRUPT_GlobalInterruptHighEnable();
    //INTERRUPT_GlobalInterruptLowEnable();
    /* enable interrupts before trying i2c */
    INTERRUPT_GlobalInterruptEnable();
 

    __delay_ms(1);
    printf("\r\r\rresetting sensor..\r\n");
    
    si7021_soft_reset();
    
    printf("done, starting loop\r\n");   
   
    while (1) {
#if 1
        /* every 1 second tick run logic */
        if (dirty) {
            if (parse_next) {
                //printf("updating display\r\n");
                req_parse_temp();
                //printf("done2\r\n");
                parse_next = false;
            }
            
            /* always refresh reading */
            si7021_meas_rh_nohold();
            dirty = false;
        }
        
        if (ticks >= TEMP_UPDATE_RATE_TICKS) {
            //printf("doing update\r\n");
            parse_next = true;
            ticks = 0;
            continue;
        }
#endif
    }
}