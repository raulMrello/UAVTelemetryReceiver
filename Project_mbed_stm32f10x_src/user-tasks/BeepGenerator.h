/*
 * BeepGenerator.h
 *
 *  Created on: 20/4/2015
 *      Author: raulMrello
 *
 *	BeepGenerator is an periodic module (RtosTimerTask) that generates a beep sound 
 *	on a PwmOut pin. It can manage repeated short-long beeps.
 */

#ifndef SRC_ACTIVE_MODULES_BEEPGENERATOR_H_
#define SRC_ACTIVE_MODULES_BEEPGENERATOR_H_


//------------------------------------------------------------------------------------
//-- DEPENDENCIES --------------------------------------------------------------------
//------------------------------------------------------------------------------------

#include "mbed.h"
#include "rtos.h"

//------------------------------------------------------------------------------------
//-- TYPEDEFS ------------------------------------------------------------------------
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
//-- CLASS ---------------------------------------------------------------------------
//------------------------------------------------------------------------------------

class BeepGenerator{
public:
	
	/** Shot mode enumeration */
	enum ShotModeEnum{
		ONE_SHOT  = 1,	///< One shot beep
		DUAL_SHOT = 2,	///< Two shots beep
		QUAD_SHOT = 4	///< Four shots beep
	};
	
	/** Beep time duration in milliseconds */
	enum ShotTimeEnum{
		SHORT_TIME = 200,
		LONG_TIME  = 1000
	};
	
	/** Beep repeating mode */
	enum ShotRepeatEnum {
		NO_REPEAT,		///< No repeat (one shot)		
		REPEAT_FOREVER	///< Repeat forever
	};
	
	/** Beep state */
	enum BeepStateEnum{
		STOPPED,
		BEEP,
		SILENCE,
		WAIT_CYCLE
	};

	/** Constructor, destructor, getter and setter */
	BeepGenerator(PwmOut *buzzer);
	
	virtual ~BeepGenerator();
	
	void beepStart(ShotModeEnum mode, ShotTimeEnum time, ShotRepeatEnum repeat);
	void beepStop();

	PwmOut *_buzzer;
	ShotModeEnum _mode;
	ShotTimeEnum _time;
	ShotRepeatEnum _repeat;
	BeepStateEnum _status;
	RtosTimer *_tmr;
	uint8_t _shots;
};


#endif /* SRC_ACTIVE_MODULES_BEEPGENERATOR_H_ */
