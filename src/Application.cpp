#include "Application.h"

#include <iostream>

Application::Application()
{
}


Application::~Application()
{
    this->TerminateOpenGL();
}


void Application::TerminateOpenGL()
{
    // 6. Terminate EGL when finished
    eglTerminate(this->eglDpy);
}


void Application::InitOpenGL()
{
    const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
    };

    const int pbufferWidth = 200;
    const int pbufferHeight = 200;

    const EGLint pbufferAttribs[] = {
        EGL_WIDTH, pbufferWidth,
        EGL_HEIGHT, pbufferHeight,
        EGL_NONE,
    };

    // 1. Initialize EGL
    const int MAX_DEVICES = 10;
    EGLDeviceEXT eglDevs[MAX_DEVICES];
    EGLint numDevices;

    PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT =
        (PFNEGLQUERYDEVICESEXTPROC)eglGetProcAddress("eglQueryDevicesEXT");

    eglQueryDevicesEXT(MAX_DEVICES, eglDevs, &numDevices);

    std::cout << "Detected " << numDevices << " devices" << std::endl;

    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =
        (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");

    this->eglDpy = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT, eglDevs[0], 0);

    // If you want to use on-screen rendering,
    // just delete the code up there.
    // And use following line:
    // this->eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint major, minor;

    eglInitialize(this->eglDpy, &major, &minor);

    // 2. Select an appropriate configuration
    EGLint numConfigs;
    EGLConfig eglCfg;

    eglChooseConfig(this->eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

    // 3. Create a surface
    EGLSurface eglSurf = eglCreatePbufferSurface(this->eglDpy, eglCfg, pbufferAttribs);

    // 4. Bind the API
    eglBindAPI(EGL_OPENGL_API);

    // 5. Create a context and make it current
    EGLContext eglCtx = eglCreateContext(this->eglDpy, eglCfg, EGL_NO_CONTEXT, nullptr);

    if (eglCtx == nullptr) {
        throw RenderException("eglCreateContext failed");
    }

    eglMakeCurrent(this->eglDpy, eglSurf, eglSurf, eglCtx);
}


