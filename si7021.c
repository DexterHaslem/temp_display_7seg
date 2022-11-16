#include "si7021.h"
#include "mcc_generated_files/examples/i2c1_master_example.h"

#define ADDR (0x40)

static uint8_t scratch[8];
static uint8_t *const psc = &scratch[0];

static uint8_t read() {
    I2C1_ReadNBytes(ADDR, psc, 1);
    return scratch[0];
}

static void write(uint8_t val) {
    scratch[0] = val;
    I2C1_WriteNBytes(ADDR, psc, 1);
}

void si7021_soft_reset(void) {
    write(0xfe);
}

void si7021_meas_rh_nohold(void) {
    write(0xf5);
}

void si7021_meas_rh_hold(void) {
    write(0xe5);
}

void si7021_meas_temp_hold(void) {
    write(0xe3);
}

void si7021_meas_temp_nohold(void) {
    write(0xf3);
}

float si7021_get_temp_postrh(void) {
    //write(0xe0);  // read postrh
    uint16_t tr = I2C1_Read2ByteRegister(ADDR, 0xe0);
    RA0 = 1;
    float tf = ((175.72*tr)/65536) - 46.85;
    return tf;
}
       