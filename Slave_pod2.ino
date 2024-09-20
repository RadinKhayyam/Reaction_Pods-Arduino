#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN   8
#define CSN_PIN 9

const byte thisSlaveAddress[5] = {'R','x','A','A','B'};

RF24 radio(CE_PIN, CSN_PIN);

int dataReceived[11]={0,0,0,0,0,0,0,0,0,0,0};  
int ackData= 5; 
bool newData = false;

#define redPin  6       
#define greenPin 10     
#define bluePin  5
#define buzzer 2
boolean button_state = 0;
boolean led_on=0;
struct color{
  int red;
  int green;
  int blue;
};
struct color pod_color;
int start=0;
int zero[10]={0,0,0,0,0,0,0,0,0,0};
int one[10]={1,0,0,0,0,0,0,0,0,0};
bool ledOff= 0;
bool buzz=1;


//==============

void setup() {
    turn_led_off();
    pinMode(redPin,OUTPUT);
    pinMode(greenPin,OUTPUT);
    pinMode(bluePin,OUTPUT);
    pinMode(buzzer,OUTPUT);
    Serial.begin(9600);

    Serial.println("SimpleRxAckPayload Starting");
    radio.begin();
    radio.setDataRate( RF24_250KBPS );
    radio.openReadingPipe(1, thisSlaveAddress);

    radio.enableAckPayload();
    
    radio.startListening();

    radio.writeAckPayload(1, &ackData, sizeof(ackData)); // pre-load data
}

//==========

void loop() {
  unsigned long sensor_value = analogRead(A0); 
  Serial.println(sensor_value);
    getData();
    showData();
}

//============

void getData() {
    if ( radio.available() ) {
        Serial.println("salam");
        radio.read( &dataReceived, sizeof(dataReceived) );
        updateReplyData();
        
        newData = true;
    }
}

//================

void showData() {
    if (newData == true) {
        
        print_arr(dataReceived);
        Serial.println(ackData);
        newData = false;
    }
}

//================

void updateReplyData() {
  int mode=dataReceived[0];
  bool ringBuzzer=dataReceived[10];
  if(twoArrEqual(dataReceived,zero)){
    turn_led_off();
    ackData=3;
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); 
    buzz=1;
  }
  else if(mode==1){
    
    if(is_touched()==0 ){
      turn_led_off();
      ledOff=1;
      if(ringBuzzer==1){
        tone(buzzer,3000,200);
      }
      ackData=3;
      radio.writeAckPayload(1, &ackData, sizeof(ackData)); 
    } 
    else {
      process_input(dataReceived);
      if(ledOff==0){
        turn_led_on(pod_color.red, pod_color.green, pod_color.blue);
      }
      ledOff=0; 
      ackData=4;
      radio.writeAckPayload(1, &ackData, sizeof(ackData)); 
    }
  }
  else if(mode==2){

    process_input(dataReceived);
    turn_led_on(pod_color.red, pod_color.green, pod_color.blue);
    ackData=4;
    radio.writeAckPayload(1, &ackData, sizeof(ackData)); 
    if(ringBuzzer==1 && buzz==1){
      tone(buzzer,3000,200);
      buzz=0;
    }
    
  }
    
}

//================

void turn_led_on(int red,int green, int blue){
  analogWrite(redPin,255-red);
  analogWrite(greenPin,255-green);
  analogWrite(bluePin,255-blue);
}

//================

void turn_led_off(){
  analogWrite(redPin,255);
  analogWrite(greenPin,255);
  analogWrite(bluePin,255);
}

//================

void process_input(int input[10]){
  
  int red=input[1]*100+input[2]*10+input[3];
  int green=input[4]*100+input[5]*10+input[6];
  int blue=input[7]*100+input[8]*10+input[9];
  pod_color.red=red;
  pod_color.green=green;
  pod_color.blue=blue; 
  
}

//================

int countDigits(int num){
  int count=0;
  while(num){
    num=num/10;
    count++;
  }
  return count;
}

//================

bool twoArrEqual(int arr1[], int arr2[]) 
{ 
    if (sizeof(arr1) != sizeof(arr2)) 
        return false;
    for (int i = 0; i < sizeof(arr1); ++i){
        if(arr1[i] != arr2[i]){ return false; }
    }
    return true;
}

//================

void print_arr(int myArray[10]){
  for(int i = 0; i < 10; i++){
    Serial.print(myArray[i]);
  }
  Serial.println();
}

//================

int is_touched(){
  unsigned long sensor_value = analogRead(A0); 
  if(sensor_value>350){
    return 0;
  }
  else{
    return 1;
  }
}