void Application::Input(const char * inputFile)
{

    using std::vector;
    using std::string;
    using std::fstream;
    using std::stringstream;
    using std::bitset;

    fstream input;
    input.open(inputFile, std::fstream::in);

    if (!input) {
        throw RenderException("Could not open input file");
    }


    // read nodes
    {
        char file1[100] = "/home/hanyi/KDEEB/others/new_dataset/Nodes.csv";
        //sprintf(file1, "/home/hanyi/KDEEB/others/test7/nodes.txt", this->inputPath);
        fstream nodeIn;
        nodeIn.open(file1, fstream::in);
        this->refPoints.resize(500000);

        int index;
        double x, y;
        std::string str, str2, str3;
        while (std::getline(nodeIn, str)) {
            std::stringstream ss(str);
            {
                std::getline(ss, str2, ',');
                std::stringstream ss2(str2);
                ss2 >> index;
                index--;
            }
            {
                std::getline(ss, str2, ',');
                std::stringstream ss2(str2);
                ss2 >> x;
            }
            {
                std::getline(ss, str2, ',');
                std::stringstream ss2(str2);
                ss2 >> y;
            }

            Point p(x, y, 0);
            p.fixed = true;
            p.id = index;
            this->refPoints[index] = p;
        }
        nodeIn.close();
    }

    this->dataSet.xMin = 30.5968 - 0.05;
    this->dataSet.yMin = 104.0160 - 0.05;
    this->dataSet.xMax = 30.7258 + 0.05;
    this->dataSet.yMax = 104.1605 + 0.05;
    

    int tripsNum = 0;

    this->dataSet.lines.resize(500000);
    
    std::vector<int> indices;

    for (string stringLine; getline(input, stringLine); ) {
        stringstream lineBuffer(stringLine);
        
        int a, b, c;
        lineBuffer >> a >> b >> c;

        Line & line = dataSet.lines[a - 1];
        indices.push_back(a - 1);
        line.AddPoint(this->refPoints[b - 1]);
        line.AddPoint(this->refPoints[c - 1]);

        // {
        //   const Point &p = this->refPoints[b - 1];
        //   this->dataSet.xMin = std::min(this->dataSet.xMin, p.x);
        //   this->dataSet.yMin = std::min(this->dataSet.yMin, p.y);
        //   this->dataSet.xMax = std::max(this->dataSet.xMax, p.x);
        //   this->dataSet.yMax = std::max(this->dataSet.yMax, p.y);
        // }
        // {
        //   const Point &p = this->refPoints[c - 1];
        //   this->dataSet.xMin = std::min(this->dataSet.xMin, p.x);
        //   this->dataSet.yMin = std::min(this->dataSet.yMin, p.y);
        //   this->dataSet.xMax = std::max(this->dataSet.xMax, p.x);
        //   this->dataSet.yMax = std::max(this->dataSet.yMax, p.y);
        // }
    }

    // normalize
    for (size_t i = 0; i < 500000; i ++) {
        double x = this->refPoints[i].x;
        double y = this->refPoints[i].y;
        x = (x - this->dataSet.xMin) / (this->dataSet.xMax - this->dataSet.xMin);
        y = (y - this->dataSet.yMin) / (this->dataSet.yMax - this->dataSet.yMin);
        this->refPoints[i].x = x;
        this->refPoints[i].y = y;
    }

    for (size_t i = 0; i < 500000; i ++) {
        Line & line = this->dataSet.lines[i];
        for (size_t j = 0; j < line.GetPointSize(); j ++) {
            Point p = this->refPoints[line.GetPoint(j).id];
            line.SetPoint(j, p);
        }
    }


    fstream output_kdeeb;
    output_kdeeb.open("/home/hanyi/KDEEB/build/in/more.txt", fstream::in);

    for (string stringLine; getline(output_kdeeb, stringLine); ) {
        stringstream buffer(stringLine);
        if (stringLine == "" || stringLine == " ") {
            continue;
        }

        int id;
        double x, y;
        buffer >> y >> x >> id;

        x = (x - this->dataSet.xMin) / (this->dataSet.xMax - this->dataSet.xMin);
        y = (y - this->dataSet.yMin) / (this->dataSet.yMax - this->dataSet.yMin);


        Point p(x, y, 0);
        this->dataSet.lines[indices[id - 1]].AddSecondLastPoint(p);
	}


    input.close();
}

void Application::Output(const char * outputFile)
{
    std::fstream output;
    output.open(outputFile, std::fstream::out);

    if (!output) {
        throw RenderException("Could not open output file");
    }

    output << this->dataSet;
    output.close();
}


void Application::Config()
{
    int count, textureWidth;
    float kernelSize;
    double attractionFactor, smoothingFactor;
    bool doResampling;
    int resampleStep;
  
    count = 10;
    textureWidth = 200;
    kernelSize = 20;
    attractionFactor = 0.01;
    smoothingFactor = 0.1;doResampling = true;
    resampleStep = 1;
  
    this->iterationsForNonRealTime.push_back(Iteration(count, textureWidth, kernelSize, attractionFactor, smoothingFactor, doResampling, resampleStep));
}


int Application::Run(int layerNum, double maxLng, double minLng, double maxLat, double minLat)
{
    //this->inputPath = inputFile;

    clock_t start, end;
    start = clock();

    this->LngRecord = maxLng; this->LatRecord = maxLat;

    this->InitOpenGL();

    this->Config();

	this->Input("/home/hanyi/KDEEB/build/in/trips.txt");

    //this->Input(inputFile);
    //int tripsNum = this->Input(layerNum, maxLng, minLng, maxLat, minLat);

/*
    if (tripsNum == 0) {
        this->TerminateOpenGL();
        return 1;
    }
*/

    this->NonRealTimeBundleWithWaypoint();
    //this->NonRealTimeBundle();

    //this->Output("xx");

    this->TerminateOpenGL();

    end = clock();
    std::cout<< "whole total time: " << (double)(end-start)/CLOCKS_PER_SEC << std::endl;

    return 0;
}



