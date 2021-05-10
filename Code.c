#include <MotorCtrl.h>
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal LCD(46, 44, 42, 40, 38, 36, 34, 32, 30, 28); //RS, EN, Data (D0-D7)



MotorCtrl m;


int tickCounter = 0;
int timerCount = 0;
double revCount = 0;
double rpm = 0;
double refspeed = 0; 
double dutyPf = 0;
double dutyPfIf = 0;
double dutyPfIfDf = 0;



double Kp = 0.01 ;
double Ki = 1;
double Kd = 0.25;


double Pf= 0 ;
double If = 0;
double Df = 0;
double olderror = 0;
double error = 0 ;



void counter();

void setup(){
  Serial.begin(9600);

  // set up the LCD
  LCD.begin(16, 2);    // 2-line, 16-character display
  LCD.clear();         // must be called to refresh display
 
  // Display welcome message
  LCD.setCursor(0, 0);      // first column, first row
  LCD.print("Ref Speed"); 
  LCD.setCursor(0, 1);      
  LCD.print("Act Speed"); 
 
// Set motor frequency.
// Parameters: FREQ  = Desired Frequency
//   DUTY_CYCLE = Duty cycle of the PWM signal in Percentage

  
//  delay(1000);
  
  m.Motor_Setup();
  m.Motor_Encoder_Interrupt(counter);
  m.Motor_Timer(1999);

}




void loop(){ 

// sets analog reference as external
// gets analog value from POT to set motor frequency
// Returns an 'int'
// Parameters: desiredBottomValue = maps the bottom value
//     : desiredTopValue = maps the top value

  double duty = ( m.Motor_AnalogInput(10,80) );
  refspeed = ((duty - 10) * 40.857) + 2500 ;
 
  error = 0.97*refspeed - (rpm);

  Pf = error * Kp ;
 
   dutyPf = duty + Pf;
   
 if (Pf >= 10){
    Pf = 10; 
 }
 if (Pf <= -10){
     Pf = -10;
 }
 
 if( dutyPf< 0){
     dutyPf = 0;
 }
 

 //////////Integral//////////
 
If = If + Ki*error;

if(If > 20){
   If = 0;
}
if(If < -20){
   If = 0;
}

dutyPfIf = dutyPf + If;

if(dutyPfIf < 0 ){
  dutyPfIf = 0;
}

////////derivative //////////

Df = (error - olderror) * Kd;

dutyPfIfDf = dutyPfIf + Df;

double olderror = error;

if(Df<-10){
  Df = -10;
}
if(Df>10){
  Df = 10;
}
if (dutyPfIfDf<0){
    dutyPfIfDf = 0;
}
///////////////////////////

m.Motor_PWM(400, dutyPfIfDf);



// print out the value you read:
  Serial.println( error);

  LCD.setCursor(11, 0);
  LCD.print(refspeed); 
  LCD.setCursor(11, 1);
  LCD.print(rpm); 


if (tickCounter % 20 == 0){
     revCount++;
  }
  
}
// Interrupt Service Routine for Timer0 interrupts
// Needed by the MotorCtrl library - do not remove
ISR(TIMER0_COMPA_vect){
  if (timerCount >99){
    rpm = tickCounter * 60;
    tickCounter=0;
    timerCount=0;
  }
timerCount++;




if(300<If<-300){
    If = 0;    
  }
}

void counter() {
  tickCounter++;
}
