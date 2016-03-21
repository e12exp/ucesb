#ifndef __VESNA5_CALIB_H__
#define __VESNA5_CALIB_H__

/* This file contain definition of data structures used to
 * auto-calibrate the parameters...
 */

/******************************************************************/

void
calib_init();

void
first_round_calibration();

/******************************************************************/

void 
cal_vdc_tdc(int vdc,int plane,vdc_hit* hit,int nhits);

void 
cal_vdc_tdc_init(const char *prefix);

void 
cal_vdc_tdc_exit();

void
vdc_tdc_calib();

/******************************************************************/

#endif /* __VESNA5_CALIB_H__ */

