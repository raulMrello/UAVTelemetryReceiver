/*
 * LedFlasher.cpp
 *
 *  Created on: 20/04/2015
 *      Author: raulMrello
 */

#include "LedFlasher.h"

//------------------------------------------------------------------------------------
//-- PRIVATE TYPEDEFS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
//-- PRIVATE DEFINITIONS -------------------------------------------------------------
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
//-- STATIC FUNCTIONS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

static void ledTimer(void const *handler){
	LedFlasher * me = (LedFlasher*)handler;
	switch((int)me->_status){
		case LedFlasher::STAT_FLASH:{
			me->_status = LedFlasher::STAT_OFF;			
			me->_led->write(0);
			me->_tmr->start(me->_off_time);
			return;
		}
		case LedFlasher::STAT_OFF:{
			if(me->_wait_time == LedFlasher::TIME_DISABLED){
				me->_status = LedFlasher::STAT_FLASH;			
				me->_led->write(1);
				me->_tmr->start(me->_on_time);
			}
			else if(me->_mode == LedFlasher::SINGLE_FAST_FLASHING){
				me->_status = LedFlasher::STAT_WAIT;			
				me->_tmr->start(me->_wait_time);
			}
			else if(me->_shots == 0){
				me->_shots++;
				me->_status = LedFlasher::STAT_FLASH;			
				me->_led->write(1);
				me->_tmr->start(me->_on_time);
			}
			else{
				me->_shots = 0;
				me->_status = LedFlasher::STAT_WAIT;			
				me->_tmr->start(me->_wait_time);
			}
			break;
		}
		case LedFlasher::STAT_WAIT:{
			me->_status = LedFlasher::STAT_FLASH;			
			me->_led->write(1);
			me->_tmr->start(me->_on_time);
			break;
		}

	}
}

//------------------------------------------------------------------------------------
//-- PUBLIC FUNCTIONS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

LedFlasher::LedFlasher(DigitalOut *led) {
	_led = led;
	_led->write(0);
	_status = STOPPED;
	_tmr = new RtosTimer(ledTimer, osTimerPeriodic, this);
}

//------------------------------------------------------------------------------------
LedFlasher::~LedFlasher() {
	_tmr->stop();
	delete(_tmr);
}

//------------------------------------------------------------------------------------
void LedFlasher::ledStart(ShotModeEnum mode){
	_tmr->stop();
	_mode = mode;
	switch((int)_mode){
		case ON_FOREVER:{
			_status = STAT_FLASH;			
			_led->write(1);
			return;
		}
		case SLOW_FLASHING:{
			_status = STAT_FLASH;			
			_led->write(1);
			_on_time = TIME_SLOW;
			_off_time = TIME_SLOW;
			_wait_time = TIME_DISABLED;
			break;
		}
		case SINGLE_FAST_FLASHING:
		case DUAL_FAST_FLASHING:{
			_status = STAT_FLASH;			
			_led->write(1);
			_on_time = TIME_FAST;
			_off_time = TIME_FAST;
			_wait_time = TIME_WAIT;
			_shots = 0;
			break;
		}
		case CONTINUOUS_FAST_FLASHING:{
			_status = STAT_FLASH;			
			_led->write(1);
			_on_time = TIME_FAST;
			_off_time = TIME_FAST;
			_wait_time = TIME_DISABLED;
			break;
		}
	}
	_tmr->start(_on_time);
}


//------------------------------------------------------------------------------------
void LedFlasher::ledStop() {
	_tmr->stop();
	_status = STOPPED;
	_led->write(0);
}

//------------------------------------------------------------------------------------
//-- PROTECTED/PRIVATE FUNCTIONS -----------------------------------------------------
//------------------------------------------------------------------------------------

