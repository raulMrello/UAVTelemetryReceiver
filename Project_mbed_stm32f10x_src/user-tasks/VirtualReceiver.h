/*
 * VirtualReceiver.h
 *
 *  Created on: 20/04/2015
 *      Author: raulMrello
 *
 *  VirtualReceiver is an Active module (running as a Task) that will virtualize a 
 *	remote Telemetry Receiver. It will manage Serial communications with a wifi modem
 *	to establish a connection with the remote Telemetry Receiver. Its basic functions are:
 *
 *	1 - Configure and setup communications with the remote peer.
 *	2 - Attach to /gps and /keyb topic updates and pass data to the wifi modem
 *	3 - Convert serial responses into /alarm, /stat topics for other modules
 */

#ifndef SRC_ACTIVE_MODULES_VIRTUALRECEIVER_H_
#define SRC_ACTIVE_MODULES_VIRTUALRECEIVER_H_


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

class VirtualReceiver{
public:
	
	
	/** Constructor, destructor, getter and setter */
	VirtualReceiver(osPriority prio, Serial *serial);
	virtual ~VirtualReceiver();
	Thread *getThread();
	
	/** Serial Interrupt callbacks */
	void RxISRCallback(void);
	void TxISRCallback(void);
		
	/** Topic updates callbacks */
	void notifyUpdate(uint32_t event);

	/** Topic updates event enumeration */
	typedef enum {
		GPS_EV_READY 	= (1 << 0),
		KEY_EV_READY 	= (1 << 1),
		VR_EV_DATAREADY = (1 << 2),
		VR_EV_DATASENT  = (1 << 3)
	}EventEnum;	
		
	/** Task */
	static void task(void const *arg){
		VirtualReceiver *me = (VirtualReceiver*)arg;
		me->run();
	}	

private:
	
	/** Data protocol head flag and max size */
	static const uint8_t HEAD_FLAG = 0x5A;
	static const uint8_t MAX_SIZE = 32;

	/** Data protocol command types */
	static const uint8_t CMD_SET_GPS 	= 0x01;
	static const uint8_t CMD_SET_KEY 	= 0x02;
	static const uint8_t CMD_DECODING	= 0x03;
	static const uint8_t CMD_ERROR		= 0x04;
	static const uint8_t CMD_READY		= 0x05;
	static const uint8_t CMD_ACK		= 0x00;	// on response _rxpdu.data[0] = CMD_ACK/NACK | OPERATION
	static const uint8_t CMD_NACK		= 0x80;	// on response _rxpdu.data[0] = CMD_ACK/NACK | OPERATION
	/** Data protocol state machine */
	enum ProtocolStat{
		STAT_WAIT_COMMAND,
		STAT_WAIT_RESPONSE,
		STAT_RECV_HEAD,
		STAT_RECV_SIZE,
		STAT_RECV_DATA
	};
	/** Data protocol structure */
	struct PDU{
		uint8_t head;
		uint8_t size;
		uint8_t data[MAX_SIZE];	//data[0]=>Command, data[1..N-1]=>PDU, data[N]=>CRC
	};
	
	/** Private variables */
	Serial *_serial;
	Thread *_th;
	uint32_t _timeout;
	uint8_t _errcount;
	Topic::AlarmData_t _alarmdata;
	Topic::StatusData_t _statdata;
	PDU _rxpdu;
	PDU _txpdu;
	ProtocolStat _protostat;

	void run();
	void send();
	uint8_t decodePdu(uint8_t data);

};


#endif /* SRC_ACTIVE_MODULES_VIRTUALRECEIVER_H_ */
