//www.elegoo.com
//2016.06.13

#define SENSOR_THR 400
#define TREADMILL_LENGTH 125.0
#define INCHES_PER_MILE 63360.0
#define TREADMILL_LAP (TREADMILL_LENGTH / INCHES_PER_MILE)
#define INCH_PER_SEC_TO_MPH 17.6
#define CONVERSION ((TREADMILL_LENGTH * 1000.0) / INCH_PER_SEC_TO_MPH)
#define DEBOUNCE_TIME 1

int analog_input_pin = A0;

// edge variables
char last_state     = 0;
char edge_state     = 0;
char edge_detected  = 0;
char edge_reading   = 0;

// distance variables
int lap_count       = 0;
float mph           = 0.0;
float total_time    = 0.0;
float distance      = 0.0;

// time variables
unsigned long last_debounce_time = 0;
unsigned long last_pulse_time    = 0;
unsigned long delta_t            = 0;

void  setup() {
    pinMode(LED_BUILTIN,OUTPUT); // define LED as a output port
    pinMode(analog_input_pin, INPUT); // define A0 as input
    Serial.begin(9600);  // set baud rate
}

char sensor_hysteresis(int sensor_reading) {
    if(sensor_reading < SENSOR_THR) {
        return 1;
    }
    else {
        return 0;
    }
}

void loop() { 
    int sensor_reading = analogRead(analog_input_pin); 

    edge_reading = sensor_hysteresis(sensor_reading);

    if(edge_reading != last_state) {
        last_debounce_time = millis();
    }

    if((millis() - last_debounce_time) > DEBOUNCE_TIME) {
        if(edge_reading != edge_state) {
            edge_state = edge_reading; // the edge has changed, store the new state
            // falling edge, only want to count one edge
            if(edge_state == 0) {  
//                Serial.println("White to black transition");
                lap_count++;  // increase laps counted
                delta_t     = millis() - last_pulse_time;  // time since last edge transition
                total_time  += delta_t;  // total time in milliseconds
                distance    = lap_count * TREADMILL_LAP;  // # of laps times length of treadmill
                mph         = CONVERSION / float(delta_t);  // convert from inches per sec to mph
                last_pulse_time = millis();  //save the new time since the last pulse

                // run screen for serial logging
                // any terminal works except the arduino IDE
                // screen /dev/tty.usbmodem14101 9600
                // don't judge me mike, this is quick and dirty
                Serial.write(27);       // ESC command
                Serial.print("[2J");    // clear screen command
                Serial.write(27);       // ESC command
                Serial.print("[H");     // cursor to home command
                Serial.print("Total distance: ");
                Serial.println(distance);
                Serial.print("MPH: ");
                Serial.println(mph);
                Serial.print("Time elapsed: ");
                Serial.println(total_time / 1000.0);
            }
            else {
                // if you wanted to do rising edge logic instead
//                Serial.println("Black to white transition");
            }
        }
    }
    last_state = edge_reading;  // grab the last edge reading
}
