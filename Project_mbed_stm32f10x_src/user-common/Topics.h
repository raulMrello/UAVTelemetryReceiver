/* 
 * Topics.h
 *
 *  Created on: 20/4/2015
 *      Author: raulMrello
 *
 *	Topics data structure description file
 */

#ifndef SRC_COMMON_TOPICS_H_
#define SRC_COMMON_TOPICS_H_

namespace Topic {	


	//------------------------------------------------------------------------------------
	/** Gps topic data structure */
	struct GpsData_t{
		float lat;	///< latitude -100º to +100º
		float lng;	///< longitude -12h to +12h
		float alt;	///< altitude -100m to 8000m
		float hea;	///< heading 0º to 360º
	};


	//------------------------------------------------------------------------------------
	/** Alarm topic data structure */
	struct AlarmData_t{
		uint8_t alarm[8];
	};

	
	/** Alarm error code enumeration */
	enum AlarmCodeEnum {
		ALARM_MISSING_RESPONSE = 0,		///< No response from wifi link
		ALARM_WRONG_RESPONSE = 0,		///< Response bad formed from wifi link
	};


	//------------------------------------------------------------------------------------
	/** Status topic data structure */
	struct StatusData_t{
		uint8_t mode;
		GpsData_t location;
	};
	
	/** Status mode enumeration */
	enum StatusCodeEnum {
		MODE_DISARMED 	= 0,
		MODE_LOC 		= (1 << 0),
		MODE_MANUAL		= (1 << 1),
		MODE_ALT		= (1 << 2),
		MODE_RTH		= (1 << 3)
	};

	//------------------------------------------------------------------------------------
	/** Keyboard topic data structure */
	struct KeyData_t{
		union {
			uint32_t keycode;
			struct {
				unsigned key_N	:1; //bit 0
				unsigned key_NE	:1;
				unsigned key_E	:1;
				unsigned key_ES	:1;
				unsigned key_S	:1;
				unsigned key_SW	:1;
				unsigned key_W	:1;
				unsigned key_WN	:1;
				unsigned key_ARM:1;
				unsigned key_LOC:1;
				unsigned key_ALT:1;
				unsigned key_RTH:1;
				unsigned keys_UNUSED:21;
			}keys;
		}data;	
	};


}

#endif  /* SRC_COMMON_TOPICS_H_ */
