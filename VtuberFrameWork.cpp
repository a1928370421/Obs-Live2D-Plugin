/**
  Created by Weng Y on 2020/05/25.
  Copyright © 2020 Weng Y. Under GNU General Public License v2.0.
*/
#include "VtuberFrameWork.hpp"
#include"VtuberDelegate.hpp"
#include<mutex>

namespace {
    static bool isLoad;

    static bool isFirst;

    static bool isEnd;

    static std::mutex mut;

    static int ModelInx;

    static bool isChangeModel;
}

void VtuberFrameWork::InitVtuber(int bufferWidth, int bufferHeight)
{    
    isFirst = true;
    isEnd = false; 
    isChangeModel = false;
    ModelInx = 0;
    isLoad = VtuberDelegate::GetInstance()->LoadResource(bufferWidth,bufferHeight);
}

void VtuberFrameWork::ReanderVtuber(char * data)
{
    mut.lock();

    if (isLoad) {
    
    if (isEnd) {      
        VtuberDelegate::GetInstance()->Release();
        isEnd = false;
    }

    if (isFirst) {
	    VtuberDelegate::GetInstance()->Initialize();
        isFirst = false;
    }

    if (isChangeModel) {
	    VtuberDelegate::GetInstance()->ChangeModel(ModelInx);
        isChangeModel = false;
    }    
    VtuberDelegate::GetInstance()->Reader(data);

    }
    mut.unlock();
}

void VtuberFrameWork::UinitVtuber()
{
    isEnd = true;
    if (isLoad)
	    VtuberDelegate::GetInstance()->ReleaseResource();
}

int VtuberFrameWork::GetWidth()
{    
    return VtuberDelegate::GetInstance()->getBufferWidth();
}

int VtuberFrameWork::GetHeight()
{
	return VtuberDelegate::GetInstance()->getBufferHeight();
}

const char** VtuberFrameWork::GetModelFileName()
{
	return VtuberDelegate::GetInstance()->GetModelsName();
}

int VtuberFrameWork::ModelNum()
{
	return VtuberDelegate::GetInstance()->ModelCount();
}

void VtuberFrameWork::ChangeModel(int i)
{
    ModelInx = i;
    isChangeModel = true;
}

void VtuberFrameWork::SetScale(double sc) {
	VtuberDelegate::GetInstance()->setScale(sc);
}

double VtuberFrameWork::GetScale()
{
	return VtuberDelegate::GetInstance()->getScale();
}

double VtuberFrameWork::GetX()
{
	return VtuberDelegate::GetInstance()->GetX();
}

void VtuberFrameWork::SetX(double _x)
{
	VtuberDelegate::GetInstance()->SetX(_x);
}

double VtuberFrameWork::GetY()
{
	return VtuberDelegate::GetInstance()->GetY();
}

void VtuberFrameWork::SetY(double _y)
{
	VtuberDelegate::GetInstance()->SetY(_y);
}

void VtuberFrameWork::Resize(int width, int height) {
	VtuberDelegate::GetInstance()->Resize(width,height);
}


