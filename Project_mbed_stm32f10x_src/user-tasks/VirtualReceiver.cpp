/*
 * VirtualReceiver.cpp
 *
 *  Created on: 20/04/2015
 *      Author: raulMrello
 */

#include "VirtualReceiver.h"

//------------------------------------------------------------------------------------
//-- PRIVATE TYPEDEFS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
//-- PRIVATE DEFINITIONS -------------------------------------------------------------
//------------------------------------------------------------------------------------

/** Time to get a valid response after sending a commando (default: 5000 ms) */
# define TIME_TO_VALID_RESPONSE		5000

//------------------------------------------------------------------------------------
//-- STATIC FUNCTIONS ----------------------------------------------------------------
//------------------------------------------------------------------------------------


static uint8_t getCRC(uint8_t * buffer, uint8_t size){
	uint8_t result  = 0;
	for(uint8_t i=0 ; i<size; i++){
		result = result ^ buffer[i];
	}
	return result;
}

//------------------------------------------------------------------------------------
static bool checkCRC(uint8_t * buffer, uint8_t crc, uint8_t size){
	if(getCRC(buffer, size) == crc)
		return true;
	return false;
}

//------------------------------------------------------------------------------------
static void OnTopicUpdateCallback(void *subscriber, const char * topicname){
	VirtualReceiver *me = (VirtualReceiver*)subscriber;
	if(strcmp(topicname, "/keyb") == 0){
		me->notifyUpdate(VirtualReceiver::KEY_EV_READY);
		return;
	}
	if(strcmp(topicname, "/gps") == 0){
		me->notifyUpdate(VirtualReceiver::GPS_EV_READY);
		return;
	}
}


//------------------------------------------------------------------------------------
//-- PUBLIC FUNCTIONS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

VirtualReceiver::VirtualReceiver(osPriority prio, Serial *serial) {
	_serial = serial;
	_serial->baud(115200);
	_th = 0;
	_th = new Thread(&VirtualReceiver::task, this, prio);
}

//------------------------------------------------------------------------------------
VirtualReceiver::~VirtualReceiver() {
	// TODO Auto-generated destructor stub
}

//------------------------------------------------------------------------------------
Thread * VirtualReceiver::getThread() {
	return _th;
}

//------------------------------------------------------------------------------------
void VirtualReceiver::RxISRCallback(void){
	_th->signal_set(VR_EV_DATAREADY);
}

//------------------------------------------------------------------------------------
void VirtualReceiver::TxISRCallback(void){
	_th->signal_set(VR_EV_DATASENT);
}

//------------------------------------------------------------------------------------
void VirtualReceiver::notifyUpdate(uint32_t event){
	_th->signal_set(event);
}

