




//GLOBAL VARIABLES and definitions

#byte X_EDGE_PORT =0x05  // Those are the port definitions for each edge
#byte Y_EDGE_PORT =0x06    // port number will depends of used microcontroller
#byte Z_EDGE_PORT =0x07  //

unsigned int32 TOTAL_PULSEQ  // Pulse quantity to do on the previously stablished  Edge and direction. 
                         // That variable will be the target for a FOR statement until arrive 
                         // to total value, then it will be overwrited by the next ORDER to execute.
                         
                         

/****************************************************************************************/

// POINTER AND DECODING FOR EACH EDGE

//  DECLARING POINTERS AND SEQUENCES
// The pointers will set what part of the sequence will be send to the output 
// depending of the stablished direction or last position. 
// 
// The sequences are wrotten separately in order to manage different sequences independently 
//  

int SEQ_X[8]=0X01,0X0C,0X04,0X06,0X02,0X03,0X01,0X09;
int SEQ_Y[8]=0X01,0X0C,0X04,0X06,0X02,0X03,0X01,0X09;
int SEQ_Z[8]=0X01,0X0C,0X04,0X06,0X02,0X03,0X01,0X09;

int SEQ_POINTER[3]=1,1,1;  // THIS IS THE VALUE FOR POINTERS X,Y,Z

/****************************************************************************************/


//  POINTER_UPDATER function will manage the SEQ_POINTERS , IT increases and decreases 
//   the values of all Sequence pointer variables
//   --> SEQ_POINTER[1](x), SEQ_POINTER[2](y) and SEQ_POINTER[3](z) 
//  and takes care about that values will be in the range between 1 and 8, because, those
//  are the positions in the stepper

//SELECTED_POINTER = 1 -> X_POINTER is selected
//SELECTED_POINTER = 2 -> Y_POINTER is selected
//SELECTED_POINTER = 3 -> Z_POINTER is selected

// DIRECTION =0 -> RIGHT movement
// DIRECTION =1 -> LEFT movement


// POINTER_UPDATER updater function must be ejecuted once per Pulse. At the end of each tracing instruction

void POINTER_UPDATER (int1 DIRECTION,int SELECTED_POINTER) {

   switch(DIRECTION){

      case 0: 
         if (SEQ_POINTER[SELECTED_POINTER]== 8){   //check the last value
         SEQ_POINTER[SELECTED_POINTER]= 1; break;  // reset count if needed   
         }else{
         SEQ_POINTER[SELECTED_POINTER]++; break;
         }

      case 1: 
         if (SEQ_POINTER[SELECTED_POINTER]== 1){  //check the last value
         SEQ_POINTER[SELECTED_POINTER]= 8; break; // reset count if needed   
         }else{
         SEQ_POINTER[SELECTED_POINTER]--; break;
         }
   }
}

// note: only used inside MOVEMENT_ACTUATOR function
// EXAMPLES:

POINTER_UPDATER (0,2);    // will increase once the value of the Y pointer (SEQ_POINTERS[2]) 
                  //(if it was 8 next will be 1)
POINTER_UPDATER (1,1);    // will decrease once the value of the X pointer (SEQ_POINTERS[1]) 
                  //(if it was 8 next will be 1)                


/****************************************************************************************/
// ALGORITHM_INTERPRETER function 

// That function will set the value of TOTAL_PULSEQ variable value, in order to decode data from bits
// 4,5,6 and 7 from the order to execute. 
// Value must be less than 4294967295 if declared as int32 , or les than  65535 if it declared as int16
// depending completely of the microcontroller to use.

 void ALGORITHM_INTERPRETER (int16 OQUANT, int OMULT, int OADD) {

TOTAL_PULSEQ = (OQUANT*OMULT)+OADD;
 
}



/****************************************************************************************/

// MOVEMENT ACTUATOR 

// Movement actuator function will operate once per order execution and all the movement task
// will happen inside it. 
//Movement actuator function function will use any other function in order to comply the main task, 
// READ + INTERPRET +DECODE + SET the values from the Buffer Ram to the TTL outputs
// IMPORTANT! Always , ALGORITHM_INTERPRETER function must be executed before each ejecution of MOVEMENT_ACTUATOR

void MOVEMENT_ACTUATOR (bool HOLD, bool DIRECTION, int SELECTED_POINTER, int32 TOTAL_PULSEQ){
int32 MOVEMENT_ACTUATOR_COUNTER // datatype must be the same of MOVEMENT_ACTUATOR

   for MOVEMENT_ACTUATOR_COUNTER = 1 to TOTAL_PULSEQ {            // Sets the number of times that is repeated the selected action 
   
         Switch (SELECTED_POINTER){
            case 1:X_EDGE_PORT= SEQ_X[SEQ_POINTER[1]]; break;   // Establishes at outports values that each SEQ_POINTER points 
            case 2:Y_EDGE_PORT= SEQ_Y[SEQ_POINTER[2]]; break;   //into the SEQ_* array of each edge
            case 3:Z_EDGE_PORT= SEQ_Z[SEQ_POINTER[3]]; break;
         }
      POINTER_UPDATER (DIRECTION,SELECTED_POINTER);             // Updates the used pointer 
   }
   
   IF (HOLD==0){
         Switch (SELECTED_POINTER){
            case 1:X_EDGE_PORT= 0X00; break;   // Value can be 0xff it depends of the
            case 2:Y_EDGE_PORT= 0X00; break;   // logic used in Power State
            case 3:Z_EDGE_PORT= 0X00; break;   //  idea is set the coils free
         }
   }


}
// EXAMPLES:
MOVEMENT_ACTUATOR (1,0, 2, 4500)   // it will move to the right, the Y edge,  4500 pulses and Hold that value at output
MOVEMENT_ACTUATOR (0,1, 1, 200)      // it will move to the left, the x edge,  200 pulses and set the coils free without providing  current to them 

/****************************************************************************************/



