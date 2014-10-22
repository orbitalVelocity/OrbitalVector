//
//  GPUTimer.h
//  GLFW3_test
//
//  Created by Si Li on 10/21/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#ifndef __GLFW3_test__GPUTimer__
#define __GLFW3_test__GPUTimer__

#include <iostream>
#include "includes.h"

#define GPU_QUERY_COUNT 5
struct GPUtimer {
	int supported;
	int cur, ret;
	unsigned int queries[GPU_QUERY_COUNT];
};

void initGPUTimer(GPUtimer* timer);
void startGPUTimer(GPUtimer* timer);
int stopGPUTimer(GPUtimer* timer, float* times, int maxTimes);

#endif /* defined(__GLFW3_test__GPUTimer__) */