//------------------------------------------------------------------------------------
//-- PROTECTED/PRIVATE FUNCTIONS -----------------------------------------------------
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
void VirtualReceiver::run(){
	while(_th == 0){
		Thread::wait(100);
	}
	// Attaches to serial peripheral
	_serial->attach(this, &VirtualReceiver::RxISRCallback, (SerialBase::IrqType)RxIrq);
	_serial->attach(this, &VirtualReceiver::TxISRCallback, (SerialBase::IrqType)TxIrq);
	
	// Attaches to topic updates
	MsgBroker::Exception e;
	MsgBroker::attach("/gps", this, OnTopicUpdateCallback, &e);
	MsgBroker::attach("/keyb", this, OnTopicUpdateCallback, &e);

	// starts waiting events...
	_protostat = STAT_WAIT_COMMAND;	
	_errcount = 0;
	_timeout = osWaitForever;
	for(;;){
		// Wait incoming data ... 
		osEvent oe = _th->signal_wait((VR_EV_DATAREADY | KEY_EV_READY | GPS_EV_READY), _timeout);
		// if timeout...
		if(oe.status == osEventTimeout){
			// if waiting response, active alarm ALARM_MISSING_RESPONSE
			if(_protostat >= STAT_WAIT_RESPONSE){
				if(_protostat == STAT_WAIT_RESPONSE){
					_alarmdata.alarm[0] = (uint8_t)Topic::ALARM_MISSING_RESPONSE;
				}
				else{
					_alarmdata.alarm[0] = (uint8_t)Topic::ALARM_WRONG_RESPONSE;
				}
				MsgBroker::publish("/alarm", &_alarmdata, sizeof(Topic::AlarmData_t), &e);
				_timeout = osWaitForever;
				_protostat = STAT_WAIT_COMMAND;
			}			
		}
		if(oe.status == osEventSignal && (oe.value.signals & VR_EV_DATAREADY) != 0){	
			_th->signal_clr(VR_EV_DATAREADY);						
			while(_serial->readable()){
				uint8_t cmd;
				// reads data and executes protocol state machine
				cmd = decodePdu((uint8_t)_serial->getc());
				switch(cmd){
					case CMD_ERROR:{
						// nothing done, if error persists, EV_TIMEOUT will raise later
						break;
					}
					case CMD_READY:{
						// if NACK received, transmission wan't understood, raise alarm and continue
						if((_rxpdu.data[0] & CMD_NACK) != 0){
						}
						// publish /stat topic
						else{
							memcpy(&_statdata, &_rxpdu.data[1], sizeof(Topic::StatusData_t));
							MsgBroker::publish("/stat", &_alarmdata, sizeof(Topic::AlarmData_t), &e);
							_timeout = osWaitForever;
							_protostat = STAT_WAIT_COMMAND;
						}
						break;
					}
					default: /*CMD_DECODING*/{
						// nothing done, decoding reception stream
						break;
					}
				}
			}
		}
		if(oe.status == osEventSignal && (oe.value.signals & KEY_EV_READY) != 0){		
			_th->signal_clr(KEY_EV_READY);						
			Topic::KeyData_t * keydata = (Topic::KeyData_t *)MsgBroker::getTopicData("/keyb", &e);
			memcpy(&_txpdu.data[1], keydata, sizeof(Topic::KeyData_t));
			MsgBroker::consumed("/keyb", &e);
			_txpdu.size = sizeof(Topic::KeyData_t)+1;
			_txpdu.data[0] = CMD_SET_KEY;
			send();		
			_timeout = TIME_TO_VALID_RESPONSE;
			_protostat = STAT_WAIT_RESPONSE;
		}
		if(oe.status == osEventSignal && (oe.value.signals & GPS_EV_READY) != 0){	
			_th->signal_clr(GPS_EV_READY);						
			Topic::GpsData_t * gpsdata = (Topic::GpsData_t *)MsgBroker::getTopicData("/gps", &e);
			memcpy(&_txpdu.data[1], gpsdata, sizeof(Topic::GpsData_t));
			MsgBroker::consumed("/gps", &e);
			_txpdu.size = sizeof(Topic::GpsData_t)+1;
			_txpdu.data[0] = CMD_SET_GPS;
			send();
			_timeout = TIME_TO_VALID_RESPONSE;
			_protostat = STAT_WAIT_RESPONSE;
		}
	}	
}

//------------------------------------------------------------------------------------
void VirtualReceiver::send(){
	// build tx pdu: head, crc
	_txpdu.head = HEAD_FLAG;
	_txpdu.data[_txpdu.size] = getCRC(&_txpdu.size, _txpdu.size + 1);
	// sends 
	for(int i=0;i<_txpdu.size+3;i++){
		_serial->putc(((char *)(&_txpdu))[i]);
	}
}

//------------------------------------------------------------------------------------
uint8_t VirtualReceiver::decodePdu(uint8_t data){
	static uint8_t datasize = 0;
	switch((int)_protostat){
		case STAT_WAIT_RESPONSE:{
			if(data != HEAD_FLAG){
				return CMD_ERROR;
			}
			_rxpdu.head = data;
			_protostat = STAT_RECV_HEAD;
			return CMD_DECODING;
		}
		case STAT_RECV_HEAD:{
			if(data >= MAX_SIZE){
				return CMD_ERROR;
			}
			_rxpdu.size = data;
			datasize = 0;
			_protostat = STAT_RECV_SIZE;
			return CMD_DECODING;
		}
		case STAT_RECV_SIZE:{
			_rxpdu.data[datasize++] = data;
			if(datasize >= _rxpdu.size){
				_protostat = STAT_RECV_DATA;
			}
			return CMD_DECODING;
		}
		case STAT_RECV_DATA:{
			_rxpdu.data[datasize] = data;
			if(checkCRC(&_rxpdu.size, _rxpdu.data[datasize], datasize+1)){
				return CMD_READY;
			}
			return CMD_ERROR;
		}
	}
	return CMD_ERROR;
}

