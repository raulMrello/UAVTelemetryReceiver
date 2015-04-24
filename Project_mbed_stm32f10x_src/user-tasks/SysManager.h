/*
 * SysManager.h
 *
 *  Created on: 20/4/2015
 *      Author: raulMrello
 *
 *	SysManager is an active module (Task) which main function is system management. This
 *	agent will attach to /gps, /keyb, /alarm topic updates to carry out management operations
 *	accordingly. These are its main functions:
 *		- On /gps topic it will start a short dual beep-beep.
 *		- On /keyb topic it will start a short single beep.
 *		- On /alarm topic it will start a short burst_x5 beeps until user interaction
 */

#ifndef SRC_ACTIVE_MODULES_SYSMANAGER_H_
#define SRC_ACTIVE_MODULES_SYSMANAGER_H_


//------------------------------------------------------------------------------------
//-- DEPENDENCIES --------------------------------------------------------------------
//------------------------------------------------------------------------------------

#include "mbed.h"
#include "rtos.h"
#include "MsgBroker.h"
#include "BeepGenerator.h"
#include "LedFlasher.h"

//------------------------------------------------------------------------------------
//-- TYPEDEFS ------------------------------------------------------------------------
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
//-- CLASS ---------------------------------------------------------------------------
//------------------------------------------------------------------------------------

class SysManager : public BeepGenerator, public LedFlasher {
public:
	
	/** Constructor, destructor, getter and setter */
	SysManager(	osPriority prio, DigitalOut *led, PwmOut *buzzer) : BeepGenerator(buzzer), LedFlasher(led) {
		_th = 0;
		_th = new Thread(&SysManager::task, this, prio);
	}
	
	virtual ~SysManager();
	Thread *getThread();
		
	/** Topic updates callbacks */
	void notifyUpdate(uint32_t event);

	/** Topic updates event enumeration */
	typedef enum {
		GPS_EV_READY 	= (1 << 0),
		KEY_EV_READY 	= (1 << 1),
		ALARM_EV_READY 	= (1 << 2),
		STAT_EV_READY 	= (1 << 3),
	}EventEnum;	
	
	/** Task */
	static void task(void const *arg){
		SysManager *me = (SysManager*)arg;
		me->run();
	}	

private:
	Thread *_th;
	uint32_t _timeout;

	void run();

};


#endif /* SRC_ACTIVE_MODULES_SYSMANAGER_H_ */
