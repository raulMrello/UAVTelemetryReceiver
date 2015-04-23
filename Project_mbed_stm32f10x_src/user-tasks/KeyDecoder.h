/*
 * KeyDecoder.h
 *
 *  Created on: 20/4/2015
 *      Author: raulMrello
 *
 *  KeyDecoder is an Active module (running as a Task) that will decode keyboard input
 *	combination by interrupts. Each time an interrupt is decode, a topic data structure
 *	(type "/keyb") is updated and published. It can recognise 3 different events: pressed,
 *	repeated and released.
 */

#ifndef SRC_ACTIVE_MODULES_KEYDECODER_H_
#define SRC_ACTIVE_MODULES_KEYDECODER_H_


//------------------------------------------------------------------------------------
//-- DEPENDENCIES --------------------------------------------------------------------
//------------------------------------------------------------------------------------

#include "mbed.h"
#include "rtos.h"
#include "MsgBroker.h"
#include "Topics.h"

//------------------------------------------------------------------------------------
//-- TYPEDEFS ------------------------------------------------------------------------
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
//-- CLASS ---------------------------------------------------------------------------
//------------------------------------------------------------------------------------

class KeyDecoder{
public:
	
	/** Constructor, destructor, getter and setter */
	KeyDecoder(	osPriority prio, 
				InterruptIn *ii_N,
				InterruptIn *ii_NE,
				InterruptIn *ii_E,
				InterruptIn *ii_ES,
				InterruptIn *ii_S,
				InterruptIn *ii_SW,
				InterruptIn *ii_W,
				InterruptIn *ii_WN,
				InterruptIn *ii_ARM,
				InterruptIn *ii_LOC,
				InterruptIn *ii_ALT,
				InterruptIn *ii_RTH);
	
	virtual ~KeyDecoder();
	Thread *getThread();
	
	/** Input Interrupt callbacks */
	void KeyPressedISRCallback(void);
	void KeyReleasedISRCallback(void);
	
	/** Task */
	static void task(void const *arg){
		KeyDecoder *me = (KeyDecoder*)arg;
		me->run();
	}	

private:
	Topic::KeyData_t _keydata;
	Thread *_th;
	InterruptIn *_ii_N;
	InterruptIn *_ii_NE;
	InterruptIn *_ii_E;
	InterruptIn *_ii_ES;
	InterruptIn *_ii_S;
	InterruptIn *_ii_SW;
	InterruptIn *_ii_W;
	InterruptIn *_ii_WN;
	InterruptIn *_ii_ARM;
	InterruptIn *_ii_LOC;
	InterruptIn *_ii_ALT;
	InterruptIn *_ii_RTH;
	uint32_t _lastkey;
	uint32_t _currentkey;
	uint32_t _timeout;


	void run();
	uint32_t readKeyboard();

	/** Key event enumeration */
	typedef enum {
		KEY_EV_PRESSED 	= (1 << 0),
		KEY_EV_RELEASED = (1 << 1)
	}EventEnum;	

};


#endif /* SRC_ACTIVE_MODULES_KEYDECODER_H_ */
