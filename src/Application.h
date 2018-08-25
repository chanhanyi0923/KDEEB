#pragma once

#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl32.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <bitset>
#include "DataSet.h"
#include "Iteration.h"
#include "Framebuffer.h"
#include "Gradient.h"
#include "Image.h"
#include "Frame.h"

class Application
{
private:

    // temp
    const char * inputPath = "/home/hanyi/KDEEB/others/1group/";

    //const double obstacleRadius = 0.0;
	const float pointSplitDistant = 0.0001f;
	const float pointRemoveDistance = 0.0001f;
	std::vector<Iteration> iterationsForNonRealTime;
	std::vector<Iteration> iterationsForRealTime;

    EGLDisplay eglDpy;
	// std::vector<Frame> frames;
	DataSet dataSet;
	Gradient gradient;
    Shader shader;

    // for morphing
    std::vector<Point> refPoints;

	void Input(const char * inputFile);
	void Input();
	void Output(const char * outputFile);
	//void Output();

	void TerminateOpenGL();
	void InitOpenGL();
	void Config();
    void NonRealTimeBundleWithWaypoint();
    void NonRealTimeBundle();
    void RealTimeBundle();
    void InitBundle();
	void Bundle(const Iteration & iteration);
    void BundleWithWaypoint(const Iteration & iteration);

public:
	Application();
	~Application();
	void Run(const char * inputFile, const char * outputFile);
};

