//##########################################################################################################
//### Setup Zone ###########################################################################################
//##########################################################################################################
  
  #define Output_Driver 4      // 1 =  Steering Motor + Cytron MD30C Driver
                               // 2 =  Steering Motor + IBT 2  Driver
                               // 3 =  Steering Motor + JRK 2 Driver (see https://github.com/aortner/jrk)
							   // 4 =  Steering valve VALVE_LEFT and VALVE_RIGHT
 
  #define Output_Invert 0      // 1 = reverse output direction (Valve & Motor)

  #define ADC_Mode 0           //0 = No ADS installed, Wheel Angle Sensor connected directly to Arduino at A0
                               //2 = ADS1115 Differential Mode - Connect Sensor GND to A1, Signal to A0
                               //3 = JRK 2 AD_Input (only for use with JRK 2 Motorcontroller)
  
  #define SteerPosZero 512     //vary this to get near 0 degrees when wheels are straight forward    
                               //with Arduino ADC start with 512 (0-1024)
                               //with ADS start with 6500  (possible Values are 0-13000 Counts)
                               //with JRK 2 use 2046
  
  #define Invert_WAS 0                 // set to 1 to Change Direction of Wheel Angle Sensor - to + 
  
  #define IMU_Installed 0               // set to 1 to enable BNO055 IMU
  
  #define Inclinometer_Installed 0      // set to 1 if DOGS2 Inclinometer is installed
                                        // set to 2 if MMA8452 is installed (Address 0x1C) (SA0=LOW)
                                        // set to 3 if MMA8452 is installed (Address 0x1D) (SA0=HIGH, Sparkfun)
  
  #define InvertRoll 0                  // Roll to the right must be positive
                                        // Set to 1 if roll to right shows negative

  #define Relay_Type 0                  // set to 0 if up to 8 Section Relays will be used
                                        // set to 1 if up to 8 uTurn Relays will be used (only Serial Mode)
  
  #define useSteerSwitch 0              // set to 1 if a Steerswitch is installed
  #define PinMapping 2                  // 0 = default Mapping (like the included Schematics)
                                        // 1 = PCB Basic Autosteer
										// 2 = default Mapping mod by ipaev
  
  //Ethernet Details
  #define EtherNet 0      // 0 = Serial/USB communcation with AOG
                          // 1 = Ethernet comunication with AOG (using a ENC28J60 chip)
  #define CS_Pin 10       // Arduino Nano= 10 depending how CS of Ethernet Controller ENC28J60 is Connected

  #define   maxspeed  20     // km/h  above -> steering off
  #define   minspeed  1      // km/h  below -> sterring off (minimum = 0.25)
  //##########################################################################################################
  //### End of Setup Zone ####################################################################################
  //##########################################################################################################

// Pin Configuaration
#if (PinMapping == 0 )  // Default Mapping
  #define STEERSW_PIN 3  //PD3
  #define WORKSW_PIN  4  //PD4
  #define PWM1_PIN    5  //PD5  
  #define DIR_PIN     6  //PD6
  #define LED_PIN     7  //PD7 Autosteer LED
  #define W_A_S      A0  //PC0 Wheel Angle Sensor
  #define Dogs2_Roll A1  //PC1 EADOGS2 Inclinometer
  //ethercard 10,11,12,13   
  #define RELAY1_PIN 8   //PB0
  #define RELAY2_PIN 9   //PB1
  #define RELAY3_PIN A2  //PC2
  #define RELAY4_PIN A3  //PC3
  //#define RELAY5_PIN 10  //PB2  serial Mode only
  //#define RELAY6_PIN 11  //PB3  serial Mode only
  //#define RELAY7_PIN 12  //PB4  serial Mode only
  //#define RELAY8_PIN 13  //PB5  serial Mode only

#elif (PinMapping == 1 ) // (PinMapping == 1)
// PCB Basic Autosteer
  #define STEERSW_PIN 6  //PD6
  #define WORKSW_PIN  8  //PB0
  #define IMPSW_PIN     7  //PD7
  #define PWM1_PIN    3  //PD3  
  #define DIR_PIN     4  //PD4
  #define PWM2_PIN    9  //PB1
  #define LED_PIN     5  //PD5 Autosteer LED
  #define W_A_S      A0  //PC0 Wheel Angle Sensor
  #define Dogs2_Roll A1  //PC1 EADOGS2 Inclinometer
  //ethercard 10,11,12,13   
  #define RELAY1_PIN A2  //PC2
  #define RELAY2_PIN A3  //PC3
  //#define RELAY3_PIN A0  //PC0
  //#define RELAY4_PIN A1  //PC1
