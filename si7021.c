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

static float _convert_rh() {
    /* datasheet notes lsb always xxxxxx10 for RH. but no need to mask out */
    uint16_t raw = (uint16_t)(scratch[0] << 8) | scratch[1];
    float rh = ((125.0f*raw)/65536) - 6;
    return rh;
}

float si7021_meas_rh_nohold(void) {
    write(0xf5);
    I2C1_ReadNBytes(ADDR, psc, 2);    
    return _convert_rh();
}

float si7021_meas_rh_hold(void) {
    write(0xe5);
    I2C1_ReadNBytes(ADDR, psc, 2);    
    return _convert_rh();
}

void si7021_meas_temp_hold(void) {
    write(0xe3);
}

void si7021_meas_temp_nohold(void) {
    write(0xf3);
}

float si7021_get_temp_postrh(void) {
    //write(0xe0);  // read postrh
    //uint16_t tr = I2C1_Read2ByteRegister(ADDR, 0xe0);
    write(0xe0);
    I2C1_ReadNBytes(ADDR, psc, 2);
    /* datasheet notes lsb always xxxxxx00 for temp. but no need to mask out */
    uint16_t raw = (uint16_t)(scratch[0] << 8) | scratch[1];
    float tf = ((175.72f*raw)/65536) - 46.85f;
    return tf;
}
       