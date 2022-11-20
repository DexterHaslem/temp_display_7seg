
#include "segments.h"
#include "si7021.h"

/* tmr0 goes off 250ms, update display this often*/
#define TEMP_UPDATE_RATE_TICKS (2)

/* switch between temp and RH every 15 seconds*/
#define DISP_MODE_SWITCH_TICKS ((1000/250) * 15)


static uint8_t disp_mode_count = 0;
static bool display_rh = false;
static bool parse_next = false;

/* not strictly needed, could use temps in the parse functions, but avoid bank
 switches */
static uint8_t temp_scalar;
static uint8_t temp_vals[3]; /* broken down temp digits for display */
static uint8_t rh_vals[3]; /* broken down RH digits for display */
static uint8_t rh_scalar;
static float temp_c;
static float temp_f;
static float rh;

static uint8_t shiftcnt = 0; /* number of digits to show (3 or 4 once temp read) */
static uint8_t disps[4]; /* actual display dec->7seg digit values */


static volatile uint8_t ticks = 0;
static volatile bool dirty = false;

static void isr_on_tmr0(void) {
    ticks++;
    dirty = true;
}

static void req_parse_rh(void) {
    rh_scalar = (uint8_t) rh;
    uint8_t remain = (uint8_t) ((rh - rh_scalar) * 10);
    rh_vals[0] = (uint8_t) rh_scalar / 10 % 10;
    rh_vals[1] = (uint8_t) rh_scalar % 10;

    disps[0] = seg_h;
    disps[1] = seg_nums[remain];
    disps[2] = seg_nums[rh_vals[1]] | seg_dp;
    disps[3] = seg_nums[rh_vals[0]];

    shiftcnt = 4;
}

static void req_parse_temp(void) {
    temp_c = si7021_get_temp_postrh();
    temp_f = (temp_c * (9.0f / 5.0f)) + 32.0f;
    temp_scalar = (uint8_t) temp_f;
    
    //uint8_t remain = (uint8_t)((temp_f - temp_scalar) * 10);
    temp_vals[1] = (uint8_t) temp_scalar / 10 % 10;
    temp_vals[2] = (uint8_t) temp_scalar % 10;

    /*display enable bits go from lsb msb so put right most display first*/
    //disps[0] = seg_nums[remain];
    disps[0] = seg_f;
    disps[1] = seg_nums[temp_vals[2]]; // | seg_dp; 
    disps[2] = seg_nums[temp_vals[1]];
    disps[3] = seg_nums[temp_vals[0]];

    if (temp_scalar > 99) {
        temp_vals[0] = 1;
        /* if temp got to 200f we have bigger problems */
        shiftcnt = 4;
    } else {
        shiftcnt = 3;
    }
}

static uint8_t digit_cathode_shift = 0;

static void isr_on_display_timer(void) {
    /* mux the pins in a very fast timer so they all appear visible
     at once to the human eye. also dont do this in the main logic loop 
     or else it will stutter on i2c updates etc */
    if (shiftcnt) {
        LATD = (uint8_t) ~(((uint8_t) 1) << digit_cathode_shift);
        LATC = disps[digit_cathode_shift];
        if (++digit_cathode_shift >= shiftcnt) {
            digit_cathode_shift = 0;
        }
    }
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
    rh = si7021_meas_rh_hold();
    parse_next = true;
    dirty = true;

    while (1) {
        if (dirty) {
            if (++disp_mode_count >= DISP_MODE_SWITCH_TICKS) {
                disp_mode_count = 0;
                display_rh = !display_rh;
            }
            if (parse_next) {
                if (display_rh) {
                    req_parse_rh();
                } else {
                    req_parse_temp();
                }
                parse_next = false;
            }

            /* always refresh reading */
            rh = si7021_meas_rh_hold();
            dirty = false;
        } else if (ticks >= TEMP_UPDATE_RATE_TICKS) {
            parse_next = true;
            ticks = 0;
            continue;
        }
    }
}