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

    // input >> this->dataSet;
    const int frameSize = 100;

    frames.resize(frameSize);
    bitset<3000> state[frameSize], zero;

    size_t lineNum = 0;
    for (string line; getline(input, line); ) {
        stringstream lineBuffer(line);
        string token;

        // split by ','
        float number[4];
        for (int i = 0; i < 4; i++) {
            getline(lineBuffer, token, ',');
            stringstream buffer(token);
            buffer >> number[i];
        }
        vector<Record> records;
        records.push_back(Record(number[1], number[0], 1.0f));
        records.push_back(Record(number[3], number[2], 1.0f));
        this->dataSet.data.push_back(records);

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
/*
    for (int i = 0; i < 2; i ++) {
        const Record & r = this->dataSet.data[i][0];
        std::cout << r.x << " " << r.y << " " << r.z << std::endl;
    }
    for (int i = 0; i < 10; i ++) {
        std::cout << "Add: " << std::endl;
        for (int j = 0; j < this->frames[i].add.size() && j < 10; j ++) {
            std::cout << (int)this->frames[i].add[j] << std::endl;
        }

        std::cout << "Remove: " << std::endl;
        for (int j = 0; j < this->frames[i].remove.size() && j < 10; j ++) {
            std::cout << (int)this->frames[i].remove[j] << std::endl;
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

    this->Input(inputFile);
    //this->Input();

    this->RealTimeBundle();
    this->Output(outputFile);
    //this->Output();

    this->TerminateOpenGL();

    end = clock();
    std::cout<< "whole total time: " << (double)(end-start)/CLOCKS_PER_SEC << std::endl;
}


void Application::NonRealTimeBundle()
{
    this->InitBundle();
    for (const Iteration & iteration : this->iterationsForNonRealTime) {
        this->Bundle(iteration);
    }
}


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

    for (int i = 0; i < iteration.count; i++) {
        Image image(dataSet, &this->shader);
        image.Init();

        vector<float> accD = framebuffer.ComputeSplatting(iteration.kernelSize, this->dataSet, image);
        this->gradient.ApplyGradient(this->dataSet, accD, textureWidth, iteration.attractionFactor, this->obstacleRadius);
        this->dataSet.SmoothTrails(iteration.smoothingFactor);
        if (iteration.doResampling) {
            this->dataSet.AddRemovePoints(this->pointRemoveDistance, this->pointSplitDistant);
        }
    }
}


