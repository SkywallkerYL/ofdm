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


int main(int argc , char* argv[]) {
    //test_fft();
    transmitter_test();
	
    return 0;
}
