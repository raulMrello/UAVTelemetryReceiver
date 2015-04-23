/*
 * LedFlasher.h
 *
 *  Created on: 21/4/2015
 *      Author: raulMrello
 *
 *	LedFlasher is a periodic module (RtosTimerTask) that generates a led flashing shot 
 *	on a DigitalOut pin. It can manage repeated short-long flashing shots.
 */

#ifndef SRC_ACTIVE_MODULES_LEDFLASHER_H_
#define SRC_ACTIVE_MODULES_LEDFLASHER_H_


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

class LedFlasher{
public:
	
	/** Shot mode enumeration */
	enum ShotModeEnum{
		ON_FOREVER,					///< Always lighting
		SLOW_FLASHING,				///< Slow (1sec,1sec) flashing
		SINGLE_FAST_FLASHING,		///< Rapid (500ms, 1s) flashing
		DUAL_FAST_FLASHING,			///< Rapid (500ms, 500ms) dual-shot flashing
		CONTINUOUS_FAST_FLASHING	///< Rapid continuous flashing 500ms
	};
	
	/** Shot status enumeration */
	enum ShotStatusEnum{
		STOPPED,		///< Switched off and stopped
		STAT_FLASH,		///< Switched on
		STAT_OFF,		///< Switched off
		STAT_WAIT		///< Waiting next cycle
	};
	
	/** Shot time enumeration */
	enum ShotTimeEnum{
		TIME_DISABLED = 0,
		TIME_FAST = 500,		///< 500ms
		TIME_SLOW = 1000,		///< 1s
		TIME_WAIT = 1500		///< 1.5
	};
	
	/** Constructor, destructor, getter and setter */
	LedFlasher(DigitalOut *led);
	
	virtual ~LedFlasher();
	
	void ledStart(ShotModeEnum mode);
	void ledStop();

	DigitalOut *_led;
	ShotModeEnum _mode;
	ShotTimeEnum _on_time, _off_time, _wait_time;
	ShotStatusEnum _status;
	RtosTimer *_tmr;
	uint8_t _shots;
};


#endif /* SRC_ACTIVE_MODULES_LEDFLASHER_H_ */
