
#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Math/CubismMatrix44.hpp>
#include <Math/CubismViewMatrix.hpp>
#include "CubismFramework.hpp"
#include <Rendering/OpenGL/CubismOffscreenSurface_OpenGLES2.hpp>

#define MAXVIEWDATA 1024

class LAppModel;

enum SelectTarget
    {
        SelectTarget_None,                ///< デフォルトのフレームバッファにレンダリング
        SelectTarget_ModelFrameBuffer,    ///< LAppModelが各自持つフレームバッファにレンダリング
        SelectTarget_ViewFrameBuffer,     ///< Viewの持つフレームバッファにレンダリング
    };

/**
* @brief 描画クラス
*/
struct ViewData {
	Csm::CubismViewMatrix *_viewMatrix;
	Csm::CubismMatrix44 *_deviceToScreen;
	SelectTarget target;
};

class View
{
public:

    View();

    ~View();

    void Initialize(int id);

    void Release(int id);

    void Render(int id);

    float TransformViewX(float deviceX,int id) const;

    float TransformViewY(float deviceY,int id) const;

    float TransformScreenX(float deviceX, int id) const;

    float TransformScreenY(float deviceY, int id) const;

    void PreModelDraw(LAppModel& refModel,int id);

    void PostModelDraw(LAppModel &refModel, int id);

    void SwitchRenderingTarget(SelectTarget targetType,int id);

    void SetRenderTargetClearColor(float r, float g, float b);

     Csm::CubismViewMatrix *GetViewMatrix(int id);

     Csm::CubismMatrix44 *GetDeviceToScreenMatrix(int id);

     uint16_t GetTotalViewer();


private:
    ViewData _viewData[MAXVIEWDATA];

    uint16_t dataCount;

    GLuint _programId;                       ///< シェーダID

    Csm::Rendering::CubismOffscreenFrame_OpenGLES2 _renderBuffer;   ///< モードによってはCubismモデル結果をこっちにレンダリング
    float _clearColor[4];           ///< レンダリングターゲットのクリアカラー
};
