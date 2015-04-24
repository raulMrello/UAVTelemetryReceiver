/*
 * GpsReader.cpp
 *
 *  Created on: 14/04/2015
 *      Author: raulMrello
 */

#include "GpsReader.h"

//------------------------------------------------------------------------------------
//-- PRIVATE TYPEDEFS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
//-- PRIVATE DEFINITIONS -------------------------------------------------------------
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
//-- STATIC FUNCTIONS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
//-- PUBLIC FUNCTIONS ----------------------------------------------------------------
//------------------------------------------------------------------------------------

GpsReader::GpsReader(osPriority prio, GpsReader::ModeEnum mode, Serial *serial) {
	_mode = (uint32_t)mode;
	_serial = serial;
	_serial->baud(9600);
	_th = 0;
	_th = new Thread(&GpsReader::task, this, prio);
}

//------------------------------------------------------------------------------------
GpsReader::~GpsReader() {
	// TODO Auto-generated destructor stub
}

//------------------------------------------------------------------------------------
Thread * GpsReader::getThread() {
	return _th;
}

//------------------------------------------------------------------------------------
void GpsReader::RxISRCallback(void){
	_th->signal_set(GPS_EV_DATAREADY);
}

//------------------------------------------------------------------------------------
void GpsReader::TxISRCallback(void){
	_th->signal_set(GPS_EV_DATASENT);
}

//------------------------------------------------------------------------------------
void GpsReader::run(){
	while(_th == 0){
		Thread::wait(100);
	}
	// Attaches to installed serial peripheral
	_serial->attach(this, &GpsReader::RxISRCallback, (SerialBase::IrqType)RxIrq);
	_serial->attach(this, &GpsReader::TxISRCallback, (SerialBase::IrqType)TxIrq);
	
	// Starts execution 
	for(;;){
		// Wait incoming data forever ... 
		_th->signal_wait(GPS_EV_DATAREADY, osWaitForever);
		_th->signal_clr(GPS_EV_DATAREADY);
		while(_serial->readable()){
			char data;
			bool ready = false;
			// reads data and pass to gps processor
			data = (char) _serial->getc();
			#warning TODO: ready = libgps_ProcessData(&_gpsdata, data);					
			// if data ready, publish "/gps" topic 
			if(ready){
				ready = false;
				MsgBroker::Exception e = MsgBroker::NO_ERRORS;
				MsgBroker::publish("/gps", &_gpsdata, sizeof(Topic::GpsData_t), &e);
				if(e != MsgBroker::NO_ERRORS){
					// TODO: add error handling ...
				}
			}
		}
	}	
}

//------------------------------------------------------------------------------------
//-- PROTECTED/PRIVATE FUNCTIONS -----------------------------------------------------
//------------------------------------------------------------------------------------


