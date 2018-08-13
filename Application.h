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
    const double obstacleRadius = 0.0;
	const float pointSplitDistant = 0.003f;
	const float pointRemoveDistance = 0.003f;
	std::vector<Iteration> iterationsForNonRealTime;
	std::vector<Iteration> iterationsForRealTime;

    EGLDisplay eglDpy;
	// std::vector<Frame> frames;
	DataSet dataSet;
	Gradient gradient;
    Shader shader;
public:
	Application();
	~Application();

	void TerminateOpenGL();
	void InitOpenGL();
	void Config();

	void Input(const char * inputFile);
	//void Input();
	void Output(const char * outputFile);
	//void Output();

	void Run(const char * inputFile, const char * outputFile);

    void NonRealTimeBundle();
    void RealTimeBundle();
    void InitBundle();
	void Bundle(const Iteration & iteration);
};

