/**
  Created by Weng Y on 2020/05/25.
  Copyright © 2020 Weng Y. Under GNU General Public License v2.0.
*/

#include "VtuberDelegate.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "View.hpp"
#include "LAppPal.hpp"
#include "LAppDefine.hpp"
#include "LAppLive2DManager.hpp"
#include "LAppTextureManager.hpp" 
using namespace Csm;
using namespace std;
using namespace LAppDefine;

namespace {
VtuberDelegate *s_instance = NULL;

    static int VtuberCount = 0;
}

VtuberDelegate *VtuberDelegate::GetInstance()
{
    if (s_instance == NULL)
    {
		s_instance = new VtuberDelegate();
    }

    return s_instance;
}

void VtuberDelegate::ReleaseInstance()
{
    if (s_instance != NULL)
    {
        delete s_instance;
    }

    s_instance = NULL;
}

bool VtuberDelegate::LoadResource()
{
	if (!LAppPal::IsPathExist(ResourcesPath)) {
		return false;
	}
	ModelFileCount = LAppPal::GetAllDirName(ResourcesPath, ModelFileName);

	return true;
}

void VtuberDelegate::ReleaseResource() {

}

bool VtuberDelegate::Initialize()
{   VtuberCount++;
    if (!isINIT) {
	isINIT = true;
    if (DebugLogEnable)
    {
        LAppPal::PrintLog("START");
    }

    // GLFWの初期化
    if (glfwInit() == GL_FALSE)
    {
        if (DebugLogEnable)
        {
            LAppPal::PrintLog("Can't initilize GLFW");
        }
        return GL_FALSE;
    }
    
    // Windowの生成_
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    _window = glfwCreateWindow(RenderTargetWidth, RenderTargetHeight, "vtuber", NULL,
			       NULL);
    if (_window == NULL)
    {
        if (DebugLogEnable)
        {
            LAppPal::PrintLog("Can't create GLFW window.");
        }
        glfwTerminate();
        return GL_FALSE;
    }

    glfwMakeContextCurrent(_window);
    if (glewInit() != GLEW_OK) {
        if (DebugLogEnable)
        {
            LAppPal::PrintLog("Can't initilize glew.");
        }
        glfwTerminate();
        return GL_FALSE;
    }

    //AppViewの初期化
    _view->Initialize();

    // Cubism SDK の初期化
    InitializeCubism();
    }

    return GL_TRUE;
}

void VtuberDelegate::Release()
{   
    if(VtuberCount>1)
    {
        VtuberCount--;
    }
    else 
    {
        isINIT = false;

        glfwDestroyWindow(_window);

        glfwTerminate();

        delete _textureManager;

        delete _view;

        LAppLive2DManager::ReleaseInstance();

        CubismFramework::Dispose();

        CubismFramework::CleanUp();   

        ReleaseInstance();
    }  
}

void VtuberDelegate::Reader(char *buffer)
{
	if (Resizeflag) {
		Resizeflag = false;

		_view->Initialize();

		glViewport(0, 0, _windowWidth, _windowHeight);
	}
	LAppPal::UpdateTime();

	//描画更新
	_view->Render();
	
	glReadPixels(0, 0, _windowWidth, _windowHeight, GL_RGBA,GL_UNSIGNED_BYTE,buffer);
}

int VtuberDelegate::ModelCount()
{
	return ModelFileCount;
}

const char **VtuberDelegate::GetModelsName()
{
	return (const char**)ModelFileName;
}

void VtuberDelegate::Resize(int width, int height) {
	if (width != _windowWidth || height != _windowHeight) {
		Resizeflag = true;	
		_windowWidth = width;
		_windowHeight = height;
	}
}

void VtuberDelegate::SetDelayTime(double _delaytime) {
	if (isINIT)
		LAppLive2DManager::GetInstance()->SetDelayTime(_delaytime);
	delaytime = _delaytime;
}

