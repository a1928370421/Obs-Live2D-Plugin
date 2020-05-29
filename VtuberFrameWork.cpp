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
}

void VtuberFrameWork::InitVtuber()
{    
    isFirst = true;
    isEnd = false; 

    isLoad = VtuberDelegate::GetInstance()->LoadResource();
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

void VtuberFrameWork::UpData(double _x, double _y, int width, int height,
			     double sc,double _delayTime, bool randomMotion,const char *modelName)
{
	mut.lock();

	VtuberDelegate::GetInstance()->SetX(_x);
	VtuberDelegate::GetInstance()->SetY(_y);
	VtuberDelegate::GetInstance()->Resize(width, height);
	VtuberDelegate::GetInstance()->setScale(sc);
	VtuberDelegate::GetInstance()->ChangeModel(modelName);
	VtuberDelegate::GetInstance()->SetDelayTime(_delayTime);
	VtuberDelegate::GetInstance()->SetRandomMotion(randomMotion);

	mut.unlock();
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

double VtuberFrameWork::GetScale()
{
	return VtuberDelegate::GetInstance()->getScale();
}

double VtuberFrameWork::GetX()
{
	return VtuberDelegate::GetInstance()->GetX();
}

double VtuberFrameWork::GetY()
{
	return VtuberDelegate::GetInstance()->GetY();
}





