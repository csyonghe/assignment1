#include <stdio.h>
#include <thread>

#include "CycleTimer.h"

typedef struct {
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
    int maxIterations;
    int* output;
    int threadId;
    int numThreads;
} WorkerArgs;


extern void mandelbrotSerial(
    float x0, float y0, float x1, float y1,
    int width, int height,
    int startRow, int numRows,
    int maxIterations,
    int output[]);


//
// workerThreadStart --
//
// Thread entrypoint.
void* workerThreadStart(void* threadArgs) {

    WorkerArgs* args = static_cast<WorkerArgs*>(threadArgs);

    // TODO: Implement worker thread here.
	mandelbrotSerial(args->x0, args->y0, args->x1, args->y1, 
		args->width, args->height, 
		args->height / args->numThreads * args->threadId, 
		args->height / args->numThreads, 
		args->maxIterations, 
		args->output);
    printf("Hello world from thread %d\n", args->threadId);

    return NULL;
}

//
// MandelbrotThread --
//
// Multi-threaded implementation of mandelbrot set image generation.
// Multi-threading performed via pthreads.
void mandelbrotThread(
    int numThreads,
    float x0, float y0, float x1, float y1,
    int width, int height,
    int maxIterations, int output[])
{
    const static int MAX_THREADS = 32;

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(1);
    }

	std::thread workers[MAX_THREADS - 1];
    WorkerArgs args[MAX_THREADS];

    for (int i = 0; i < numThreads; i++) {
        // TODO: Set thread arguments here.
        args[i].threadId = i;
		args[i].x0 = x0;
		args[i].y0 = y0;
		args[i].x1 = x1;
		args[i].y1 = y1;
		args[i].width = width;
		args[i].height = height;
		args[i].maxIterations = maxIterations;
		args[i].output = output;
		args[i].numThreads = numThreads;
    }

    // Fire up the worker threads.  Note that numThreads-1 pthreads
    // are created and the main app thread is used as a worker as
    // well.

	for (int i = 0; i < numThreads - 1; i++)
	{
        workers[i] = std::thread(workerThreadStart, &args[i + 1]);
	}

    workerThreadStart(&args[0]);

    // wait for worker threads to complete
    for (int i = 0; i < numThreads - 1; i++)
        workers[i].join();
}
