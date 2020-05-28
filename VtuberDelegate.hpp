/**
  Created by Weng Y on 2020/05/25.
  Copyright © 2020 Weng Y. Under GNU General Public License v2.0.
*/

#pragma once

#include <GL/glew.h>
#include "LAppAllocator.hpp"
#include <GLFW/glfw3.h>

#define MAXMODELCOUNT 10

class LAppView;
class LAppTextureManager;
/**
* @brief   アプリケーションクラス。
*   Cubism SDK の管理を行う。
*/
class VtuberDelegate {
public:
    /**
    * @brief   クラスのインスタンス（シングルトン）を返す。<br>
    *           インスタンスが生成されていない場合は内部でインスタンを生成する。
    *
    * @return  クラスのインスタンス
    */
	static VtuberDelegate *GetInstance();

	static void ReleaseInstance();

    /*
    *@breif 不处理opengl相关的初始化
    */
    bool LoadResource(uint16_t windowWidth,uint16_t windowHeight);


    void ReleaseResource();

    bool Initialize();

    void Release();

    GLuint CreateShader();

    int getBufferWidth();

    int getBufferHeight();

    double getScale();

    void setScale(double _sc);

    double GetX();

    void SetX(double _x);

    double GetY();

    void SetY(double _y);

    LAppView* GetView() { return _view; }

    LAppTextureManager* GetTextureManager() { return _textureManager; }

    /**
    * @brief 创建一个帧缓冲并且绑定
    */
    void creatFrameBuffer();

     /**
    * @brief 释放帧缓冲
    */
    void releaseFrameBuffer();

    /**
    * @brief 渲染一帧画面到指定缓冲
    */
    void Reader(char* data);

    /**
    * @brief 返回模型数
    */
    int ModelCount();

    void ChangeModel(int i);

    const char** GetModelsName();

    void Resize(int width, int height);

private:
    /**
    * @brief   コンストラクタ
    */
	VtuberDelegate();

    /**
    * @brief   デストラクタ
    */
	~VtuberDelegate();

    /**
    * @brief   Cubism SDK の初期化
    */
    void InitializeCubism();

    /**
     * @brief   CreateShader内部関数 エラーチェック
     */
    bool CheckShader(GLuint shaderId);

    LAppAllocator _cubismAllocator;              ///< Cubism SDK Allocator
    Csm::CubismFramework::Option _cubismOption;  ///< Cubism SDK Option
    GLFWwindow* _window;                         ///< OpenGL ウィンドウ
    LAppView* _view;                             ///< View情報
    LAppTextureManager* _textureManager;         ///< テクスチャマネージャー

    int _windowWidth;                            ///< Initialize関数で設定したウィンドウ幅
    int _windowHeight;                           ///< Initialize関数で設定したウィンドウ高さ

    bool isINIT;

    
    GLuint fbuffer;				///帧缓冲指针

    char *ModelFileName[MAXMODELCOUNT];		///模型文件夹的名称集合
    int ModelFileCount;				///模型数量

    double Scale;

    double viewPoint_x;
    double viewPoint_y;

    bool Resizeflag;
    };
 

