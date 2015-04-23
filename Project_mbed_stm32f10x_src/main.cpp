#include "mbed.h"
#include "rtos.h"
#include "GpsReader.h"
#include "KeyDecoder.h"
#include "SysManager.h"
#include "VirtualReceiver.h"
#include "MsgBroker.h"
#include "Topics.h"

InterruptIn key_N(PA_4);
InterruptIn key_NE(PA_5);
InterruptIn key_E(PA_6);
InterruptIn key_ES(PA_7);
InterruptIn key_S(PB_1);
InterruptIn key_SW(PB_2);
InterruptIn key_W(PB_9);
InterruptIn key_WN(PB_10);
InterruptIn key_ARM(PB_11);
InterruptIn key_LOC(PB_12);
InterruptIn key_ALT(PB_13);
InterruptIn key_RTH(PB_14);

DigitalOut led(PA_8);
PwmOut buzzer(PB_0);

Serial gps(PA_2,PA_3);
Serial lnk(PA_9,PA_10);

int main() {
	
	/** Setup peripherals */
	
	gps.baud(9600);
	lnk.baud(9600);
	
	key_N.mode(PullUp);
	key_NE.mode(PullUp);
	key_E.mode(PullUp);
	key_ES.mode(PullUp);
	key_S.mode(PullUp);
	key_SW.mode(PullUp);
	key_W.mode(PullUp);
	key_WN.mode(PullUp);
	key_ARM.mode(PullUp);
	key_LOC.mode(PullUp);
	key_ALT.mode(PullUp);
	key_RTH.mode(PullUp);
	
	led.write(0);	
	buzzer.period_us(250);
	buzzer.pulsewidth_us(0);
	
	/** generate bip-bip start */
	
	for(int i=0;i<2;i++){
		buzzer.pulsewidth_us(125);
		wait(0.2);
		buzzer.pulsewidth_us(0);
		wait(0.2);
	}
	
	/** Setup Message Broker and topics */
	
	MsgBroker::init();
	MsgBroker::installTopic("/gps", sizeof(Topic::GpsData_t));
	MsgBroker::installTopic("/keyb", sizeof(Topic::KeyData_t));
	MsgBroker::installTopic("/alarm", sizeof(Topic::AlarmData_t));
	MsgBroker::installTopic("/stat", sizeof(Topic::StatusData_t));
	
	/** Start scheduling */
	
	osStatus osStat = osKernelInitialize();
	osStat = osKernelStart();
	
	/** Start tasks */
	
	KeyDecoder *kd = new KeyDecoder(osPriorityHigh, &key_N, &key_NE, &key_E, &key_ES,
									&key_S, &key_SW, &key_W, &key_WN, &key_ARM, &key_LOC,
									&key_ALT, &key_RTH);
	GpsReader *gr = new GpsReader(osPriorityAboveNormal, GpsReader::GPS_MODE_UBX, &gps);
	SysManager *sm = new SysManager(osPriorityNormal, &led, &buzzer);
	
	/** main is lowest priority task, hence manage low power conditions */
    while(1) {
        /** Low power management */
    }
	
}
