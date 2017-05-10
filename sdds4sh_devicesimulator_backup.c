#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


#include "sdds4sh_devicesimulator_sdds_impl.h"
#include "../sdds4sh/sdds4sh_devices.h"
#include "../sdds4sh/system_devices.h"
#include "../sdds4sh/console_commands.h"



static uint16_t l_device_id = DEVICE_SIMULATOR_DEVICE_ID;
uint16_t ID_Handler = DEVICE_SIMULATOR_DEVICE_ID; //The Device ID shall be changable.
//uint16_t Nonce_Handler = DEVICE_SIMULATOR_DEVICE_NONCE;

char* cmd = "Press: \n \t "
            "battery <0 ... 100 for remaining capacity in percent> for Battery Information\n\t "
            "occupancy <0 or 1 for true / false> [type: PIR, UNKNOWN]  for Occupancy Sensor\n\t "
            "contact <0 or 1 for open / closed> for Contact Sensor \n\t "
            "light <0 or 1 for off / on> for Light\n\t "
            "loglevel <0 ... 6> to set the log level (0 for verbose, 6 for none)\n\t "
            "status for current status\n\t "
            "help/? for this information\n\t "
            "exit to exit the program\n";
            
            
static BatteryInformation batteryinformation_pub;
static OccupancySensor occupancysensor_pub;
static SingleInputContact singleinputcontact_pub;
static OnOffLight onofflight_pub;
static SingleInputContactNonce singleinputcontactnonce_pub;




int 
_doBatteryInformation(char* arg);
int
_doOccupancySensor(char* arg);
int
_doContactSensor(char* arg);
int
_doOnOffLight(char* arg);
int
_printStatus(char* arg);
int
_setLogLevel(char* arg);
int
_doContactSensorNonce(char* arg);

int _initSimulator() {
    
    batteryinformation_pub.id = ID_Handler;
    batteryinformation_pub.remaining = 100;
    
    occupancysensor_pub.id = ID_Handler;
    occupancysensor_pub.occupancy = sDDS_OccupiedState_UNOCCUPIED;
    occupancysensor_pub.sensortype = sDDS_OccupancySensorType_PIR;
    
    singleinputcontact_pub.id = ID_Handler;
    singleinputcontact_pub.contact = sDDS_OpenCloseState_OPEN;
    singleinputcontact_pub.outofservice = false;

    singleinputcontactnonce_pub.id = ID_Handler;
    singleinputcontactnonce_pub.contact = sDDS_OpenCloseState_OPEN;
    singleinputcontactnonce_pub.outofservice = false;
    
    onofflight_pub.id = ID_Handler;
    onofflight_pub.state = sDDS_OnOffState_OFF;
}

void sdds_DR_handler_OnOffFunctionality(DDS_DataReader reader);

int main()
{
	DDS_ReturnCode_t ret;

	if (sDDS_init() == SDDS_RT_FAIL) {
		return 1;
	}
	Log_setLvl(6);  // Disable logs, set to 0 for to see everything.

    _initSimulator();
    
    // init OnOffFunctionality callback handler
    
    struct DDS_DataReaderListener listStruct = { .on_data_available =
			&sdds_DR_handler_OnOffFunctionality };

	ret = DDS_DataReader_set_listener(
			(DDS_DataReader) g_OnOffFunctionality_reader, &listStruct, NULL );
  

    // Ausgabe Program
    printf("Device Simulator!\n");
    

    for (;;) {
        // Ausgabe Befehle
        
        char* input = NULL;
        size_t length = 0;
        
        printf("Command: ");
        
        ssize_t readsize = getline(&input, &length, stdin);
        
        if (readsize < 0) {
            continue;
        }
        
        // trim string
        trim(input);
        
        
        if (strcmp(input, "exit") == 0 || strcmp(input, "quit") == 0) {
            exit(0);
        }
        
        if ((strcmp(input, "help") == 0) ||  (strcmp(input, "?") == 0)) {
            printf(cmd);
            free(input);
            continue;
        }
        
        // string tokenize
        
        char* str_ptr;
	
        
        str_ptr = strtok(input, " ");
        

        if (str_ptr == NULL) {
            printf("fehler?\n");
            free(input);
            continue;
        }
        //printf("strptr %s >\n" str_ptr);
        if (strcmp(str_ptr, "battery") == 0) {
            _doBatteryInformation(str_ptr);
        } else 
        if (strcmp(str_ptr, "occupancy") == 0) {
            _doOccupancySensor(str_ptr);
        } else 
        if (strcmp(str_ptr, "contact") == 0) {

            _doContactSensor(str_ptr);
        } else
        if (strcmp(str_ptr, "light") == 0) {
            _doOnOffLight(str_ptr);
        } else
        if (strcmp(str_ptr, "status") == 0) {
            _printStatus(str_ptr);
        } else
        if (strcmp(str_ptr, "loglevel") == 0) {
            _setLogLevel(str_ptr);
        } else
	if (strcmp(str_ptr, "contactnonce") == 0) {
	    _doContactSensorNonce(str_ptr);
	}
        else
        {
            printf("Unknown command!\n");
            printf(cmd);
        }        
        
        // do stuff
        
        // free input
        free(input);
        /*
       

        
    */
        
/*
        

        
        */

     //   sleep (5);
    }

    return 0;
}

