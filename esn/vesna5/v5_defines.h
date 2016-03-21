#ifndef __VESNA5_DEFINES_H__
#define __VESNA5_DEFINES_H__

/* This file contain various constants.
 */

#undef  V5_DEBUG_PRINTING
#undef  V5_DEBUG_PRINTING_FOUND_TRACKS

/******************************************************************/

#define VDC_WIRES 240

#define MAX_SIGNALS_PER_VDC_WIRE 3

#define MIN_HITS_REQUIRED 4

/* The number of wires that may be jumped over in one instant. */
#define MAX_NO_SIGNAL_WIRES 2

/* The VDC_TIME_STD_ERR can be calibrated after having found valid
 * tracks.  One can then simply see how much the times that are part
 * of tracks are deviating from the expected values.
 *
 * Should be part of an (automatic) calibration procedure.  (Which
 * simply runs with a larger VDC_TIME_STD_ERR).
 */

#define VDC_TIME_STD_ERR  20 /* This should be calibrated!  I have no
			      * idea what is reasonable */
#define VDC_FIT_TOL       3

/* If a hit is within this tolerance compared to the expected value,
 * then it is included in the Q track.  A larger value leads to fewer
 * permutations, so it should be set to something small while doing
 * calibration.
 */ 

#define VDC_GOOD_FIT_TOL  0.5

/******************************************************************/

#endif /* __VESNA5_DEFINES_H__ */
