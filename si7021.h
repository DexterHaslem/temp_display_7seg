
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include "mcc_generated_files/mcc.h"

void si7021_soft_reset(void);
float si7021_meas_rh_nohold(void);
float si7021_meas_rh_hold(void);
void si7021_meas_temp_hold(void);
void si7021_meas_temp_nohold(void);
float si7021_get_temp_postrh(void);

#endif	/* XC_HEADER_TEMPLATE_H */

