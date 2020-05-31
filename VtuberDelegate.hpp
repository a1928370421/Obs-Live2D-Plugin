/**
  Created by Weng Y on 2020/05/25.
  Copyright © 2020 Weng Y. Under GNU General Public License v2.0.
*/

#pragma once

#include <GL/glew.h>
#include "LAppAllocator.hpp"
#include <GLFW/glfw3.h>

#define MAXMODELCOUNT 1024

class View;
class LAppTextureManager;
/**
* @brief   アプリケーションクラス。
*   Cubism SDK の管理を行う。
*/
class VtuberDelegate {

	struct RenderInfo {

		double viewPoint_x;
		double viewPoint_y;
		int windowWidth;
		int windowHeight; 				
		double Scale;

		bool randomMotion;
		double delayTime;
	};

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
    bool LoadResource(int id);

    void ReleaseResource(int id);

    bool Initialize();

    void Release();

    GLuint CreateShader();


    int getBufferWidth(int id);

    int getBufferHeight(int id);

    double getScale(int id);

    double GetX(int id);

    double GetY(int id);

    void UpdataViewWindow(double _x,double _y,int _width, int _height,double _scale, int _id);


    View* GetView() { return _view; }

    LAppTextureManager* GetTextureManager() { return _textureManager; }


    /**
    * @brief 渲染一帧画面到指定缓冲
    */
    void Reader(int targatid, char *data, int bufferWidth, int bufferheight);


    void ChangeModel(const char *ModelName, int id);



    void updataModelSetting(bool _randomMotion,double _delayTime,int id);

    bool GetRandomMotion(int id);

    double GetDelayTime(int id);

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
    View* _view;                             ///< View情報
    LAppTextureManager* _textureManager;         ///< テクスチャマネージャー

    char *ModelFileName[MAXMODELCOUNT];		///模型文件夹的名称集合
    int ModelFileCount;				///模型数量

    RenderInfo _renderInfo[8];
    };
 

