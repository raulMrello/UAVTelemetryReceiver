/*
 * SysManager.cpp
 *
 *  Created on: 20/04/2015
 *      Author: raulMrello
 */

#include "SysManager.h"
#include "Topics.h"

//------------------------------------------------------------------------------------
//-- PRIVATE TYPEDEFS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
//-- PRIVATE DEFINITIONS -------------------------------------------------------------
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
//-- STATIC FUNCTIONS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

static void OnTopicUpdateCallback(void *subscriber, const char * topicname){
	SysManager *me = (SysManager*)subscriber;
	if(strcmp(topicname, "/alarm") == 0){
		me->notifyUpdate(SysManager::ALARM_EV_READY);
		return;
	}
	if(strcmp(topicname, "/keyb") == 0){
		me->notifyUpdate(SysManager::KEY_EV_READY);
		return;
	}
	if(strcmp(topicname, "/gps") == 0){
		me->notifyUpdate(SysManager::GPS_EV_READY);
		return;
	}
}

//------------------------------------------------------------------------------------
//-- PUBLIC FUNCTIONS ----------------------------------------------------------------
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
SysManager::~SysManager() {

}

//------------------------------------------------------------------------------------
Thread * SysManager::getThread() {
	return _th;
}

//------------------------------------------------------------------------------------
void SysManager::notifyUpdate(uint32_t event){
	_th->signal_set(event);
}


//------------------------------------------------------------------------------------
void SysManager::run(){
	while(_th == 0){
		Thread::wait(100);
	}
	// attaches to topic updates 
	MsgBroker::Exception e;
	MsgBroker::attach("/gps", this, OnTopicUpdateCallback, &e);
	MsgBroker::attach("/keyb", this, OnTopicUpdateCallback, &e);
	MsgBroker::attach("/alarm", this, OnTopicUpdateCallback, &e);
	MsgBroker::attach("/stat", this, OnTopicUpdateCallback, &e);
	_timeout = osWaitForever;
	ledStop();
	beepStop();
	// Starts execution 
	for(;;){
		// Wait topic updates changes ... 
		osEvent oe = _th->signal_wait((GPS_EV_READY | KEY_EV_READY | ALARM_EV_READY | STAT_EV_READY), _timeout);
		// if /alarm topic update
		if(oe.status == osEventSignal && (oe.value.signals & ALARM_EV_READY) != 0){	
			_th->signal_clr(ALARM_EV_READY);
			beepStart(QUAD_SHOT, SHORT_TIME, REPEAT_FOREVER);
		}
		// if /keyb topic update, starts one short beep
		if(oe.status == osEventSignal && (oe.value.signals & KEY_EV_READY) != 0){
			_th->signal_clr(KEY_EV_READY);
			MsgBroker::consumed("/keyb", &e);
			beepStart(ONE_SHOT, SHORT_TIME, NO_REPEAT);
		}
		// if /gps topic update
		if(oe.status == osEventSignal && (oe.value.signals & GPS_EV_READY) != 0){
			_th->signal_clr(GPS_EV_READY);
			MsgBroker::consumed("/gps", &e);
			beepStart(DUAL_SHOT, SHORT_TIME, NO_REPEAT);
		}
		// if /stat topic update
		if(oe.status == osEventSignal && (oe.value.signals & STAT_EV_READY) != 0){
			_th->signal_clr(STAT_EV_READY);
			Topic::StatusData_t * statdata = (Topic::StatusData_t *)MsgBroker::getTopicData("/stat", &e);
			switch(statdata->mode){
				case Topic::MODE_DISARMED:{
					ledStart(LedFlasher::ON_FOREVER);	// ON forever
					break;
				}
				case Topic::MODE_MANUAL:{
					ledStart(LedFlasher::SLOW_FLASHING);	// 1s ON, 1s OFF, forever
					break;
				}
				default: { /* LOC, ALT, RTH combination flags */
					if((statdata->mode & Topic::MODE_LOC) != 0 ){
						ledStart(LedFlasher::SINGLE_FAST_FLASHING);	// 500ms ON, 1s OFF, forever
					}
					if((statdata->mode & Topic::MODE_ALT) != 0 ){
						ledStart(LedFlasher::DUAL_FAST_FLASHING);	// 500ms ON, 500mss OFF, twice-shots forever
					}
					if((statdata->mode & Topic::MODE_RTH) != 0 ){
						ledStart(LedFlasher::CONTINUOUS_FAST_FLASHING);	// 500ms ON, 500ms OFF, forever
					}
					break;
				}
			}
			MsgBroker::consumed("/stat", &e);
			beepStart(DUAL_SHOT, SHORT_TIME, NO_REPEAT);
		}
	}	
}

//------------------------------------------------------------------------------------
//-- PROTECTED/PRIVATE FUNCTIONS -----------------------------------------------------
//------------------------------------------------------------------------------------

