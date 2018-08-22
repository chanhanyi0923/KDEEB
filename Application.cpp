#include "Application.h"

#include <time.h>


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

    input >> this->dataSet;
/*
    const int frameSize = 100;

    frames.resize(frameSize);
    bitset<3000> state[frameSize], zero;

    size_t lineNum = 0;
    for (string stringLine; getline(input, stringLine); ) {
        stringstream lineBuffer(stringLine);
        string token;

        // split by ','
        float number[4];
        for (int i = 0; i < 4; i++) {
            getline(lineBuffer, token, ',');
            stringstream buffer(token);
            buffer >> number[i];
        }

        Line line;
        //vector<Record> records;
        line.AddPoint(Point(number[1], number[0], 1.0f));
        line.AddPoint(Point(number[3], number[2], 1.0f));
        this->dataSet.lines.push_back(line);

        for (int i = 0; i < frameSize; i ++) {
            int in;
            getline(lineBuffer, token, ',');
            stringstream buffer(token);
            buffer >> in;
            state[i].set(lineNum, (bool)in);
        }
        lineNum ++;
    }

    for (int i = 0; i < frameSize; i ++) {
        const auto & prev = i == 0 ? zero : state[i - 1];
        for (int j = 0; j < lineNum; j ++) {
            if (prev[j] != state[i][j]) {
                if (state[i][j]) {
                    this->frames[i].add.push_back(j);
                } else {
                    this->frames[i].remove.push_back(j);
                }
            }
        }
    }
*/

    //
    //

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
    std::fstream iterationConf;
    iterationConf.open("iteration.conf", std::fstream::in);
    if (!iterationConf) {
        throw RenderException("Configuration not found");
    }
    for (std::string line; getline(iterationConf, line); ) {
        std::stringstream buffer(line);

        int count, textureWidth;
        float kernelSize;
        double attractionFactor, smoothingFactor;
        std::string doResamplingString;
        bool doResampling;

        buffer >> count >> textureWidth >> kernelSize >> attractionFactor >> smoothingFactor >> doResamplingString;
        if (doResamplingString == "true") {
            doResampling = true;
        } else if (doResamplingString == "false") {
            doResampling = false;
        } else {
            throw RenderException("Failed to initialize GLEW");
        }
        this->iterationsForNonRealTime.push_back(Iteration(count, textureWidth, kernelSize, attractionFactor, smoothingFactor, doResampling));
    }
    iterationConf.close();
}


void Application::Run(const char * inputFile, const char * outputFile)
{
    clock_t start, end;
    start = clock();

    this->InitOpenGL();

    this->Config();

    //this->Input(inputFile);
    this->Input();

    this->NonRealTimeBundleWithWaypoint();
    //this->NonRealTimeBundle();
    this->Output(outputFile);
    //this->Output();

    this->TerminateOpenGL();

    end = clock();
    std::cout<< "whole total time: " << (double)(end-start)/CLOCKS_PER_SEC << std::endl;
}


