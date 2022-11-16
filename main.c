
#include "segments.h"
#include "si7021.h"

#define UPDATE_SECS (5)


static volatile bool dirty = true;
static volatile uint8_t ticks = 0;

static void on_tmr0(void) {
    ++ticks;
    dirty = true;
}

void main(void) {
    SYSTEM_Initialize();

    /* common anode , turn all digits off */
    LATD = 0xff;
    INTERRUPT_GlobalInterruptEnable();

    /* tmr0 at approx 1 sec */
    TMR0_SetInterruptHandler(on_tmr0);
    
    si7021_soft_reset();
    printf("\r\r\r\nstarted up\r\n");
    
    LATC = seg_nums[8] | seg_dp;
    LATDbits.LATD0 = 0;
    while (1) {
                
        if (ticks % UPDATE_SECS == 0 && dirty) {                       
            si7021_meas_rh_hold();
            __delay_ms(23);
            float t_try = si7021_get_temp_postrh();
            //printf("handled timer: %f\r\n", t_try);
            RA0 = 0;
            dirty = false;
        }
        
//        for (uint8_t dig = 0; dig < 5; ++dig) {
//            LATD = ~((uint8_t) 1 << dig);
//            __delay_ms(3);
//        }
    }
}