#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdlib.h>
#include "common.h"
#include "genbits.h"
#include "bit2constellation.h"
#include "myfixed.h"
#include "fft.h"
#include "pilot.h"
#include "cpandwindow.h"
#include "transmitter.h"
#include "LeaderSequence.h"
#include "DelayCorr.h"
#include "CarrierSync.h"
#include "SymbolSync.h"
#include "ChannelEqualization.h"
#include "FrequencySync.h"
#include "PhaseTracking.h"
#include "Reciver.h"
#include "Simulation.h"
int main(int argc , char* argv[]) {
    //test_fft();
    //transmitter_test();
//	delay_corr_test();  
 //   test_CFOEstimating();
    //test_matchFilter();
    simulation();
    return 0;
}