int 
_doBatteryInformation(char* arg) {
     
    char* ptr = strtok(NULL, " ");
    
    if (ptr == NULL) {
        printf("No remaning battery level given!");
        printf("Usage: battery <0 ...100>\n");
        return -1;
    }
    unsigned long value = strtoul(ptr, NULL, 10);
    
    if (value > 100) {
        printf("Battery level to high\n");
        printf("Usage: battery <0 ...100>\n");
        return -1;
    }

	// read Device ID.
	ptr = strtok(NULL, " ");
    if (ptr != NULL) {
	uint16_t Read_Device_ID = strtoul(ptr, NULL, 10);
	ID_Handler = Read_Device_ID;
        printf("Device ID %s set!\n", ptr);
	}

    printf("Set Battery Information level to %u for device 0x%4x \n", value, ID_Handler);

    batteryinformation_pub.id = ID_Handler;
    batteryinformation_pub.remaining = value;
    
    // send datasample
    DDS_ReturnCode_t ret = DDS_BatteryInformationDataWriter_write(g_BatteryInformation_writer, &batteryinformation_pub, NULL);
    if (ret != DDS_RETCODE_OK) {
        printf ("Failed to send topic batteryinformation\n");
    }
    else {
        printf ("Send a batteryinformation sample\n");
    }
    
    return 0;
    
}
int
_doOccupancySensor(char* arg) {
    char* ptr = strtok(NULL, " ");
    
    if (ptr == NULL) {
        printf("No occupancy state given!");
        printf("Usage:occupancy <0 or 1 for true / false> [type: PIR, UNKNOWN]\n");
        return -1;
    }
    unsigned long value = strtoul(ptr, NULL, 10);
    
    if (value > 1) {
        printf("no boolean value\n");
        printf("Usage: occupancy <0 or 1 for true / false> [type: PIR, UNKNOWN]\n");
        return -1;
    }
    ptr = strtok(NULL, " ");

    // static sensortype
    static uint8_t sensorType = sDDS_OccupancySensorType_PIR;
    
    if (ptr == NULL) {
        // no additonal sensor type given, use last setting
        //occupancysensor_pub.sensortype = sensorType;
    } else
    if (strcmp(ptr, "PIR") == 0) {
        sensorType = sDDS_OccupancySensorType_PIR;
    } else 
    if (strcmp(ptr, "UNKNOWN") == 0) {
        sensorType = sDDS_OccupancySensorType_UNKNOWN;
    } else {
        printf("Unknow sensor type!\n");
        printf("Usage: occupancy <0 or 1 for true / false> [type: PIR, UNKNOWN]\n");
        return -1;
    }
	// read Device ID.
	ptr = strtok(NULL, " ");
    if (ptr != NULL) {
	uint16_t Read_Device_ID = strtoul(ptr, NULL, 10);
	ID_Handler = Read_Device_ID;
        printf("Device ID %s set!\n", ptr);
	}
    
    printf("Set occupancy state to %u with type 0x%2x for device 0x%4x \n", value, sensorType, ID_Handler);
    
    occupancysensor_pub.id = ID_Handler;
    if (value == 0) {
        occupancysensor_pub.occupancy = sDDS_OccupiedState_UNOCCUPIED;
    } else {
        occupancysensor_pub.occupancy = sDDS_OccupiedState_OCCUPIED;
    }
    
    occupancysensor_pub.sensortype = sensorType;
    
    DDS_ReturnCode_t ret = DDS_OccupancySensorDataWriter_write (g_OccupancySensor_writer, &occupancysensor_pub, NULL);
    if (ret != DDS_RETCODE_OK) {
        printf ("Failed to send topic occupancysensor\n");
    }
    else {
        printf ("Send a occupancysensor sample\n");
    }
    
    return 0;
}
int
_doContactSensor(char* arg) {
    char* ptr = strtok(NULL, " ");
    
    if (ptr == NULL) {
        printf("No contact sensor state given!");
        printf("Usage: contact <0 or 1 for open / closed>\n ");
        return -1;
    }
    unsigned long value = strtoul(ptr, NULL, 10);
    
    if (value > 1) {
        printf("no boolean value\n");
        printf("Usage: contact <0 or 1 for open / closed>\n ");
        return -1;
    }

	// read Device ID.
	ptr = strtok(NULL, " ");
    if (ptr != NULL) {
	uint16_t Read_Device_ID = strtoul(ptr, NULL, 10);
	ID_Handler = Read_Device_ID;
        printf("Device ID %s set!\n", ptr);
	}
    singleinputcontact_pub.id = ID_Handler;
    if (value == 0) {
        singleinputcontact_pub.contact = sDDS_OpenCloseState_OPEN;
    } else {
	
        singleinputcontact_pub.contact = sDDS_OpenCloseState_CLOSE;
    }
    // TODO parse out of service
    singleinputcontact_pub.outofservice = false;
    
	
    
    // Send Contact sample
    while(true){
    DDS_ReturnCode_t ret = DDS_SingleInputContactDataWriter_write(g_SingleInputContact_writer, &singleinputcontact_pub, NULL);
    if (ret != DDS_RETCODE_OK) {
        printf ("Failed to send topic singleinputcontact\n");
    }
    else {
        printf ("Send a singleinputcontact sample\n");
    }
    sleep(20);
    }
    
}
int
_doContactSensorNonce(char* arg) {
    char* ptr = strtok(NULL, " ");
    
    if (ptr == NULL) {
        printf("No contact sensor state given!");
        printf("Usage: contact <0 or 1 for open / closed>\n ");
        return -1;
    }
    unsigned long value = strtoul(ptr, NULL, 10);
    
    if (value > 1) {
        printf("no boolean value\n");
        printf("Usage: contact <0 or 1 for open / closed>\n ");
        return -1;
    }

	// read Device ID.
	ptr = strtok(NULL, " ");
    if (ptr != NULL) {
	uint16_t Read_Device_ID = strtoul(ptr, NULL, 10);
	ID_Handler = Read_Device_ID;
        printf("Device ID %s set!\n", ptr);
	}
	
	singleinputcontactnonce_pub.id = ID_Handler;

    	ptr = strtok(NULL, " ");
	if(ptr != NULL){
		uint16_t Read_Device_Nonce = strtoul(ptr, NULL, 13);
		uint16_t Nonce_Handler = Read_Device_Nonce;
		printf("Nonce: %s \n", ptr);
		singleinputcontactnonce_pub.nonce = Nonce_Handler;
	}
	else{
	    time_t t;
	    t = time(NULL);
	    t = t/30;
	    uint16_t nonce_handler = t;
	    printf("%x \n", nonce_handler);
	    singleinputcontactnonce_pub.nonce = nonce_handler;
	}
    if (value == 0) {
        singleinputcontactnonce_pub.contact = sDDS_OpenCloseState_OPEN;
    } else {
	
        singleinputcontactnonce_pub.contact = sDDS_OpenCloseState_CLOSE;
    }
    // TODO parse out of service
    singleinputcontactnonce_pub.outofservice = false;
    
	
    
    // Send Contact sample
    while(true){
    DDS_ReturnCode_t ret = DDS_SingleInputContactNonceDataWriter_write(g_SingleInputContactNonce_writer, &singleinputcontactnonce_pub, NULL);
    if (ret != DDS_RETCODE_OK) {
        printf ("Failed to send topic singleinputcontactnonce\n");
    }
    else {
        printf ("Send a singleinputcontactnonce sample\n");
    }
    sleep(20);
    }
    
}
int
_doOnOffLight(char* arg) {
    char* ptr = strtok(NULL, " ");
    
    if (ptr == NULL) {
        printf("No on of light state given!");
        printf("Usage: light <0 or 1 for off / on>\n");
        return -1;
    }
    unsigned long value = strtoul(ptr, NULL, 10);
    
    if (value > 1) {
        printf("no boolean value\n");
        printf("Usage: light <0 or 1 for off / on>\n");
        return -1;
    }
	// read Device ID.
	ptr = strtok(NULL, " ");
    if (ptr != NULL) {
	uint16_t Read_Device_ID = strtoul(ptr, NULL, 10);
	ID_Handler = Read_Device_ID;
        printf("Device ID %s set!\n", ptr);
	}
    
    onofflight_pub.id = ID_Handler;
    if (value == 0) {
        onofflight_pub.state = sDDS_OnOffState_OFF;
    } else {
        onofflight_pub.state = sDDS_OnOffState_ON;
    }
    
    DDS_ReturnCode_t ret = DDS_OnOffLightDataWriter_write (g_OnOffLight_writer, &onofflight_pub, NULL);
    if (ret != DDS_RETCODE_OK) {
        printf ("Failed to send topic onofflight\n");
    }
    else {
        printf ("Send a onofflight sample\n");
    }
    
    return 0;
}

