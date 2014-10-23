//
//  GPUTimer.cpp
//  GLFW3_test
//
//  Created by Si Li on 10/21/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "GPUTimer.h"
#include <GLFW/glfw3.h>

void initGPUTimer(GPUtimer* timer)
{
	memset(timer, 0, sizeof(*timer));
    
    timer->supported = glfwExtensionSupported("GL_ARB_timer_query");
    if (timer->supported) {
#ifndef GL_ARB_timer_query
        glGetQueryObjectui64v = (pfnGLGETQUERYOBJECTUI64V)glfwGetProcAddress("glGetQueryObjectui64v");
        printf("glGetQueryObjectui64v=%p\n", glGetQueryObjectui64v);
        if (!glGetQueryObjectui64v) {
            timer->supported = GL_FALSE;
            return;
        }
#endif
        glGenQueries(GPU_QUERY_COUNT, timer->queries);
    }
}

void startGPUTimer(GPUtimer* timer)
{
	if (!timer->supported)
		return;
	glBeginQuery(GL_TIME_ELAPSED, timer->queries[timer->cur % GPU_QUERY_COUNT] );
	timer->cur++;
}

int stopGPUTimer(GPUtimer* timer, float* times, int maxTimes)
{
//	NVG_NOTUSED(times);
//	NVG_NOTUSED(maxTimes);
	GLint available = 1;
	int n = 0;
	if (!timer->supported)
		return 0;
    
	glEndQuery(GL_TIME_ELAPSED);
	while (available && timer->ret <= timer->cur) {
		// check for results if there are any
		glGetQueryObjectiv(timer->queries[timer->ret % GPU_QUERY_COUNT], GL_QUERY_RESULT_AVAILABLE, &available);
		if (available) {
            GLuint64 timeElapsed = 0;
            glGetQueryObjectui64v(timer->queries[timer->ret % GPU_QUERY_COUNT], GL_QUERY_RESULT, &timeElapsed);
            timer->ret++;
            if (n < maxTimes) {
                times[n] = (float)((double)timeElapsed * 1e-9);
                n++;
            }
		}
	}
	return n;
}