#elif (PinMapping == 2 )  // Default Mapping mod by ipaev
  #define STEERSW_PIN 3  //PD3
  #define WORKSW_PIN  4  //PD4
  #define VALVE_LEFT  5  /*PD5 LEFT VALVE*/
  #define VALVE_RIGHT 6  /*PD6 RIGHT VALVE*/
  #define LED_PIN     7  //PD7 Autosteer LED
  #define W_A_S      A0  //PC0 Wheel Angle Sensor
  #define Dogs2_Roll A1  //PC1 EADOGS2 Inclinometer
  //ethercard 10,11,12,13
  #define RELAY1_PIN 8   //PB0
  #define RELAY2_PIN 9   //PB1
  #define RELAY3_PIN A2  //PC2
  #define RELAY4_PIN A3  //PC3
  //#define RELAY5_PIN 10  //PB2  serial Mode only
  //#define RELAY6_PIN 11  //PB3  serial Mode only
  //#define RELAY7_PIN 12  //PB4  serial Mode only
  //#define RELAY8_PIN 13  //PB5  serial Mode only

  
#endif

  #include <Wire.h>
  #include <EEPROM.h>

#if (EtherNet)
  #include <EtherCard.h>
  #include <IPAddress.h> 

  //Array to send data back to AgOpenGPS
  byte toSend[] = {0,0,0,0,0,0,0,0,0,0};

  // ethernet interface ip address
  static byte myip[] = { 192,168,1,77 };
  // gateway ip address
  static byte gwip[] = { 192,168,1,1 };
  //DNS- you just need one anyway
  static byte myDNS[] = { 8,8,8,8 };
  //mask
  static byte mask[] = { 255,255,255,0 };
  //this is port of this autosteer module
  unsigned int portMy = 5577; 

  //sending back to where and which port
  static byte ipDestination[] = {192, 168, 1, 255};
  unsigned int portDestination = 9999; //AOG port that listens
  // ethernet mac address - must be unique on your network
  static byte mymac[] = { 0x70,0x69,0x69,0x2D,0x30,0x31 };
  byte Ethernet::buffer[200]; // udp send and receive buffer
#endif

#if ADC_Mode==1 | ADC_Mode==2
  #include "Adafruit_ADS1015.h"
  Adafruit_ADS1115 ads;     // Use this for the 16-bit version ADS1115
  #define SteerSensorCnt 200
#else
  #define SteerSensorCnt 10  
#endif

#if Inclinometer_Installed ==2 | Inclinometer_Installed ==3
    #include "MMA8452_AOG.h"  // MMA8452 (1) Inclinometer
    #if Inclinometer_Installed == 3
      MMA8452 accelerometer(0x1D);
    #else
      MMA8452 accelerometer;
    #endif
#endif

#if IMU_Installed
  #include "BNO055_AOG.h"  // BNO055 IMU
  #define A 0X28             //I2C address selection pin LOW
  #define B 0x29             //                          HIGH
  #define RAD2GRAD 57.2957795
  BNO055 IMU(A);  // create an instance
#endif

#if Output_Driver == 3 // 3 =  Steering Motor + JRK 2 Driver
  #include <JrkG2.h>   
  // add this library via:  Tools->Manage Libraries
  // or get from https://github.com/pololu/jrk-g2-arduino
  JrkG2I2C jrk;
  #define SteerPosZero 2046
  #define Invert_WAS 1
#endif

#define EEP_Ident 0xEDED

 //Variables   
  struct Storage {
    float Ko = 0.0f;  //overall gain
    float Kp = 0.0f;  //proportional gain
    float Ki = 0.0f;//integral gain
    float Kd = 0.0f;  //derivative gain 
    float steeringPositionZero = SteerPosZero;
    byte minPWMValue=10;
    int maxIntegralValue=20;//max PWM value for integral PID component
    float steerSensorCounts = SteerSensorCnt;
    byte oldsteerzero=0;  // stores old Steerzerovalue
};  Storage steerSettings;

//loop time variables in microseconds
const unsigned int LOOP_TIME = 100; //10hz
unsigned int lastTime = LOOP_TIME;
unsigned int currentTime = LOOP_TIME;
unsigned int dT = 50000;
byte count = 0;
byte watchdogTimer = 0;
byte serialResetTimer = 0; //if serial buffer is getting full, empty it