int
_printStatus(char* arg) {
    
}
int
_setLogLevel(char* arg) {
    
    char* ptr = strtok(NULL, " ");
    
    if (ptr == NULL) {
        printf("No log level given!");
        printf("Usage: loglevel <0 ... 6> to set the log level (0 for verbose, 6 for none)\n");
        return -1;
    }
    unsigned long value = strtoul(ptr, NULL, 10);
    
    if (value > 6) {
        printf("no valid log level\n");
        printf("Usage: loglevel <0 ... 6> to set the log level (0 for verbose, 6 for none)\n");
        return -1;
    }
    Log_setLvl(value);
    
    return 0;
}

void 
sdds_DR_handler_OnOffFunctionality(DDS_DataReader reader) {
    
    OnOffFunctionality data;
	OnOffFunctionality* data_ptr = &data;

	DDS_ReturnCode_t ret;
    
    ret = DDS_OnOffFunctionalityDataReader_take_next_sample(reader,
                &data_ptr, NULL);
    if (ret == DDS_RETCODE_NO_DATA) {
        printf("no data for onofffunctionality\n");
        return ;
    }
    if (ret != DDS_RETCODE_OK) {
		Log_error(
				"Error receiving data from OnOffFunctionality topic\n");
		return;
	}
    
    // check device ID
    if (data.id != ID_Handler) {
        // DEBUG
        printf("Got OnOffFunctioality command for device 0x%4x \n", data.id);
		return;
	}
    
    printf("\n Got OnOffFunctionality command: %s \n", (data.state ? "ON" : "OFF"));
    
    // change state
    
    onofflight_pub.id = ID_Handler;

	if (data.state == sDDS_OnOffCommand_OFF) {
		onofflight_pub.state = sDDS_OnOffState_OFF;
	} else 
    if (data.state == sDDS_OnOffCommand_ON) {
		onofflight_pub.state = sDDS_OnOffState_ON;
	} else
    if (data.state == sDDS_OnOffCommand_TOGGLE) {
        onofflight_pub.state = (onofflight_pub.state == sDDS_OnOffState_ON) ? sDDS_OnOffState_OFF : sDDS_OnOffState_ON;
    }

    // bad behaviour, but emulates some action
    sleep(1);
    
    printf("Set Light to state %s\n", (onofflight_pub.state == sDDS_OnOffState_ON)? "ON" : "OFF");

     
    ret = DDS_OnOffLightDataWriter_write(g_OnOffLight_writer, &onofflight_pub, NULL);
    if (ret != DDS_RETCODE_OK) {
        printf ("Failed to send topic onofflight\n");
    }
    return;
}
