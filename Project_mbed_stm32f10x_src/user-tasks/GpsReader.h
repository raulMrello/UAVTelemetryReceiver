/*
 * GpsReader.h
 *
 *  Created on: 15/4/2015
 *      Author: raulMrello
 *
 *  GpsReader is an Active module (running as a Task) that will read NMEA/UBX data
 *	streams from a serial GPS module. Each time data is received and analysed, a topic
 *	data structure (type "/gps") is updated and published.
 */

#ifndef SRC_ACTIVE_MODULES_GPSREADER_H_
#define SRC_ACTIVE_MODULES_GPSREADER_H_


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

class GpsReader{
public:
	/** Gps mode reading enumeration */
	typedef enum {
		GPS_MODE_NMEA = (1 << 0),
		GPS_MODE_UBX  = (1 << 1)
	}ModeEnum;
		
	/** Constructor, destructor, getter and setter */
	GpsReader(osPriority prio, GpsReader::ModeEnum mode, Serial *serial);
	virtual ~GpsReader();
	Thread *getThread();
	
	/** Serial Interrupt callbacks */
	void RxISRCallback(void);
	void TxISRCallback(void);
	
	/** Task */
	static void task(void const *arg){
		GpsReader *me = (GpsReader*)arg;
		me->run();
	}	

private:
	uint32_t _mode;
	Topic::GpsData_t _gpsdata;
	Serial *_serial;
	Thread *_th;
	void run();

	/** Gps event enumeration */
	typedef enum {
		GPS_EV_DATAREADY = (1 << 0),
		GPS_EV_DATASENT  = (1 << 1)
	}EventEnum;

};


#endif /* SRC_ACTIVE_MODULES_GPSREADER_H_ */
