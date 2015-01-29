// digital input pin
#define BUTTON 12

#define button_crest1 8
#define button_crest2 9
#define button_crest3 10
#define button_crest4 11

#define data 2
#define clock 4

// analog input pin for laboratory
#define SLIDER 2
#define LIGHT  4
#define SOUND  3
#define R_A 0
#define R_B 1
#define R_C 5
#define R_D 5// Arduino Diecimila has 6 analog inputs

#define FIRMWAEW_ID 4  // ScratchBoard 1.1 Firmware ID

// analog input pin for robot
#define ANALOG0 0
#define ANALOG1 1
#define ANALOG2 2
#define ANALOG3 3
#define ANALOG4 4
#define ANALOG5 5

#define FIRMWAEW_ID 4  // ScratchBoard 1.1 Firmware ID

#define PWM1_PIN 6
#define PWM2_PIN 5
#define WAY1_PIN 7
#define WAY2_PIN 4

#define req_scratchboard 0  // request messge from Scratch
#define mask_scratcharduino 240  // request mask of Scratch+Ardunio
#define ch_analog0 0
#define ch_analog1 1
#define ch_analog2 2
#define ch_analog3 3
#define ch_analog4 4
#define ch_analog5 5
#define ch_button_rob 6
#define ch_firmware 15

// laboratory channels
#define ch_r_D 0
#define ch_r_C 1
#define ch_r_B 2
#define ch_button_lab 3
#define ch_r_A 4
#define ch_light 5
#define ch_sound 6
#define ch_slider 7

#define LAB 0
#define ROBOT 1
#define DEFAULT_MOTOR_POWER 200

byte device = ROBOT;
int sensorValue = 0;  // sensor value to send
byte inByte = 0;      // incoming serial byte
byte prevInByte = 0;  // previously received data
byte motorDirection = 0;
byte isMotorOn = 0;
byte motorPower = DEFAULT_MOTOR_POWER;

const byte SCRATCH_14_MODE = 0;
const byte BLUETOOTH_MODE = 1;
byte mode = SCRATCH_14_MODE;

const unsigned long ALIVE_TIME = 500;                  //in ms
const int MASK_CHANGE_MODE = (1<<0) + (1<<2) + (1<<4); //mask = 00010101
unsigned long lastReceiveMills = 0;
struct AnalogsState
{
    int firmwareId;
    int analogButton;
    int analog0;
    int analog1;
    int analog2;
    int analog3;
    int analog4;
    void clear()
    {
        firmwareId = 0;
        analog0 = 0;
        analog1 = 0;
        analog2 = 0;
        analog3 = 0;
        analog4 = 0;
        analogButton = 0;
    }
};
AnalogsState state;

void setup()
{
    // start serial port at 38400 bps:
    Serial.begin(38400);

    //find if we are running on robot or laboratory
    digitalWrite(14 + SOUND, HIGH);
    if (analogRead(SOUND) < 600 && analogRead(LIGHT) > 200)
    {
        device = LAB;
    }
    digitalWrite(14 + SOUND, LOW);

    switch (device)
    {
    case LAB:
        digitalWrite(14 + R_C, HIGH);
        digitalWrite(14 + R_D, HIGH);

        pinMode(clock, OUTPUT); // make the clock pin an output
        pinMode(data , OUTPUT); // make the data pin an output

        for(int i = 0; i < 8; ++i)
        {
            shiftOut(data, clock, MSBFIRST, 1 << i); // bit shift a logic high (1) value by i
            delay(100);
        }

        shiftOut(data, clock, LSBFIRST, B00000000);  // send this binary value to the shift register

        pinMode(BUTTON, INPUT);                      // digital sensor is on digital pin 2
        digitalWrite(BUTTON, HIGH);

        break;

    case ROBOT:
        pinMode(PWM1_PIN, OUTPUT);
        pinMode(PWM2_PIN, OUTPUT);
        pinMode(WAY1_PIN, OUTPUT);
        pinMode(WAY2_PIN, OUTPUT);

        break;
    }
}

