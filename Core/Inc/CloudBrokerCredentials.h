/*
 * CloudBrokerCredentials.h
 *
 *  Created on: Nov 11, 2022
 *      Author: nokeeffe
 */

#ifndef INC_CLOUDBROKERCREDENTIALS_H_
#define INC_CLOUDBROKERCREDENTIALS_H_

/*
 * Add host name DNS here
 * For Adafruit use "io.adafruit.com"
 * For Ubidots use "industrial.api.ubidots.com"
 */
#define CloudBroker_HostName "io.adafruit.com"
//#define CloudBroker_HostName "io.adafruit.com"
/*
 * Port = "1883" for unencrypted communication
 */
#define CloudBroker_Port "1883"

/*
 * For Adafruit use your username
 * For Ubidots use your device token
 */
//#define CloudBroker_Username "nokeeffe"
#define CloudBroker_Username "g00409592"

/*
 * For Adafruit use "your active key
 * For Ubidots use an empty password ""
 */
#define CloudBroker_Password "aio_JzsE33mjBivyW0cUjJRK1tLVbewM"
//#define CloudBroker_Password "aio_otlP359Ckvb2gGDYiApu8b54GG3d"


/*
 * For Adafruit use a random ID
 * For Ubidots use your Device ID
 */
#define CloudBroker_ClientID "STM32NOK625"
//#define CloudBroker_ClientID "STM32NOK625"


#endif /* INC_CLOUDBROKERCREDENTIALS_H_ */