void Application::NonRealTimeBundleWithWaypoint()
{
    using std::fstream;
    using std::string;
    using std::stringstream;

    this->InitBundle();

    //this->dataSet.CreateGridGraph();

    for (int fileNum = 1; fileNum <= 26; fileNum ++) {
        char filename[100];
        sprintf(filename, "%swaypoint%d.txt", "/home/hanyi/KDEEB/build/in/", fileNum);


        fstream fin;
        fin.open(filename, fstream::in);
        for (string stringLine; getline(fin, stringLine); ) {
            std::vector<Waypoint> waypoints;

            stringstream stringLineBuffer(stringLine);
            int lineIndex;
            stringLineBuffer >> lineIndex;

            Line &line = this->dataSet.lines[lineIndex - 1];

            string remain;
            getline(stringLineBuffer, remain);
            stringstream remainBuffer(remain);
            // 130 76622 98792 186488, 98792 199048 131376, 
            for (string waypointString; getline(remainBuffer, waypointString, ','); ) {
                if (waypointString == "" || waypointString == " ") continue;
                stringstream waypointStringBuffer(waypointString);

                int oId, dId, pointId;

                waypointStringBuffer >> oId >> dId >> pointId;
                const Point &refPoint = this->refPoints[pointId - 1];

                Waypoint waypoint(refPoint.x, refPoint.y, refPoint.id);
                waypoint.oId = oId - 1;
                waypoint.dId = dId - 1;
                waypoints.push_back(waypoint);
            }

			line.AddPointsForWaypoint(waypoints);
            for (const Waypoint & waypoint: waypoints) {
                
                const size_t oId = line.FindPointIndexById(waypoint.oId);
                const size_t dId = line.FindPointIndexById(waypoint.dId);

                line.AddWaypoint(oId, dId, waypoint);
            }
        }
        fin.close();

        sprintf(filename, "%sroute_%d.txt", "/home/hanyi/KDEEB/build/in/", fileNum);

        fin.open(filename, fstream::in);
        for (string stringLine; getline(fin, stringLine); ) {
            stringstream buffer1(stringLine);
            int lineIndex, oIdInput, dIdInput;
            buffer1 >> lineIndex >> oIdInput >> dIdInput;
            lineIndex --;

            Line & line = this->dataSet.lines[lineIndex];
            if (line.GetPointSize() == 0) continue;

            line.AddSegment(oIdInput - 1, dIdInput - 1);
        }
        fin.close();

        //this->dataSet.UpdateWaypoints(this->refPoints);

        // bundle
        //#pragma omp parallel for
        for (size_t i = 0; i < this->dataSet.lines.size(); i ++) {
            Line & line = this->dataSet.lines[i];
            line.UpdatePoints();
        }


        // Iteration(count, textureWidth, kernelSize, attractionFactor, smoothingFactor, doResampling))
        //this->BundleWithWaypoint(Iteration(20, 200, 20, 0.001, 0.1, true));
        this->BundleWithWaypoint(this->iterationsForNonRealTime[0], fileNum);


        //#pragma omp parallel for
        // for (size_t i = 0; i < this->dataSet.lines.size(); i ++) {
        //     Line & line = this->dataSet.lines[i];
        //     line.ClearWaypoints();
        // }

    }
}


void Application::NonRealTimeBundle()
{
    this->InitBundle();
    for (const Iteration & iteration : this->iterationsForNonRealTime) {
        this->Bundle(iteration);
    }
}

void Application::InitBundle()
{
    // initial shader
    this->shader.LoadFragmentShader();
    this->shader.LoadVertexShader();
    this->shader.CreateProgram();
}


/*
 * Kernel Density Estimation-based Edge Bundling
 */