void Application::NonRealTimeBundleWithWaypoint()
{
    using std::fstream;
    using std::string;
    using std::stringstream;

    this->InitBundle();

    for (int fileNum = 1; fileNum <= 35; fileNum ++) {


/*
        // load waypoints
        char filename[100];
        // others/Morphing/data/waypoints_1.txt
        sprintf(filename, "others/Morphing/data/13/waypoints_%d.txt", fileNum);

        fstream fin;
        fin.open(filename, fstream::in);
        for (string stringLine; getline(fin, stringLine); ) {
            stringstream buffer1(stringLine);
            int lineIndex, oIndex, dIndex, waypointIndex;
            string mincut;
            buffer1 >> lineIndex >> oIndex >> dIndex >> mincut;

            lineIndex --;
            oIndex --;
            dIndex --;

            Line & line = this->dataSet.lines[lineIndex];

            stringstream buffer2(mincut);
            for (string w; getline(buffer2, w, ','); ) {
                stringstream buffer3(w);
                buffer3 >> waypointIndex;
                waypointIndex --;

                const Point p = this->refPoints[waypointIndex];

                const size_t oId = line.FindPointIndex(this->refPoints[oIndex]);
                const size_t dId = line.FindPointIndex(this->refPoints[dIndex]);
                line.AddWaypoint(oId, dId, Waypoint(p.x, p.y));
            }
        }
        fin.close();


        // load routes
        sprintf(filename, "others/Morphing/data/13/routes_%d.txt", fileNum);

        fin.open(filename, fstream::in);
        for (string stringLine; getline(fin, stringLine); ) {
            stringstream buffer1(stringLine);
            int lineIndex, oIndex, dIndex;
            buffer1 >> lineIndex >> oIndex >> dIndex;

            lineIndex --;
            oIndex --;
            dIndex --;

            Line & line = this->dataSet.lines[lineIndex];
            const size_t oId = line.FindPointIndex(this->refPoints[oIndex]);
            const size_t dId = line.FindPointIndex(this->refPoints[dIndex]);
            line.AddSegment(oId, dId);
        }
        fin.close();
*/

        // test 2
        // load waypoints
        char filename[100];
        // others/Morphing/data/waypoints_1.txt
        sprintf(filename, "others/out/waypoints_%d.txt", fileNum);

        fstream fin;
        fin.open(filename, fstream::in);
        for (string stringLine; getline(fin, stringLine); ) {
            stringstream buffer1(stringLine);
            int lineIndex;
            double ox, oy, dx, dy;
            string mincut;
            buffer1 >> lineIndex >> ox >> oy >> dx >> dy;

            lineIndex --;

            Line & line = this->dataSet.lines[lineIndex];

            const size_t oId = line.FindPointIndex(Point(ox, oy, 0));
            const size_t dId = line.FindPointIndex(Point(dx, dy, 0));

            for (string w; getline(buffer1, w, ','); ) {
                stringstream buffer3(w);
                double waypointX, waypointY;
                buffer3 >> waypointX >> waypointY;

                line.AddWaypoint(oId, dId, Waypoint(waypointX, waypointY));
            }
        }
        fin.close();


        // load routes
        sprintf(filename, "others/out/routes_%d.txt", fileNum - 1);

        fin.open(filename, fstream::in);
        for (string stringLine; getline(fin, stringLine); ) {
            stringstream buffer1(stringLine);
            int lineIndex;
            double ox, oy, dx, dy;
            buffer1 >> lineIndex >> ox >> oy >> dx >> dy;
            lineIndex --;

            Line & line = this->dataSet.lines[lineIndex];

            const size_t oId = line.FindPointIndex(Point(ox, oy, 0));
            const size_t dId = line.FindPointIndex(Point(dx, dy, 0));

            line.AddSegment(oId, dId);
        }
        fin.close();


        // bundle
        //#pragma omp parallel for
        for (size_t i = 0; i < this->dataSet.lines.size(); i ++) {
            Line & line = this->dataSet.lines[i];

            if (line.GetWaypointSize() == 0) {
                line = Line();
            }
            // std::cout << "Line " << i << std::endl;
            line.UpdatePoints();
        }


        // Iteration(count, textureWidth, kernelSize, attractionFactor, smoothingFactor, doResampling))
        this->BundleWithWaypoint(Iteration(20, 200, 20, 0.1, 0.1, true));

        //#pragma omp parallel for
        for (size_t i = 0; i < this->dataSet.lines.size(); i ++) {
            Line & line = this->dataSet.lines[i];
            line.ClearWaypoints();
        }

    }
}


void Application::NonRealTimeBundle()
{
    this->InitBundle();
    for (const Iteration & iteration : this->iterationsForNonRealTime) {
        this->Bundle(iteration);
    }
}

/*
void Application::RealTimeBundle()
{
    this->InitBundle();
    DataSet * originalDataSet = new DataSet(this->dataSet);

    for (size_t i = 0; i < this->frames.size(); i ++) {
        std::cout << i << std::endl;
        const auto & frame = this->frames[i];

        for (const size_t index: frame.add) {
            this->dataSet.data[index] = originalDataSet->data[index];
        }
        for (const size_t index: frame.remove) {
            this->dataSet.data[index].clear();
        }
        // Iteration(count, textureWidth, kernelSize, attractionFactor, smoothingFactor, doResampling))
        this->Bundle(Iteration(10, 100, 20, 0.1, 0.1, true));

        // for debug
        std::stringstream buffer;
        buffer << "realtime_" << i;
        this->Output(buffer.str().c_str());
        //
    }
    delete originalDataSet;
}
*/

void Application::InitBundle()
{
    // initial shader
    this->shader.LoadFragmentShader("shader.frag");
    this->shader.LoadVertexShader("shader.vs");
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
        if (iteration.doResampling) {
            this->dataSet.AddRemovePoints(this->pointRemoveDistance, this->pointSplitDistant);
        }
    }
}


void Application::BundleWithWaypoint(const Iteration & iteration)
{
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
        this->dataSet.SmoothTrailsWithWaypoint(iteration.smoothingFactor);
        if (iteration.doResampling) {
            this->dataSet.AddRemovePointsWithWaypoint(this->pointRemoveDistance, this->pointSplitDistant);
        }

        if (i == iteration.count - 1) {
            this->dataSet.RemovePointsInSegment();
        }
        //
        static int c = 0;
        char s[100];
        sprintf(s, "others/test_out/%d.txt", c);
        this->Output(s);
        c ++;
        //
    }

}