//Kalman variables
float rollK = 0, Pc = 0.0, G = 0.0, P = 1.0, Xp = 0.0, Zp = 0.0;
float XeRoll = 0;
const float varRoll = 0.1; // variance,
const float varProcess = 0.0001; //smaller is more filtering

//Program flow
bool isDataFound = false, isSettingFound = false, MMAinitialized = false;
int header = 0, tempHeader = 0, temp, EEread = 0;
byte relay = 0, uTurn = 0, workSwitch = 0, steerSwitch = 1, impSwitch = 0, switchByte = 0;
float distanceFromLine = 0, corr = 0, speeed = 0;

//steering variables
float steerAngleActual = 0;
int steerPrevSign = 0, steerCurrentSign = 0; // the steering wheels angle currently and previous one
float steerAngleSetPoint = 0; //the desired angle from AgOpen
long steeringPosition = 0; //from steering sensor
float steerAngleError = 0; //setpoint - actual
float distanceError = 0; //
int steerZero = 0;

//inclinometer variables
int roll = 0;
uint16_t x_ , y_ , z_;

//pwm variables
int pwmDrive = 0, drive = 0, pwmDisplay = 0;
float pValue = 0, iValue = 0, dValue = 0;


void setup()
{    
  //keep pulled high and drag low to activate, noise free safe    
  pinMode(WORKSW_PIN, INPUT_PULLUP);    
  pinMode(STEERSW_PIN, INPUT_PULLUP);   
#ifdef DIR_PIN
  pinMode(DIR_PIN, OUTPUT);            // direction pin of PWM Board
#endif
#ifdef PWM1_PIN
  pinMode(PWM1_PIN, OUTPUT);           // PWM pin
#endif
  pinMode(LED_PIN, OUTPUT);            // Autosteer LED indicates AS on

  #ifdef VALVE_LEFT
  	  pinMode(VALVE_LEFT, OUTPUT); //left valve
  #endif

  #ifdef VALVE_RIGHT
	  pinMode(VALVE_RIGHT, OUTPUT); //right valve
  #endif
  
  #ifdef IMPSW_PIN
      pinMode(IMPSW_PIN, INPUT_PULLUP); //third Switch
  #endif
  
  #ifdef PWM2_PIN
    pinMode(PWM2_PIN, OUTPUT); //second PWM Pin
  #endif
  
  #ifdef RELAY1_PIN
    pinMode(RELAY1_PIN, OUTPUT); //configure RELAY1 for output
  #endif
  #ifdef RELAY2_PIN
    pinMode(RELAY2_PIN, OUTPUT); //configure RELAY2 for output
  #endif
  #ifdef RELAY3_PIN
    pinMode(RELAY3_PIN, OUTPUT); //configure RELAY3 for output
  #endif
  #ifdef RELAY4_PIN
    pinMode(RELAY4_PIN, OUTPUT); //configure RELAY4 for output
  #endif
  #ifdef RELAY5_PIN
    pinMode(RELAY5_PIN, OUTPUT); //configure RELAY5 for output
  #endif
  #ifdef RELAY6_PIN
    pinMode(RELAY6_PIN, OUTPUT); //configure RELAY6 for output
  #endif
  #ifdef RELAY7_PIN
    pinMode(RELAY7_PIN, OUTPUT); //configure RELAY7 for output
  #endif
  #ifdef RELAY8_PIN
    pinMode(RELAY8_PIN, OUTPUT); //configure RELAY8 for output
  #endif
  
  //set up communication
  Wire.begin();
  Serial.begin(38400);

#if (IMU_Installed)  // BNO055 init
  IMU.init();
  // Restore calibration data from zeroing
  bno055_offsets_t calibrationData;
  calibrationData.accel_offset_x = 65520;
  calibrationData.accel_offset_y = 65421;
  calibrationData.accel_offset_z = 65421;
  calibrationData.gyro_offset_x = 65534;
  calibrationData.gyro_offset_y = 65533;
  calibrationData.gyro_offset_z = 1;
  calibrationData.mag_offset_x = 65305;
  calibrationData.mag_offset_y = 119;
  calibrationData.mag_offset_z = 65418;
  calibrationData.accel_radius = 1000;
  calibrationData.mag_radius = 979;
  IMU.setSensorOffsets(calibrationData);
  //use external 32K crystal
  IMU.setExtCrystalUse(true);
#endif  
	
#if Inclinometer_Installed ==2 | Inclinometer_Installed ==3
      // MMA8452 (1) Inclinometer
      MMAinitialized = accelerometer.init();
      if (MMAinitialized){
	accelerometer.setDataRate(MMA_800hz);
        accelerometer.setRange(MMA_RANGE_8G);
        accelerometer.setHighPassFilter(false); 
      }
      else Serial.println("MMA init fails!!");
#endif

	//PWM rate settings Adjust to desired PWM Rate
	//TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
	TCCR1B = TCCR1B && B11111000 | B00000011;    // set timer 1 divisor to    64 for PWM frequency of     490.20 Hz (The DEFAULT)

#if (EtherNet)
 if (ether.begin(sizeof Ethernet::buffer, mymac, CS_Pin) == 0)
    Serial.println(F("Failed to access Ethernet controller"));

  //set up connection
  ether.staticSetup(myip, gwip, myDNS, mask); 
  ether.printIp("IP:  ", ether.myip);
  ether.printIp("GW:  ", ether.gwip);
  ether.printIp("DNS: ", ether.dnsip);

  //set up the pgn for returning data for autosteer
  toSend[0] = 0x7F;
  toSend[1] = 0xFD;
  
  //register udpSerialPrint() to port 8888
  ether.udpServerListenOnPort(&udpSteerRecv, 8888);
#endif

EEPROM.get(0, EEread);               // read identifier
 if (EEread != (int)EEP_Ident){           // check on first start and write EEPROM
    EEPROM.put(0, EEP_Ident);
    EEPROM.put(2, SteerPosZero);
    EEPROM.put(8, steerSettings);   
  }
 else 
  { 
    EEPROM.get(8, steerSettings);     // read the Settings
    EEPROM.get(2, EEread);            // read SteerPosZero
    if (EEread != SteerPosZero){
      EEPROM.put(2, SteerPosZero);    // if changed , rewrite
      steerSettings.steeringPositionZero = (SteerPosZero);  //use new steering zero offset now
      EEPROM.put(8, steerSettings);   
    }
    
  }
}// End of Setup


