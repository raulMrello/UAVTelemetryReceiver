/*
 * KeyDecoder.cpp
 *
 *  Created on: 20/04/2015
 *      Author: raulMrello
 */

#include "KeyDecoder.h"

//------------------------------------------------------------------------------------
//-- PRIVATE TYPEDEFS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
//-- PRIVATE DEFINITIONS -------------------------------------------------------------
//------------------------------------------------------------------------------------

/** Milliseconds timeout to publish a /key update when a key is pressed */
#define REPEAT_TIMEOUT	200

/** Keyboard codes */
#define KEY_NONE 0u
#define KEY_N	(uint32_t)(1 << 0)
#define KEY_NE	(uint32_t)(1 << 1)
#define KEY_E	(uint32_t)(1 << 2)
#define KEY_ES	(uint32_t)(1 << 3)
#define KEY_S	(uint32_t)(1 << 4)
#define KEY_SW	(uint32_t)(1 << 5)
#define KEY_W	(uint32_t)(1 << 6)
#define KEY_WN	(uint32_t)(1 << 7)
#define KEY_ARM	(uint32_t)(1 << 8)
#define KEY_LOC	(uint32_t)(1 << 9)
#define KEY_ALT	(uint32_t)(1 << 10)
#define KEY_RTH	(uint32_t)(1 << 11)

//------------------------------------------------------------------------------------
//-- STATIC FUNCTIONS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
//-- PUBLIC FUNCTIONS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

KeyDecoder::KeyDecoder(	osPriority prio, InterruptIn *ii_N, InterruptIn *ii_NE,
						InterruptIn *ii_E, InterruptIn *ii_ES, InterruptIn *ii_S,
						InterruptIn *ii_SW, InterruptIn *ii_W, InterruptIn *ii_WN,
						InterruptIn *ii_ARM, InterruptIn *ii_LOC, InterruptIn *ii_ALT,
						InterruptIn *ii_RTH) {

	ii_N->fall(this, &KeyDecoder::KeyPressedISRCallback);
	ii_N->rise(this, &KeyDecoder::KeyReleasedISRCallback);
	ii_N->disable_irq();
	ii_N->mode(PullUp);
	_ii_N = ii_N;

	ii_NE->fall(this, &KeyDecoder::KeyPressedISRCallback);
	ii_NE->rise(this, &KeyDecoder::KeyReleasedISRCallback);
	ii_NE->disable_irq();
	ii_NE->mode(PullUp);
	_ii_NE = ii_NE;
							
	ii_E->fall(this, &KeyDecoder::KeyPressedISRCallback);
	ii_E->rise(this, &KeyDecoder::KeyReleasedISRCallback);
	ii_E->disable_irq();
	ii_E->mode(PullUp);
	_ii_E = ii_E;

	ii_ES->fall(this, &KeyDecoder::KeyPressedISRCallback);
	ii_ES->rise(this, &KeyDecoder::KeyReleasedISRCallback);
	ii_ES->disable_irq();
	ii_ES->mode(PullUp);
	_ii_ES = ii_ES;

	ii_S->fall(this, &KeyDecoder::KeyPressedISRCallback);
	ii_S->rise(this, &KeyDecoder::KeyReleasedISRCallback);
	ii_S->disable_irq();
	ii_S->mode(PullUp);
	_ii_S = ii_S;

	ii_SW->fall(this, &KeyDecoder::KeyPressedISRCallback);
	ii_SW->rise(this, &KeyDecoder::KeyReleasedISRCallback);
	ii_SW->disable_irq();
	ii_SW->mode(PullUp);
	_ii_SW = ii_SW;

	ii_W->fall(this, &KeyDecoder::KeyPressedISRCallback);
	ii_W->rise(this, &KeyDecoder::KeyReleasedISRCallback);
	ii_W->disable_irq();
	ii_W->mode(PullUp);
	_ii_W = ii_W;

	ii_WN->fall(this, &KeyDecoder::KeyPressedISRCallback);
	ii_WN->rise(this, &KeyDecoder::KeyReleasedISRCallback);
	ii_WN->disable_irq();
	ii_WN->mode(PullUp);
	_ii_WN = ii_WN;

	ii_ARM->fall(this, &KeyDecoder::KeyPressedISRCallback);
	ii_ARM->rise(this, &KeyDecoder::KeyReleasedISRCallback);
	ii_ARM->disable_irq();
	ii_ARM->mode(PullUp);
	_ii_ARM = ii_ARM;

	ii_LOC->fall(this, &KeyDecoder::KeyPressedISRCallback);
	ii_LOC->rise(this, &KeyDecoder::KeyReleasedISRCallback);
	ii_LOC->disable_irq();
	ii_LOC->mode(PullUp);
	_ii_LOC = ii_LOC;

	ii_ALT->fall(this, &KeyDecoder::KeyPressedISRCallback);
	ii_ALT->rise(this, &KeyDecoder::KeyReleasedISRCallback);
	ii_ALT->disable_irq();
	ii_ALT->mode(PullUp);
	_ii_ALT = ii_ALT;

	ii_RTH->fall(this, &KeyDecoder::KeyPressedISRCallback);
	ii_RTH->rise(this, &KeyDecoder::KeyReleasedISRCallback);
	ii_RTH->disable_irq();
	ii_RTH->mode(PullUp);			
	_ii_RTH = ii_RTH;
	
	_th = 0;
	_th = new Thread(&KeyDecoder::task, this, prio);
}

