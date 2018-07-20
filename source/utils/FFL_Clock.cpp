/*
*  This file is part of FFL project.
*
*  The MIT License (MIT)
*  Copyright (C) 2017-2018 zhufeifei All rights reserved.
*
*  FFL_Clock.cpp
*  Created by zhufeifei(34008081@qq.com) on 2018/04/29
*  https://github.com/zhenfei2016/FFLv2-lib.git
*
*  时钟类，可以获取当前时间，改变时钟速度，修改时钟原点偏移
*  y=ax +b
*/

#include <utils/FFL_Clock.hpp>
namespace FFL {
	//
	//   y=(a/KA_RATIO) x +b
	//  当前设置的a参数的放大倍数，实际的值= a/KA_RATIO
	//
	//
	static const int64_t KA_RATIO = 100;

    //
    //   y=x*a /KA_RATIO
    //
	static int64_t getY(int64_t x, int64_t a) {
		return (int64_t)((double)(x * a) / KA_RATIO);
	}

	Clock::Clock():mListener(NULL){
		reset();
	}
	Clock::~Clock() {

	}
	void Clock::reset() {
		mB = 0;
		mA = KA_RATIO;
	}

	IClockListener* Clock::setListener(IClockListener* listener){
		IClockListener* ret = mListener;
        mListener=listener;
		return ret;
    }
	//
	//  当前时间
	//
	int64_t Clock::nowUs() {
		return nowUs(0);
	}
	//
	//  这个时钟的当前时间  systemUs：返回系统时钟的时间
	//
	int64_t Clock::nowUs(int64_t* systemUs) {
		int64_t now = FFL_getNowUs();
		if (systemUs) {
			*systemUs = now;
		}
		if (equalSystemClock()) {
			return now;
		}

		//
		//  转化到当前时钟的时间值
		//  y=ax +b
		//	
		int64_t clockNow = getY(now, mA) + mB;
		FFL_LOG_INFO("Clock x:%" lld64 " y:%" lld64 "a=%" lld64 " b=%" lld64, now, clockNow, mA, mB);
		return clockNow;

	}
	//
	//  转换当前时钟的一个时间段转成系统时钟时间段
	//  例如可以把当前时钟5分钟转成系统时钟8分钟
	//
	int64_t Clock::clockToWorldTimeBucket(int64_t dy) {
		//
		// y=ax+b
		// 计算dx
		//
		return (int64_t)(double)(dy * 100) / mA;
	}
	int64_t Clock::SystemToClockRelativeUs(int64_t dx) {
		//
		// y=ax+b
		// 计算dy
		//
		return (int64_t)((double)(dx *  mA) / 100);
	}
	int64_t Clock::worldToClockUs(int64_t x) {
		//
		// y=ax+b
		// 计算dy
		//
		return (int64_t)getY(x, mA) + mB;
	}
	//
	//  时钟速度，默认1.0时正常时钟值
	//
	uint32_t Clock::speed() {
		return mA;
	}
	void Clock::setSpeed(uint32_t percent) {
		if (percent != mA) {
			int64_t x;
			int64_t y = nowUs(&x);

			//
			//  计算新的a,b参数
			//  b=y-ax
			//
            uint32_t oldA=mA;
			mA = percent;
			mB = y - (int64_t)((double)(mA* x) / KA_RATIO);
            
            if(mListener){
                int32_t type=0;
                if(percent>oldA)
                    type=1;
                else if(percent<oldA)
                    type=-1;
                mListener->onSpeedChange(type, mA);
            }
		}
	}
	//
	//  偏移
	//
	int64_t Clock::offset() {
		return mOffset;
	}
	void Clock::setOffset(int64_t offset) {
		mOffset = offset;
	}
    //
	//  是否系统时钟一致的
	//
	bool Clock::equalSystemClock() {
		return mA == KA_RATIO;
	}

}