void loop()
{
	/*
	 * Loop triggers every 100 msec and sends back gyro heading, and roll, steer angle etc
	 * All imu code goes in the loop
	 *  Determine the header value and set the flag accordingly
	 *  Then the next group of serial data is according to the flag
	 *  Process accordingly updating values
	 */

currentTime = millis();

if (currentTime - lastTime >= LOOP_TIME)
 {
     dT = currentTime - lastTime;
     lastTime = currentTime;

#if (IMU_Installed)
     IMU.readIMU();
#endif

     //If connection lost to AgOpenGPS, the watchdog will count up and turn off steering
     if (watchdogTimer++ > 250) watchdogTimer = 12;

#if (Inclinometer_Installed ==1)
     //DOGS2 inclinometer
     delay(1);
     analogRead(Dogs2_Roll); //discard
     delay(1);
     roll = analogRead(Dogs2_Roll);   delay(1);
     roll += analogRead(Dogs2_Roll);   delay(1);
     roll += analogRead(Dogs2_Roll);   delay(1);
     roll += analogRead(Dogs2_Roll);
     roll = roll >> 2; //divide by 4
     //inclinometer goes from -25 to 25 from 0 volts to 5 volts
     rollK = map(roll, -1023, 1023, -400, 400); //16 counts per degree
#endif

#if Inclinometer_Installed ==2 | Inclinometer_Installed ==3
   // MMA8452 (1) Inclinometer
  if (MMAinitialized){
    accelerometer.getRawData(&x_, &y_, &z_);
    roll=x_; //Conversion uint to int
    if (roll > 4200)  roll =  4200;
    if (roll < -4200) roll = -4200;
    rollK = map(roll,-4200,4200,-960,960); //16 counts per degree (good for 0 - +/-30 degrees) 
  }
#endif
//if not positive when rolling to the right
#if InvertRoll ==1
  rollK *= -1.0;
#endif
	

    //Kalman filter
    Pc = P + varProcess;
    G = Pc / (Pc + varRoll);
    P = (1 - G) * Pc;
    Xp = XeRoll;
    Zp = Xp;
    XeRoll = G * (rollK - Zp) + Xp;

  #ifdef IMPSW_PIN
    impSwitch = digitalRead(IMPSW_PIN);  // read imp switch
  #endif 
    workSwitch = digitalRead(WORKSW_PIN);  // read work switch
    if (useSteerSwitch) steerSwitch = digitalRead(STEERSW_PIN); //read auto steer enable switch 
    else steerSwitch = 0; //permanetely On
    switchByte = workSwitch | steerSwitch << 1 | impSwitch << 2;

#if Relay_Type==0
    SetRelays();       //turn on off section relays
#else
    SetuTurnRelays();  //turn on off uTurn relays
#endif

 //get steering position 
#if ADS_Mode==0   //WAS at arduino
    analogRead(W_A_S); //discard initial reading // Arduino ADC     
    steeringPosition = analogRead(W_A_S);    delay(1);
    steeringPosition += analogRead(W_A_S);    delay(1);
    steeringPosition += analogRead(W_A_S);    delay(1);
    steeringPosition += analogRead(W_A_S);
    steeringPosition = steeringPosition >> 2; //divide by 4
#endif 

#if ADC_Mode==2    //ADS1115 Differential Mode
    steeringPosition = ads.readADC_Differential_0_1();    delay(2);    //ADS1115 Differential Mode 
    steeringPosition += ads.readADC_Differential_0_1();   delay(2);    //Connect Sensor GND to A1
    steeringPosition += ads.readADC_Differential_0_1();   delay(2);    //Connect Sensor Signal to A0
    steeringPosition += ads.readADC_Differential_0_1();
    steeringPosition = steeringPosition >> 3;    //divide by 8
#endif 
    
    //Serial.print("CountsPerDegree= ");Serial.print(steerSettings.steerSensorCounts); 
    //Serial.print("  Raw AD-Value:"); Serial.println(steeringPosition);  // helps calibrating zero
    steeringPosition = ( steeringPosition -steerSettings.steeringPositionZero);   //center the steering position sensor

#if Output_Driver == 3 // 3 =  Steering Motor + JRK 2 Driver
    steeringPosition = jrk.getScaledFeedback();  
    steeringPosition = ( steeringPosition -steerZero);   //center the steering position sensor
#endif      
    
    //convert position to steer angle. 
    //  ***** make sure that negative steer angle makes a left turn and positive value is a right turn *****
    // remove or add the minus for steerSensorCounts to do that.
#if Invert_WAS
    steerAngleActual = (float)(steeringPosition) /   steerSettings.steerSensorCounts;
#else
    steerAngleActual = (float)(steeringPosition) / - steerSettings.steerSensorCounts;
#endif

    
#if Inclinometer_Installed != 0
    steerAngleActual = steerAngleActual - (XeRoll * (steerSettings.Kd/800));     // add the roll
#endif		

    //close enough to center, 4 cm, remove any correction          
    //if (distanceFromLine < 40 && distanceFromLine > -40) steerAngleSetPoint = 0;
    if (distanceFromLine <= 40 && distanceFromLine >= -40) corr = 0;
    else
    {
      //use the integal value to adjust how much per cycle it increases
      corr += steerSettings.Ki;

      //provide a limit - the old max integral value
      if (corr > steerSettings.maxIntegralValue) corr = steerSettings.maxIntegralValue;

      //now add the correction to fool steering position
      if (distanceFromLine > 40) 
      {
        steerAngleSetPoint -= corr;
      }
      else
      {
        steerAngleSetPoint += corr;
      }
    }

// Send Data to AOG
#if (EtherNet)
    transmitEthernet(); 
#else
    transmitSerial();
#endif
    
	} //end of timed loop

  
#if (EtherNet)
  delay(10); 
  //this must be called for ethercard functions to work.
  ether.packetLoop(ether.packetReceive());  
#else
  receiveSerial();
#endif
  
  if (watchdogTimer < 10)
    { 
      digitalWrite(LED_PIN, 1);   //turn LED on
      steerAngleError = steerAngleActual - steerAngleSetPoint;   //calculate the steering error
      calcSteeringPID();  //do the pid
      motorDrive();       //out to motors the pwm value
    }
  else
    {
      //we've lost the comm to AgOpenGPS
      pwmDrive = 0; //turn off steering motor
      motorDrive(); //out to motors the pwm value
      digitalWrite(LED_PIN, 0); //turn LED off
    }
} // end of main loop
