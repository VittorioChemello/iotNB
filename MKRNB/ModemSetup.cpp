/*> AT 
OK

> AT+CFUN?

+CFUN: 1

OK

> AT+CFUN=1

OK

> AT+CPIN?

+CPIN: READY
OK

> AT+CEREG=1

OK

> AT+COPS=0

OK
> AT+CEREG?

+CEREG: 1,5

OK

+CEREG: 2

+CEREG: 3

+CEREG: 5

> AT+CGDCONT= 1,"IP","iot.1nce.net","10.209.106.8",0,0,0,0


*/

enum {
    READY_STATE_SET_ERROR_DISABLED,                      
    READY_STATE_WAIT_SET_ERROR_DISABLED,        
    READY_STATE_SET_MINIMUM_FUNCTIONALITY_MODE, 
    READY_STATE_WAIT_SET_MINIMUM_FUNCTIONALITY_MODE,  
    READY_STATE_CHECK_SIM,                      
    READY_STATE_WAIT_CHECK_SIM_RESPONSE,
    READY_STATE_UNLOCK_SIM,
    READY_STATE_WAIT_UNLOCK_SIM_RESPONSE,
    READY_STATE_DETACH_DATA,
    READY_STATE_WAIT_DETACH_DATA,
    READY_STATE_SET_PREFERRED_MESSAGE_FORMAT,
    READY_STATE_WAIT_SET_PREFERRED_MESSAGE_FORMAT_RESPONSE,
    READY_STATE_SET_HEX_MODE,
    READY_STATE_WAIT_SET_HEX_MODE_RESPONSE,
    READY_STATE_SET_AUTOMATIC_TIME_ZONE,
    READY_STATE_WAIT_SET_AUTOMATIC_TIME_ZONE_RESPONSE,
    READY_STATE_SET_APN,
    READY_STATE_WAIT_SET_APN,
    READY_STATE_SET_APN_AUTH,
    READY_STATE_WAIT_SET_APN_AUTH,
    READY_STATE_SET_FULL_FUNCTIONALITY_MODE,
    READY_STATE_WAIT_SET_FULL_FUNCTIONALITY_MODE,
    READY_STATE_SET_REG_POLICY,                       //aggiunte
    READY_STATE_WAIT_SET_REG_POLICY,                  //aggiunte
    READY_STATE_AUTOMATIC_NETWORK_SELECTION,          //aggiunte
    READY_STATE_WAIT_AUTOMATIC_NETWORK_SELECTION,     //aggiunte
    READY_STATE_CHECK_REGISTRATION,
    READY_STATE_WAIT_CHECK_REGISTRATION_RESPONSE,
    READY_STATE_DONE
};

#include "arduino.h"
#include "NB.h"

class ModemSetup : public NB {
    public:

    NB_NetworkStatus_t ready();

}

