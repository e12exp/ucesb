
/*
 * S223 (run 369)
 */

// siderem01/siderem01 /net/data2/s223/lmd/run_369_3888.lmd --max-events=4000 > ped_sst.txt
// grep "Pedestal " ped_sst.txt  | grep "x:" | sed -e 's/SST\([0-9]*\).* \([0-9]*\): *\([0-9.]*\)/CALIB_PARAM( SST[\1][\2].E,OFFSET_SLOPE, -\3 , 1.0 ); \/\//' > siderem01/calibration_x.hh
// grep "Pedestal " ped_sst.txt  | grep "y:" | sed -e 's/SST\([0-9]*\).* \([0-9]*\): *\([0-9.]*\)/echo "CALIB_PARAM( SST[\1][\" \`dc -e \"\2 640 + p\"\` \"].E,OFFSET_SLOPE, -\3 , 1.0 ); \/\/" /' > siderem01/calibration_y.hh.sh
// /bin/sh siderem01/calibration_y.hh.sh > siderem01/calibration_y.hh

#include "calibration_x.hh"
#include "calibration_y.hh"

/*
 * S271

#include "calib_sst_0.hh"
#include "calib_sst_1.hh"
#include "calib_sst_2.hh"
#include "calib_sst_3.hh"
*/
