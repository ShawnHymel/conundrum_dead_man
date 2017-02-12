#include "HX711.h"

#define DOUT  3 // Scale Digital Line
#define CLK  2 // Scale Clock Line

#define POWER_DETECT A3 // Detect if the cabinet has been activated

#define AUDIO_1 A0 // Pull low to trigger WAV001
#define AUDIO_2 A1 // Pull low to trigger WAV002
#define AUDIO_3 A2 // Pull low to trigger WAV003

// Magnetic Door Latches
#define CAB_1_LATCH 4
#define CAB_2_LATCH 5
#define CAB_3_LATCH 6

// Cabinet Lights
#define CAB_1_LIGHTS 7
#define CAB_2_LIGHTS 8
#define CAB_3_LIGHTS 9

// Auxilary Lights
#define AUX_1_LIGHT 10
#define AUX_2_LIGHT 11

// Reed Switches
#define CAB_1_REED 12
#define CAB_2_REED 13

int puzzleState = 0;
bool newState = 0;

int sound_1_duration = 5000; // Length of SFX1 in milliseconds
int sound_2_duration = 5000; // Length of SFX2 in milliseconds
int sound_3_duration = 5000; // Length of SFX3 in milliseconds

HX711 scale(DOUT, CLK); // Scale object
float calibration_factor = 450000; // Scale calibration factor

void setup() {

  /*-------------------Scale Setup-------------------*/
  scale.set_scale(calibration_factor); //Adjust to this calibration factor  
  scale.tare(); // Reset the scale to 0
  long zero_factor = scale.read_average(); //Get a baseline reading
  /*-------------------------------------------------*/

  /*------------------I/O Pin Setup------------------*/  
  pinMode(POWER_DETECT, INPUT); // Requires an external pull-down
  pinMode(CAB_1_REED, INPUT); digitalWrite(CAB_1_REED, 1); // Internal pull-up 
  pinMode(CAB_2_REED, INPUT); digitalWrite(CAB_2_REED, 1);

  pinMode(AUDIO_1, OUTPUT); digitalWrite(AUDIO_1, 1); // Set high, pull low to activate
  pinMode(AUDIO_2, OUTPUT); digitalWrite(AUDIO_2, 1);
  pinMode(AUDIO_3, OUTPUT); digitalWrite(AUDIO_3, 1);
  pinMode(CAB_1_LATCH, OUTPUT); digitalWrite(CAB_1_LATCH, 1); // Pull low to de-activate
  pinMode(CAB_2_LATCH, OUTPUT); digitalWrite(CAB_2_LATCH, 1); 
  pinMode(CAB_3_LATCH, OUTPUT); digitalWrite(CAB_3_LATCH, 1); 
  pinMode(CAB_1_LIGHTS, OUTPUT); digitalWrite(CAB_1_LIGHTS, 0); // Set low, pull up to activate
  pinMode(CAB_2_LIGHTS, OUTPUT); digitalWrite(CAB_2_LIGHTS, 0); 
  pinMode(CAB_3_LIGHTS, OUTPUT); digitalWrite(CAB_3_LIGHTS, 0); 
  pinMode(AUX_1_LIGHT, OUTPUT); digitalWrite(AUX_1_LIGHT, 0); // Set low, pull up to activate
  pinMode(AUX_2_LIGHT, OUTPUT); digitalWrite(AUX_2_LIGHT, 0); 
  /*-------------------------------------------------*/
  
}

void loop() {

  // On powerup, do nothing until power detect goes high
  while(!digitalRead(POWER_DETECT)){}; 

  /*then*/
  
  advancePuzzle();

  /*---------------Cabinet 1 Open--------------------*/

  while(puzzleState==1){
  // In this state, the top cabinet is open exposing the scale mechanic.
  // The CAB_1_LIGHTS and AUX_1_LIGHT are on and the CAB_1_LATCH
  // is inactive. We advance to state 2 when the CAB_1_REED is LOW and 
  // scale.get_units() is between 0.583 and 0.585

    if(newState){// When first entering this state set the conditions
      digitalWrite(CAB_1_LATCH, 0); // Unlock Cabinet 1
      digitalWrite(CAB_1_LIGHTS, 1); // Activate Cabinet 1 Lights
      digitalWrite(AUX_1_LIGHT, 1); // Activate Aux 1 Light
      newState = 0;
    }

    if((!digitalRead(CAB_1_REED)) && (0.583 < scale.get_units() < 0.585)){
      advancePuzzle();
    }
    
    delay(500); // Don't spam the scale too hard
    
    }

  /*---------------Cabinet 2 Open--------------------*/
  
  while(puzzleState==2){
  // In this state, the middle cabinet is open exposing the item platforms.
  // The CAB_2_LIGHTS and AUX_2_LIGHT are on and the CAB_2_LATCH
  // is inactive. We advance to state 3 when the CAB_2_REED is LOW

    if(newState){// When first entering this state set the conditions
      digitalWrite(CAB_1_LATCH, 1); // Lock Cabinet 1
      digitalWrite(CAB_2_LATCH, 0); // Unock Cabinet 2
      digitalWrite(CAB_1_LIGHTS, 0); // Dectivate Cabinet 1 Lights
      digitalWrite(CAB_2_LIGHTS, 1); // Activate Cabinet 2 Lights
      digitalWrite(AUX_2_LIGHT, 1); // Activate Aux 2 Light
      newState = 0;
    }

    if(!digitalRead(CAB_1_REED)){
      advancePuzzle();
    }
    
    delay(250); // Feels silly to check every few microseconds
    
    }  

  /*---------------Cabinet 3 Open--------------------*/

  while(puzzleState==3){
  // This is the SOLVED state. Both auxilary lights are lit
    if(newState){// When first entering this state set the conditions
      digitalWrite(CAB_2_LATCH, 1); // Lock Cabinet 2
      digitalWrite(CAB_2_LIGHTS, 0); // Dectivate Cabinet 2 Lights
      digitalWrite(CAB_3_LIGHTS, 1); // Activate Cabinet 3 Lights
      newState = 0;
    }

    // The puzzle is solved, we stay here until reset

    // The puzzle is solved, we stay here until reset    
  }


}

void advancePuzzle() {

  puzzleState++; // Advance the puzzle state
  newState = 1; // This flag lets the next state loop know it's on loop 1

  // Trigger the appropriate sound file and wait before advancing to next state loop
  switch (puzzleState) {

    case 1:
      digitalWrite(AUDIO_1, 0);
      delay(250);
      digitalWrite(AUDIO_1, 1);
      delay(sound_1_duration);
      break;

    case 2:
      digitalWrite(AUDIO_2, 0);
      delay(250);
      digitalWrite(AUDIO_2, 1);
      delay(sound_2_duration);
      break;

    case 3:
      digitalWrite(AUDIO_3, 0);
      delay(250);
      digitalWrite(AUDIO_3, 1);
      delay(sound_3_duration);      
      break;

  }
  
}