void loop()
{
    /*
	  Firmware works in two modes: Bluetooth mode and Desktop mode.
	  Before starting of data communication with mobile app, mobile app must send bytes, 
	  which contain submask 'MASK_CHANGE_MODE' = 00010101 (in binary system).
	  If this mask was received - variable 'mode' is assigned 'BLUETOOTH_MODE', 'SCRATCH_14_MODE' - otherwise.
	  
	  If firmware is in state 'SCRATCH_14_MODE' (Desktop mode), it works as usual - like a previous version of firmware.
	  
	  If firmware is in state 'BLUETOOTH_MODE' (Mobile app mode), then measurements from analogs are normalized to 100%. 
	  Measurements are sent only if they changed since last time.

      Also in firmware added new functionality - every 500 ms desktop or mobile app must send keep-alive bytes, 
	  otherwise robot will stop. This functionality is made for safety of robot, when robot is controlled via mobile app.
	  This functionality was added in both modes.
	*/
	
    // if we get a valid byte, read analog ins:
    if (Serial.available() > 0)
    {
        // get incoming byte:
        inByte = Serial.read();
        //Serial.flush();
        if (inByte >= req_scratchboard)
        {
            if (mode == SCRATCH_14_MODE)
            {
                sendValue(ch_firmware, FIRMWAEW_ID);
                delay(10);
            }
            else if (mode == BLUETOOTH_MODE)
            {
                if (state.firmwareId != FIRMWAEW_ID)
                {
                    state.firmwareId = FIRMWAEW_ID;
                    sendValue(ch_firmware, FIRMWAEW_ID);
                }
            }

            switch (device)
            {
            case ROBOT:
                lastReceiveMills = millis();//remember last moment when data was received
                if (prevInByte != inByte)  //if previous bytes equal current - it isn't necessary send comand to motor
                {
                    prevInByte = inByte;//save previous inByte
                    motorDirection = (inByte >> 5) & B11;//motor direction
                    isMotorOn = inByte >> 7;//is motor on

                    if ((inByte & B11111) == MASK_CHANGE_MODE)//if received submask 'MASK_CHANGE_MODE' firmare switch to BLUETOOTH_MODE, otherwise to SCRATCH_14_MODE
                        mode = BLUETOOTH_MODE;
                    else
                        mode = SCRATCH_14_MODE;

                    switch (motorDirection)
                    {
                    case B11:
                        Motor1(motorPower * isMotorOn, false);
                        Motor2(motorPower * isMotorOn, false);
                        break;
                    case B01:
                        Motor1(motorPower * isMotorOn, false);
                        Motor2(motorPower * isMotorOn, true);
                        break;
                    case B10:
                        Motor1(motorPower * isMotorOn, true);
                        Motor2(motorPower * isMotorOn, false);
                        break;
                    case B00:
                        Motor1(motorPower * isMotorOn, true);
                        Motor2(motorPower * isMotorOn, true);
                        break;
                    }
                }

                // read  switch, map it to 0 or 1023L
                if (mode == SCRATCH_14_MODE)//if SCRATCH_14_MODE now (Desktop mode), then firmware work as usual, like a previous version of firmware. 
                {
                    sensorValue = map(digitalRead(BUTTON), 0, 1, 0, 1023);
                    sendValue(ch_button_rob, sensorValue);

                    sensorValue = analogRead(ANALOG0);
                    sendValue(ch_analog0, sensorValue);

                    sensorValue = analogRead(ANALOG1);
                    sendValue(ch_analog1, sensorValue);

                    sensorValue = analogRead(ANALOG2);
                    sendValue(ch_analog2, sensorValue);

                    sensorValue = analogRead(ANALOG3);
                    sendValue(ch_analog3, sensorValue);

                    sensorValue = analogRead(ANALOG4);
                    sendValue(ch_analog4, sensorValue);
                }
                else if (mode == BLUETOOTH_MODE)//if BLUETOOTH_MODE now (in other words - Mobile app mode) - then behavior of firmware has same differences. (described above)
                {
                    sensorValue = int(map(digitalRead(BUTTON), 0, 1, 0, 1023) / 1023.0 * 100);
                    if (state.analogButton != sensorValue)
                    {
                        state.analogButton = sensorValue;
                        sendValue(ch_button_rob, sensorValue);
                    }

                    sensorValue = int(analogRead(ANALOG0) / 1023.0 * 100);
                    if (state.analog0 != sensorValue)
                    {
                        state.analog0 = sensorValue;
                        sendValue(ch_analog0, sensorValue);
                    }

                    sensorValue = int(analogRead(ANALOG1) / 1023.0 * 100);
                    if (state.analog1 != sensorValue)
                    {
                        state.analog1 = sensorValue;
                        sendValue(ch_analog1, sensorValue);
                    }

                    sensorValue = int(analogRead(ANALOG2) / 1023.0 * 100);
                    if (state.analog2 != sensorValue)
                    {
                        state.analog2 = sensorValue;
                        sendValue(ch_analog2, sensorValue);
                    }

                    sensorValue = int(analogRead(ANALOG3) / 1023.0 * 100);
                    if (state.analog3 != sensorValue)
                    {
                        state.analog3 = sensorValue;
                        sendValue(ch_analog3, sensorValue);
                    }

                    sensorValue = int(analogRead(ANALOG4) / 1023.0 * 100);
                    if (state.analog4 != sensorValue)
                    {
                        state.analog4 = sensorValue;
                        sendValue(ch_analog4, sensorValue);
                    }
                }

                break;

            case LAB:
                sensorValue = map(digitalRead(BUTTON), LOW, HIGH, 1023, 0);
                sendValue(ch_button_lab, sensorValue);

                sensorValue = map(digitalRead(button_crest1), LOW, HIGH, 0, 1023);
                if (sensorValue < 1023)
                {
                    sensorValue = analogRead(R_A);
                }
                sendValue(ch_r_A, sensorValue);
                delay(10);

                sensorValue = map(digitalRead(button_crest2), LOW, HIGH, 0, 1023);
                if (sensorValue < 1023)
                {
                    sensorValue = analogRead(R_B);
                }
                sendValue(ch_r_B, sensorValue);
                delay(10);

                sensorValue = map(digitalRead(button_crest3), LOW, HIGH, 0, 1023);
                if (sensorValue < 1023)
                {
                    sensorValue = 1023 - analogRead(R_C);
                }
                sendValue(ch_r_C, sensorValue);
                delay(10);

                sensorValue = map(digitalRead(button_crest4), LOW, HIGH, 0, 1023);
                if (sensorValue < 1023)
                {
                    sensorValue = 1023 - analogRead(R_D);
                }
                sendValue(ch_r_D, sensorValue);
                delay(10);

                sensorValue = 1023 - analogRead(LIGHT);
                sendValue(ch_light, sensorValue);
                delay(10);

                sensorValue = 2 * analogRead(SOUND);
                sendValue(ch_sound, sensorValue);
                delay(10);

                sensorValue = 1023 - analogRead(SLIDER);
                sendValue(ch_slider, sensorValue);
                delay(10);

                break;
            }
        }
    }
	
	//keep-alive bytes check
    if (millis() - lastReceiveMills > ALIVE_TIME)  //if data isn't received last 500 ms - turn off motors
    {
        if (prevInByte != 0)
        {
            Motor1(0, false);   //turn off motor1
            Motor2(0, false);   //turn off motor2
        }
        prevInByte = 0;         //clear last received mask
        state.clear();          //clear information about measurements of analogs
        mode = SCRATCH_14_MODE; //set up Desktop mode
    }
}

void Motor1(int pwm, boolean reverse)
{
    analogWrite(PWM1_PIN, pwm); //set pwm control, 0 for stop, and 255 for maximum speed
    if (reverse)
    {
        digitalWrite(WAY1_PIN, HIGH);
    }
    else
    {
        digitalWrite(WAY1_PIN, LOW);
    }
}

void Motor2(int pwm, boolean reverse)
{
    analogWrite(PWM2_PIN, pwm);
    if(reverse)
    {
        digitalWrite(WAY2_PIN, HIGH);
    }
    else
    {
        digitalWrite(WAY2_PIN, LOW);
    }
}

void sendValue(byte channel, int value)
{
    byte high = 0;  // high byte to send
    byte low = 0;   // low byte to send
    high = (1 << 7) | (channel << 3) | (value >> 7);
    low =  (0xff >> 1) & value;
    Serial.write(high);
    Serial.write(low);
}
