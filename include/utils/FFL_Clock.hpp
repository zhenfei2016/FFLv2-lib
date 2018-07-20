/*
 *  This file is part of FFL project.
 *
 *  The MIT License (MIT)
 *  Copyright (C) 2017-2018 zhufeifei All rights reserved.
 *
 *  FFL_Clock.hpp
 *  Created by zhufeifei(34008081@qq.com) on 2018/04/29
 *  https://github.com/zhenfei2016/FFLv2-lib.git
 *
 *  时钟类，可以获取当前时间，改变时钟速度，修改时钟原点偏移
 *
 */
#ifndef _FFL_CLOCK_HPP_
#define _FFL_CLOCK_HPP_

#include <FFL.h>
#include <ref/FFL_Ref.hpp>

namespace FFL {
    class IClockListener;

	class TimeBase {
	public:
		inline TimeBase() :mNum(1), mDen(1) {
		}
		inline TimeBase(int32_t num, int32_t den):mNum(num),mDen(den){
		}
		int32_t mNum;
		int32_t mDen;
	};

    class Clock : public RefBase{
    public:
        Clock();
        virtual ~Clock();
        
		void reset();
        
		IClockListener* setListener(IClockListener* listener);
        //
        //  这个时钟的当前时间
		//
        int64_t nowUs();
		//
		//  这个时钟的当前时间  systemUs：返回系统时钟的时间
		//
		int64_t nowUs(int64_t* systemUs);

		//
		//  转换当前时钟的一个时间段转成系统时钟时间段
		//  例如可以把当前时钟5us转成系统时钟8us
		//
		int64_t clockToWorldTimeBucket(int64_t dy);
		int64_t SystemToClockRelativeUs(int64_t dx);

		int64_t worldToClockUs(int64_t x);


        //
        //  时钟速度，默认100时正常时钟值
        //
        uint32_t speed();
		//
		//  正常速度的百分比
		//  0.1倍速  ： percent =1
		//  正常速度 ： percent =100
		//  2倍速    ： percent =200
		//
        void setSpeed(uint32_t percent);
        //
        //  偏移
        //
        int64_t offset();
        void setOffset(int64_t offset);        
		public:
        //
        //  是否系统时钟一致的
        //
        bool equalSystemClock();
	private:
		//
		//  y=ax + b + offset
		//
        int32_t mA;
        int64_t mB;
        //
        // 系统时间偏移值
        //
        int64_t mOffset;
        
        IClockListener* mListener;
	};
    
    class IClockListener {
    public:
        
        //
        //  type : 速度增加，不变，减少 1，0，-1
        //  speed :当前的速度
        //
        virtual void onSpeedChange(int32_t type,int32_t speed)=0;
    };
}


#endif