void Application::Bundle(const Iteration & iteration)
{
    const uint32_t textureWidth = iteration.textureWidth;

    Framebuffer framebuffer(iteration.textureWidth);
    framebuffer.Init();

    this->gradient.Resize(textureWidth * textureWidth);
    this->gradient.SetWidth(textureWidth);
    this->gradient.SetAttractionFactor(iteration.attractionFactor);

    for (int i = 0; i < iteration.count; i++) {
        Image image(dataSet, &this->shader);
        image.Init();

        std::vector<float> accD = framebuffer.ComputeSplatting(iteration.kernelSize, image);
        this->gradient.SetAccMap(&accD);
        this->gradient.ApplyGradient(this->dataSet);

        this->dataSet.SmoothTrails(iteration.smoothingFactor);
        
    }
	if (iteration.doResampling) {
        this->dataSet.AddRemovePoints(this->pointRemoveDistance, this->pointSplitDistant);
    }
}





void Application::BundleWithWaypoint(const Iteration & iteration, int layerNum)
{
    int c = 10 * (layerNum - 1);

    const uint32_t textureWidth = iteration.textureWidth;

    Framebuffer framebuffer(iteration.textureWidth);
    framebuffer.Init();

    this->gradient.Resize(textureWidth * textureWidth);
    this->gradient.SetWidth(textureWidth);
    this->gradient.SetAttractionFactor(iteration.attractionFactor);
    this->gradient.InitSteps(iteration.count);

    for (int i = 0; i < iteration.count; i++) {
        Image image(dataSet, &this->shader);
        image.Init();
        std::vector<float> accD = framebuffer.ComputeSplatting(iteration.kernelSize, image);
        this->gradient.SetAccMap(&accD);
        this->gradient.ApplyGradientWithWaypoint(this->dataSet, i);

        // if (i == iteration.count - 1) {
        //     for (size_t i = 0; i < this->dataSet.lines.size(); i ++) {
        //         Line & line = this->dataSet.lines[i];
        //         line.SetWaypointToFixed();
        //     }
        // }
        
        if (i % iteration.resampleStep == 0 || i == iteration.count - 1) {
          this->dataSet.SmoothTrailsWithWaypoint(iteration.smoothingFactor);
          this->dataSet.AddRemovePointsWithWaypoint(this->pointRemoveDistance, this->pointSplitDistant);
        }


        if (i == iteration.count - 1) {
            this->dataSet.RemovePointsInSegment();
        }



        char s[100];
        sprintf(s, "/home/hanyi/KDEEB/build/out/%.3f_%.3f_%d.csv", this->LngRecord, this->LatRecord, c);
        this->Output(s);
        std::cout << "iter num: " << c << std::endl;

		// {
		// 	size_t emptyCount = 0;
		// 	for (size_t i = 0; i < this->dataSet.lines.size(); i ++) {
		// 		const Line & line = this->dataSet.lines[i];
		// 		if (line.GetPointSize() == 0) {
		// 			emptyCount ++;
		// 		}
		// 	}
		// 	std::cout << (500000 - emptyCount) << std::endl;
		// }

        //std::stringstream ss1, ss2; std::string lng, lat;
        //ss1 << std::fixed << std::setprecision(3) << this->LngRecord; ss1 >> lng;
        //ss2 << std::fixed << std::setprecision(3) << this->LatRecord; ss2 >> lat;
        //std::string command = "/home/yanlyu/Downloads/miniconda3/envs/py3/bin/python linesformatTojson.py "+lng+"_"+lat+"_"+std::to_string(c)+"";
        //int res = system(command.c_str());
        //std::cout << "iter num: " << c << " python script status: " << res << std::endl;

        //std::thread t(loadOutputToDB, c, this->LngRecord, this->LatRecord);
        //t.detach();

        c ++;
    }
}


// void loadOutputToDB(int iterNum, double LngRecord, double LatRecord) 
// {
//         std::this_thread::sleep_for(std::chrono::milliseconds((iterNum+1)%21*100));

//         // call python script
//         std::stringstream ss1, ss2; std::string lng, lat;
//         ss1 << std::fixed << std::setprecision(3) << LngRecord; ss1 >> lng;
//         ss2 << std::fixed << std::setprecision(3) << LatRecord; ss2 >> lat;
//         std::string command = "/home/yanlyu/Downloads/miniconda3/envs/py3/bin/python linesformatTojson.py "+lng+"_"+lat+"_"+std::to_string(iterNum)+"";

//         std::cout<< command << std::endl;
//         int res = system(command.c_str());
// }

