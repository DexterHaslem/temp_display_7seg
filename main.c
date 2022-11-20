
#include "segments.h"
#include "si7021.h"

/* tmr0 goes off every second, update display this often*/
#define TEMP_UPDATE_RATE_TICKS (45)

static volatile uint8_t ticks = 0;
static volatile bool parse_next = false;
static volatile bool dirty = false;
static uint8_t temp_scalar;
static uint8_t digit_vals[3]; /* broken down digits for display */
static uint8_t disps[4];      /* actual display dec->7seg digit values */
static uint8_t shiftcnt = 0;  /* number of digits to show (3 or 4 once temp read) */

static void isr_on_tmr0(void) {
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
static void isr_on_display_timer(void) {
    //IO_RA0_Toggle();
#if 1
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
#endif
}

void main(void) {
    SYSTEM_Initialize();
    
    /* common cathode on port d, turn all digits off, clear portc drives */
    LATD = 0xff;
    LATC = 0x00;
           
    TMR0_SetInterruptHandler(isr_on_tmr0);   
    TMR1_SetInterruptHandler(isr_on_display_timer);
    
    /* enable interrupts before trying i2c as its int driven */
    INTERRUPT_GlobalInterruptEnable();

    __delay_ms(2);
    printf("\r\r\rresetting sensor..\r\n");
    
    si7021_soft_reset();
    
    printf("done, starting loop\r\n");   
   
    /* ask right away so we dont start blank. use the bus holding measure for
     * this reason, so we dont try to read it too early after and get NAK'd */
    si7021_meas_rh_hold();
    parse_next = true;
    dirty = true;
    
    while (1) {
        if (dirty) {
            if (parse_next) {
                req_parse_temp();
                parse_next = false;
            }
            
            /* always refresh reading */
            si7021_meas_rh_nohold();
            dirty = false;
        } else if (ticks >= TEMP_UPDATE_RATE_TICKS) {
            parse_next = true;
            ticks = 0;
            continue;
        }
    }
}