void VtuberDelegate::SetRandomMotion(bool _Random_Motion) {
	
	if (isINIT && RandomMotion != _Random_Motion)
		LAppLive2DManager::GetInstance()->SetRandomMotion(_Random_Motion);
	RandomMotion = _Random_Motion;
}

void VtuberDelegate::ChangeModel(const char *ModelName)
{
	if (ModelName == NULL) {		
		return;
	}
		
	if (ModelFileCount > 0)
		for (int i = 0; i < ModelFileCount; i++) {
			if (strcmp(ModelName, ModelFileName[i]) == 0) {
				if (curentId != i) {
					LAppLive2DManager::GetInstance()->ChangeScene(i);
					curentId = i;
				}					
				break;
			}
		}	
}

VtuberDelegate::VtuberDelegate() :
    _cubismOption(),
	  _window(NULL),
	  isINIT(false),
	  Resizeflag(false),
	  ModelFileCount(0),
	  curentId(0),
	  RandomMotion(true),
	  delaytime(5.0)
{
    _view = new View();
    _textureManager = new LAppTextureManager();
}

VtuberDelegate::~VtuberDelegate() {}

void VtuberDelegate::InitializeCubism()
{
    _cubismOption.LogFunction = LAppPal::PrintMessage;
    _cubismOption.LoggingLevel = LAppDefine::CubismLoggingLevel;
    Csm::CubismFramework::StartUp(&_cubismAllocator, &_cubismOption);

    //Initialize cubism
    CubismFramework::Initialize();

    //load model
    LAppLive2DManager::GetInstance();

    //default proj
    CubismMatrix44 projection;

    LAppPal::UpdateTime();
}

GLuint VtuberDelegate::CreateShader()
{
    //バーテックスシェーダのコンパイル
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShader =
        "#version 120\n"
        "attribute vec3 position;"
        "attribute vec2 uv;"
        "varying vec2 vuv;"
        "void main(void){"
        "    gl_Position = vec4(position, 1.0);"
        "    vuv = uv;"
        "}";
    glShaderSource(vertexShaderId, 1, &vertexShader, NULL);
    glCompileShader(vertexShaderId);
    if(!CheckShader(vertexShaderId))
    {
        return 0;
    }

    //フラグメントシェーダのコンパイル
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShader =
        "#version 120\n"
        "varying vec2 vuv;"
        "uniform sampler2D texture;"
        "uniform vec4 baseColor;"
        "void main(void){"
        "    gl_FragColor = texture2D(texture, vuv) * baseColor;"
        "}";
    glShaderSource(fragmentShaderId, 1, &fragmentShader, NULL);
    glCompileShader(fragmentShaderId);
    if (!CheckShader(fragmentShaderId))
    {
        return 0;
    }

    //プログラムオブジェクトの作成
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    // リンク
    glLinkProgram(programId);

    glUseProgram(programId);

    return programId;
}

int VtuberDelegate::getBufferWidth()
{
    return _windowWidth;
}

int VtuberDelegate::getBufferHeight()
{
    return _windowHeight;
}

double VtuberDelegate::getScale()
{
	return Scale;
}

void VtuberDelegate::setScale(double _sc) {
	Scale = _sc;
}

double VtuberDelegate::GetX()
{
	return viewPoint_x;
}

void VtuberDelegate::SetX(double _x)
{
	viewPoint_x = _x;
}

double VtuberDelegate::GetY()
{
	return viewPoint_y;
}

void VtuberDelegate::SetY(double _y)
{
	viewPoint_y = _y;
}

bool VtuberDelegate::CheckShader(GLuint shaderId)
{
    GLint status;
    GLint logLength;
    glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
    if (logLength > 0)
    {
        GLchar* log = reinterpret_cast<GLchar*>(CSM_MALLOC(logLength));
        glGetShaderInfoLog(shaderId, logLength, &logLength, log);
        CubismLogError("Shader compile log: %s", log);
        CSM_FREE(log);
    }

    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        glDeleteShader(shaderId);
        return false;
    }

    return true;
}
