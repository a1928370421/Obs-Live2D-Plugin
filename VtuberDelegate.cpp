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

bool VtuberDelegate::LoadResource(int id)
{
	if (!LAppPal::IsPathExist(ResourcesPath)) {
		return false;
	}
	ModelFileCount = LAppPal::GetAllDirName(ResourcesPath, ModelFileName);
	return true;
}

void VtuberDelegate::ReleaseResource(int id) {
	  LAppLive2DManager::GetInstance()->ReleaseModel(id);
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
    _view->Initialize(0);
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

void VtuberDelegate::Reader(int id,char *buffer,int bufferWidth, int bufferheight)
{
	if (_renderInfo[id].Resizeflag) {
		_renderInfo[id].Resizeflag = false;
		_view->Initialize(id);
		glViewport(0, 0,_renderInfo[id]._windowWidth,_renderInfo[id]._windowHeight);
	}
	LAppPal::UpdateTime();
	
	//描画更新
	_view->Render(id);
	
	glReadPixels(0, 0, _renderInfo[id]._windowWidth,
		     _renderInfo[id]._windowHeight, GL_RGBA, GL_UNSIGNED_BYTE,
		     buffer);
}

int VtuberDelegate::ModelCount()
{
	return ModelFileCount;
}

const char **VtuberDelegate::GetModelsName()
{
	return (const char**)ModelFileName;
}

void VtuberDelegate::Resize(int width, int height, int id)
{
	if (width != _renderInfo[id]._windowWidth ||
	    height != _renderInfo[id]._windowHeight) {
		_renderInfo[id].Resizeflag = true;	
		_renderInfo[id]._windowWidth = width/32*32;
		_renderInfo[id]._windowHeight = height;

		_view->Initialize(id);
	} else {
		_renderInfo[id].Resizeflag = false;
	}
}

void VtuberDelegate::SetDelayTime(double _delaytime, int id)
{
	_renderInfo[id].delaytime = _delaytime;
}

double VtuberDelegate::GetDelayTime(int id)
{
	return _renderInfo[id].delaytime;
}

uint16_t VtuberDelegate::GetSceneInx(int id)
{
	return _renderInfo[id].SceneIdx;
}


void VtuberDelegate::SetRandomMotion(bool _Random_Motion, int id)
{
	
	if (isINIT && _renderInfo[id].RandomMotion != _Random_Motion)
		_renderInfo[id].RandomMotion = _Random_Motion;
}

bool VtuberDelegate::GetRandomMotion(int id)
{
	return _renderInfo[id].RandomMotion;
}

void VtuberDelegate::ChangeModel(const char *ModelName, int id)
{
	if (ModelName == NULL) {
		return;
	}
		
	if (ModelFileCount > 0)
		for (int i = 0; i < ModelFileCount; i++) {
			if (strcmp(ModelName, ModelFileName[i]) == 0) {
				if (_renderInfo[id].SceneIdx != i) {
					LAppLive2DManager::GetInstance()
						->ChangeScene(i, id);
					_renderInfo[id].SceneIdx = i;
				}					
				break;
			}
		}	
}

VtuberDelegate::VtuberDelegate()
	: _cubismOption(),
	  _window(NULL),
	  isINIT(false),
	  ModelFileCount(0)
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

int VtuberDelegate::getBufferWidth(int id)
{
	return _renderInfo[id]._windowWidth;
}

int VtuberDelegate::getBufferHeight(int id)
{
	return _renderInfo[id]._windowHeight;
}

double VtuberDelegate::getScale(int id)
{
	return _renderInfo[id].Scale;
}

void VtuberDelegate::setScale(double _sc, int id)
{
	_renderInfo[id].Scale = _sc;
}

double VtuberDelegate::GetX(int id)
{
	return _renderInfo[id].viewPoint_x;
}

void VtuberDelegate::SetX(double _x, int id)
{
	_renderInfo[id].viewPoint_x = _x;
}

double VtuberDelegate::GetY(int id)
{
	return _renderInfo[id].viewPoint_y;
}

void VtuberDelegate::SetY(double _y, int id)
{
	_renderInfo[id].viewPoint_y = _y;
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