NB_NetworkStatus_t ModemSetup::ready()
{
if (_state == NB_ERROR) {
return 2;
}

int ready = MODEM.ready();

if (ready == 0) {
return 0;
}

switch (_readyState) {
case READY_STATE_SET_ERROR_DISABLED: {
    MODEM.send("AT+CMEE=0");
    _readyState = READY_STATE_WAIT_SET_ERROR_DISABLED;
    ready = 0;
    break;
}

case READY_STATE_WAIT_SET_ERROR_DISABLED: {
    if (ready > 1) {
    _state = NB_ERROR;
    ready = 2;
    } else {
    _readyState = READY_STATE_SET_MINIMUM_FUNCTIONALITY_MODE;
    ready = 0;
    }
    
    break;
}

case READY_STATE_SET_MINIMUM_FUNCTIONALITY_MODE: {
    MODEM.send("AT+CFUN=0");
    _readyState = READY_STATE_WAIT_SET_MINIMUM_FUNCTIONALITY_MODE;
    ready = 0;
    break;
}

case READY_STATE_WAIT_SET_MINIMUM_FUNCTIONALITY_MODE:{
    if (ready > 1) {
    _state = NB_ERROR;
    ready = 2;
    } else {
    _readyState = READY_STATE_CHECK_SIM;
    ready = 0;
    }

    break;
}

case READY_STATE_CHECK_SIM: {
    MODEM.setResponseDataStorage(&_response);
    MODEM.send("AT+CPIN?");
    _readyState = READY_STATE_WAIT_CHECK_SIM_RESPONSE;
    ready = 0;
    break;
}

case READY_STATE_WAIT_CHECK_SIM_RESPONSE: {
    if (ready > 1) {
    // error => retry
    _readyState = READY_STATE_CHECK_SIM;
    ready = 0;
    } else {
    if (_response.endsWith("READY")) {
        _readyState = READY_STATE_SET_PREFERRED_MESSAGE_FORMAT;
        ready = 0;
    } else if (_response.endsWith("SIM PIN")) {
        _readyState = READY_STATE_UNLOCK_SIM;
        ready = 0;
    } else {
        _state = NB_ERROR;
        ready = 2;
    }
    }

    break;
}

case READY_STATE_UNLOCK_SIM: {
    if (_pin != NULL) {
    MODEM.setResponseDataStorage(&_response);
    MODEM.sendf("AT+CPIN=\"%s\"", _pin);

    _readyState = READY_STATE_WAIT_UNLOCK_SIM_RESPONSE;
    ready = 0;
    } else {
    _state = NB_ERROR;
    ready = 2;
    }
    break;
}

case READY_STATE_WAIT_UNLOCK_SIM_RESPONSE: {
    if (ready > 1) {
    _state = NB_ERROR;
    ready = 2;
    } else {
    _readyState = READY_STATE_DETACH_DATA;
    ready = 0;
    }

    break;
}

case READY_STATE_DETACH_DATA: {
    MODEM.send("AT+CGATT=0");
    _readyState = READY_STATE_WAIT_DETACH_DATA;
    ready = 0;
    break;
}

case READY_STATE_WAIT_DETACH_DATA:{
    if (ready > 1) {
    _state = NB_ERROR;
    ready = 2;
    } else {
    _readyState = READY_STATE_SET_PREFERRED_MESSAGE_FORMAT;
    ready = 0;
    }

    break;
}

case READY_STATE_SET_PREFERRED_MESSAGE_FORMAT: {
    MODEM.send("AT+CMGF=1");
    _readyState = READY_STATE_WAIT_SET_PREFERRED_MESSAGE_FORMAT_RESPONSE;
    ready = 0;
    break;
}

case READY_STATE_WAIT_SET_PREFERRED_MESSAGE_FORMAT_RESPONSE: {
    if (ready > 1) {
    _state = NB_ERROR;
    ready = 2;
    } else {
    _readyState = READY_STATE_SET_HEX_MODE;
    ready = 0;
    }

    break;
}

case READY_STATE_SET_HEX_MODE: {
    MODEM.send("AT+UDCONF=1,1");
    _readyState = READY_STATE_WAIT_SET_HEX_MODE_RESPONSE;
    ready = 0;
    break;
}

case READY_STATE_WAIT_SET_HEX_MODE_RESPONSE: {
    if (ready > 1) {
    _state = NB_ERROR;
    ready = 2;
    } else {
    _readyState = READY_STATE_SET_AUTOMATIC_TIME_ZONE;
    ready = 0;
    }

    break;
}

case READY_STATE_SET_AUTOMATIC_TIME_ZONE: {
    MODEM.send("AT+CTZU=1");
    _readyState = READY_STATE_WAIT_SET_AUTOMATIC_TIME_ZONE_RESPONSE;
    ready = 0;
    break;
}

case READY_STATE_WAIT_SET_AUTOMATIC_TIME_ZONE_RESPONSE: {
    if (ready > 1) {
    _state = NB_ERROR;
    ready = 2;
    } else {
    _readyState = READY_STATE_SET_APN;
    ready = 0;
    }
    break;
}

case READY_STATE_SET_APN: {
    MODEM.sendf("AT+CGDCONT=1,\"IP\",\"%s\"", _apn);
    _readyState = READY_STATE_WAIT_SET_APN;
    ready = 0;
    break;
}

case READY_STATE_WAIT_SET_APN: {
    if (ready > 1) {
    _state = NB_ERROR;
    ready = 2;
    } else {
    _readyState = READY_STATE_SET_APN_AUTH;
    ready = 0;
    }
    break;
}

case READY_STATE_SET_APN_AUTH: {
    if (strlen(_username) > 0 || strlen(_password) > 0) {
    // CHAP
    MODEM.sendf("AT+UAUTHREQ=1,2,\"%s\",\"%s\"", _password, _username);
    } else {
    // no auth
    MODEM.sendf("AT+UAUTHREQ=1,%d", 0);
    }

    _readyState = READY_STATE_WAIT_SET_APN_AUTH;
    ready = 0;
    break;
}

case READY_STATE_WAIT_SET_APN_AUTH: {
    if (ready > 1) {
    _state = NB_ERROR;
    ready = 2;
    } else {
    _readyState = READY_STATE_SET_FULL_FUNCTIONALITY_MODE;
    ready = 0;
    }
    break;
}

case READY_STATE_SET_FULL_FUNCTIONALITY_MODE: {
    MODEM.send("AT+CFUN=1");
    _readyState = READY_STATE_WAIT_SET_FULL_FUNCTIONALITY_MODE;
    ready = 0;
    break;
}

case READY_STATE_WAIT_SET_FULL_FUNCTIONALITY_MODE:{
    if (ready > 1) {
    _state = NB_ERROR;
    ready = 2;
    } else {
    _readyState = READY_STATE_SET_REG_POLICY;
    ready = 0;
    }

    break;
}

//aggiunta
case READY_STATE_SET_REG_POLICY: {
    MODEM.send("AT+CEREG=1");
    _readyState = READY_STATE_AUTOMATIC_NETWORK_SELECTION;
    ready = 0;
    break;
}

//aggiunta
case READY_STATE_WAIT_SET_REG_POLICY:{
    if (ready > 1) {
    _state = NB_ERROR;
    ready = 2;
    } else {
    _readyState = READY_STATE_AUTOMATIC_NETWORK_SELECTION;
    ready = 0;
    }

    break;
}

//aggiunta
case READY_STATE_AUTOMATIC_NETWORK_SELECTION:{              //starts automatic network registration process
    MODEM.setResponseDataStorage(&_response);
    MODEM.send("AT+COPS=0");
    _readyState = READY_STATE_WAIT_AUTOMATIC_NETWORK_SELECTION;
    ready = 0;
}

//aggiunta
case READY_STATE_WAIT_AUTOMATIC_NETWORK_SELECTION: {
    if (ready > 1) {
        _state = NB_ERROR;
        ready = 2;
    } else {
        delay(2000)
        _readyState = READY_STATE_CHECK_REGISTRATION;
    }
}


case READY_STATE_WAIT_SET_FULL_FUNCTIONALITY_MODE:{
    if (ready > 1) {
    _state = NB_ERROR;
    ready = 2;
    } else {
    _readyState = READY_STATE_CHECK_REGISTRATION;
    ready = 0;
    }

    break;
}

case READY_STATE_CHECK_REGISTRATION: {
    MODEM.setResponseDataStorage(&_response);
    MODEM.send("AT+CEREG?");
    _readyState = READY_STATE_WAIT_CHECK_REGISTRATION_RESPONSE;
    ready = 0;
    break;
}

case READY_STATE_WAIT_CHECK_REGISTRATION_RESPONSE: {
    if (ready > 1) {
    _state = NB_ERROR;
    ready = 2;
    } else {
    int status = _response.charAt(_response.length() - 1) - '0';

    if (status == 0 || status == 4) {
        _readyState = READY_STATE_CHECK_REGISTRATION;
        ready = 0;
    } else if (status == 1 || status == 5 || status == 8) {
        _readyState = READY_STATE_DONE;
        _state = NB_READY;
        ready = 1;
    } else if (status == 2) {
        _readyState = READY_STATE_CHECK_REGISTRATION;
        _state = CONNECTING;
        ready = 0;
    } else if (status == 3) {
        _state = NB_ERROR;
        ready = 2;
    } 
    }

    break;
}

case READY_STATE_DONE:
    break;
}

return ready;

}