//------------------------------------------------------------------------------------
KeyDecoder::~KeyDecoder() {
	_ii_N->disable_irq();
	_ii_NE->disable_irq();
	_ii_E->disable_irq();
	_ii_ES->disable_irq();
	_ii_S->disable_irq();
	_ii_SW->disable_irq();
	_ii_W->disable_irq();
	_ii_WN->disable_irq();
	_ii_ARM->disable_irq();
	_ii_LOC->disable_irq();
	_ii_ALT->disable_irq();
	_ii_RTH->disable_irq();
}

//------------------------------------------------------------------------------------
Thread * KeyDecoder::getThread() {
	return _th;
}

//------------------------------------------------------------------------------------
void KeyDecoder::KeyPressedISRCallback(void){
	_th->signal_set(KEY_EV_PRESSED);
}

//------------------------------------------------------------------------------------
void KeyDecoder::KeyReleasedISRCallback(void){
	_th->signal_set(KEY_EV_RELEASED);
}

//------------------------------------------------------------------------------------
void KeyDecoder::run(){
	while(_th == 0){
		Thread::wait(100);
	}
	// waits till all keys are released
	do{
		_currentkey = readKeyboard();
	}while(_currentkey != KEY_NONE);
	_lastkey = _currentkey;
	_ii_N->enable_irq();
	_ii_NE->enable_irq();
	_ii_E->enable_irq();
	_ii_ES->enable_irq();
	_ii_S->enable_irq();
	_ii_SW->enable_irq();
	_ii_W->enable_irq();
	_ii_WN->enable_irq();
	_ii_ARM->enable_irq();
	_ii_LOC->enable_irq();
	_ii_ALT->enable_irq();
	_ii_RTH->enable_irq();
	_timeout = osWaitForever;
	// Starts execution 
	for(;;){
		bool publish = false;
		// Wait input changes ... 
		osEvent oe = _th->signal_wait((KEY_EV_PRESSED |KEY_EV_RELEASED), _timeout);
		// debounce timer
		Thread::wait(50);
		// if key pressed, read keyboard, update topic and enable publishing and repeated events
		if(oe.status == osEventSignal && (oe.value.signals & (KEY_EV_PRESSED|KEY_EV_RELEASED)) != 0){		
			_th->signal_clr(KEY_EV_PRESSED|KEY_EV_RELEASED);
			_currentkey = readKeyboard();
			_keydata.data.keycode = (_currentkey ^ _lastkey);
			publish = true;			
			if(!_currentkey){
				_timeout = osWaitForever;
			}
			else{
			_timeout = REPEAT_TIMEOUT;
			}
		}
		// if repeated event, enables publishing
		if(oe.status == osEventTimeout){
			_currentkey = readKeyboard();
			publish = true;	
			if(!_currentkey){
				_timeout = osWaitForever;
			}
			else{
			_timeout = REPEAT_TIMEOUT;
			}
		}
		// if publishing enabled, publish topic update
		if(publish){
			MsgBroker::Exception e = MsgBroker::NO_ERRORS;
			MsgBroker::publish("/keyb", &_keydata, sizeof(Topic::KeyData_t), &e);
			if(e != MsgBroker::NO_ERRORS){
				// TODO: add error handling ...
			}
		}
	}	
}

//------------------------------------------------------------------------------------
//-- PROTECTED/PRIVATE FUNCTIONS -----------------------------------------------------
//------------------------------------------------------------------------------------

uint32_t KeyDecoder::readKeyboard(){
	uint32_t result = KEY_NONE;
	if(_ii_N->read() == 0)
		result |= KEY_N;
	if(_ii_NE->read() == 0)
		result |= KEY_NE;
	if(_ii_E->read() == 0)
		result |= KEY_E;
	if(_ii_ES->read() == 0)
		result |= KEY_ES;
	if(_ii_S->read() == 0)
		result |= KEY_S;
	if(_ii_SW->read() == 0)
		result |= KEY_SW;
	if(_ii_W->read() == 0)
		result |= KEY_W;
	if(_ii_WN->read() == 0)
		result |= KEY_WN;
	if(_ii_ARM->read() == 0)
		result |= KEY_ARM;
	if(_ii_LOC->read() == 0)
		result |= KEY_LOC;
	if(_ii_ALT->read() == 0)
		result |= KEY_ALT;
	if(_ii_RTH->read() == 0)
		result |= KEY_RTH;
	return result;
}

