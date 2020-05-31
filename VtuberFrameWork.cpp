/**
  Created by Weng Y on 2020/05/25.
  Copyright © 2020 Weng Y. Under GNU General Public License v2.0.
*/
#include "VtuberFrameWork.hpp"
#include"VtuberDelegate.hpp"
#include<mutex>

namespace {
    static bool isLoad;

    static bool isInit;

    static int initCount = 0;

    static std::mutex mut;
}

void VtuberFrameWork::InitVtuber(int id)
{
    mut.lock();
    initCount++;
    if (initCount == 0) {
	isInit = false;
    }   
    isLoad = VtuberDelegate::GetInstance()->LoadResource(id);
    mut.unlock();
}

void VtuberFrameWork::ReanderVtuber(int targatid, char *data, int bufferWidth,
				    int bufferheight)
{
    mut.lock();

    if (isLoad) {
    
    if (initCount == 0 && isInit) {
        VtuberDelegate::GetInstance()->Release();
	goto end;
    }
    if (initCount>0&&!isInit) {
	    isInit = true;
	    VtuberDelegate::GetInstance()->Initialize();
    }

    if (isInit)
	VtuberDelegate::GetInstance()->Reader(targatid, data, bufferWidth,bufferheight);

    }
    end:
    mut.unlock();
}

void VtuberFrameWork::UinitVtuber(int id)
{
    mut.lock();    
    initCount--;
    VtuberDelegate::GetInstance()->ReleaseResource(id);
    mut.unlock();
}

void VtuberFrameWork::UpData(int id,double _x, double _y, int width, int height,
			     double sc,double _delayTime, bool _randomMotion,bool _break,bool _eyeBlink,const char *modelPath)
{
	mut.lock();

	VtuberDelegate::GetInstance()->UpdataViewWindow(_x,_y,width, height,sc, id);
	VtuberDelegate::GetInstance()->updataModelSetting(
		_randomMotion, _delayTime, _break,_eyeBlink,id);
	VtuberDelegate::GetInstance()->ChangeModel(modelPath,id);

	mut.unlock();
}

int VtuberFrameWork::GetWidth(int id)
{    
    return VtuberDelegate::GetInstance()->getBufferWidth(id);
}

int VtuberFrameWork::GetHeight(int id)
{
	return VtuberDelegate::GetInstance()->getBufferHeight(id);
}

double VtuberFrameWork::GetScale(int id)
{
	return VtuberDelegate::GetInstance()->getScale(id);
}

double VtuberFrameWork::GetX(int id)
{
	return VtuberDelegate::GetInstance()->GetX(id);
}

double VtuberFrameWork::GetY(int id)
{
	return VtuberDelegate::GetInstance()->GetY(id);
}





