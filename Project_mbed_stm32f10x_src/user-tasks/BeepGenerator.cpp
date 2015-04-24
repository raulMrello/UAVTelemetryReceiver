/*
 * BeepGenerator.cpp
 *
 *  Created on: 20/04/2015
 *      Author: raulMrello
 */

#include "BeepGenerator.h"

//------------------------------------------------------------------------------------
//-- PRIVATE TYPEDEFS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
//-- PRIVATE DEFINITIONS -------------------------------------------------------------
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
//-- STATIC FUNCTIONS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

static void beepTimer(void const *handler){
	BeepGenerator * me = (BeepGenerator*)handler;
	switch((int)me->_status){
		case BeepGenerator::STOPPED:{
			me->_tmr->stop();
			break;
		}
		case BeepGenerator::BEEP:{
			me->_buzzer->period_us(0);
			me->_status = BeepGenerator::SILENCE;
			break;
		}
		case BeepGenerator::SILENCE:{
			if(--me->_shots == 0){
				if(me->_repeat == BeepGenerator::NO_REPEAT){
					me->_tmr->stop();
					me->_status = BeepGenerator::STOPPED;
					return;
				}
				if(me->_repeat == BeepGenerator::REPEAT_FOREVER){
					me->_tmr->stop();
					me->_tmr->start(9 * BeepGenerator::SHORT_TIME);
					me->_status = BeepGenerator::WAIT_CYCLE;
					return;
				}
			}
			break;
		}
		case BeepGenerator::WAIT_CYCLE:{
			me->_shots = me->_mode;
			me->_status = BeepGenerator::BEEP;
			me->_buzzer->period_us(125);
			me->_tmr->start(me->_time);
			break;
		}
	}
}

//------------------------------------------------------------------------------------
//-- PUBLIC FUNCTIONS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

BeepGenerator::BeepGenerator(PwmOut *buzzer) {
	_buzzer = buzzer;
	_buzzer->period_us(250);
	_buzzer->pulsewidth_us(0);
	_status = STOPPED;
	_tmr = new RtosTimer(beepTimer, osTimerPeriodic, this);
}

//------------------------------------------------------------------------------------
BeepGenerator::~BeepGenerator() {
	_tmr->stop();
	delete(_tmr);
}

//------------------------------------------------------------------------------------
void BeepGenerator::beepStart(ShotModeEnum mode, ShotTimeEnum time, ShotRepeatEnum repeat){
	_tmr->stop();
	_mode = mode;
	_time = time;
	_repeat = repeat;
	_buzzer->period_us(125);
	_status = BEEP;
	_shots = _mode;
	_tmr->start(_time);
}


//------------------------------------------------------------------------------------
void BeepGenerator::beepStop() {
	_tmr->stop();
	_buzzer->period_us(0);
	_status = STOPPED;
	_shots = 0;
}

//------------------------------------------------------------------------------------
//-- PROTECTED/PRIVATE FUNCTIONS -----------------------------------------------------
//------------------------------------------------------------------------------------

