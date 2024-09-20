#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SoftwareSerial.h>

#define CE_PIN   9
#define CSN_PIN 10
#define pinToReset 4

char bleData=0;
String command;
const byte rxPin=2;
const byte txPin=3;

SoftwareSerial bluetoothSerial (rxPin,txPin);
 
const byte slaveAddress[3][5] = {{'R','x','A','A','A'},
                                 {'R','x','A','A','B'},
                                 {'R','x','A','A','C'}};

RF24 radio(CE_PIN, CSN_PIN); 

int dataToSend[11]={0,0,0,0,0,0,0,0,0,0,0};
int ackData =0; // to hold the  two values coming from the slave
bool newData = false;
unsigned long lastMillis;
boolean last_button_state=0;
int start=0;
int mode=1;
bool ringBuzzer=0;
const int podNumbers=3;
int podNumber=1;
struct pods{
  int red;
  int green;
  int blue;
};
struct pods pod1;
struct pods pod2;
struct pods pod3;
boolean select_color=1;
int i=0;
bool readyToStart=0;
char color_pattern [21]={'0','0','0',';','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0',';'};
char pod_pattern [21]={'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0',';'};
char randomColor_list[8]={'1','2','3','4','5','6','7',';'};
char randomPod_list[podNumbers+1]={'1','2','3',';'};
bool random_color=1;
bool random_pod=1;
bool limited_steps=1;
int steps=9;
int step_counter=0;
int colorPattern_counter=0;
int podPattern_counter=0;


//===============

void setup() {

    Serial.begin(9600);
    bluetoothSerial.begin(9600);
    Serial.println(F("Source File /mnt/sdb1/SGT-Prog/Arduino/ForumDemos/nRF24Tutorial/SimpleTxAckPayload.ino"));
    Serial.println("SimpleTxAckPayload Starting");
    pinMode(pinToReset, OUTPUT);
    radio.begin();
    radio.setDataRate( RF24_250KBPS );

    radio.enableAckPayload();

    radio.setRetries(5,5); // delay, count
                                       
    radio.openWritingPipe(slaveAddress[0]);
}

//=============

void loop() {
//  Serial.println("mode = "+String(mode));
//  Serial.println("buzzer = "+String(ringBuzzer));
//    Serial.println("i = "+String(i));
//Serial.println(select_color);
    if(readyToStart==0){
      podProgramme();
    }
    else{
      updateMessage();
      send();
      showData();
    }
    
}

//================

void podProgramme(){
  if(bluetoothSerial.available()>0){
    bleData=bluetoothSerial.read();
    command=command + bleData;
    Serial.println(command);
    if (command.indexOf("mode1")>= 0){mode=1; command = "";}
    if (command.indexOf("mode2")>= 0){mode=2; command = "";}
    if (command.indexOf("mode3")>= 0){mode=3; command = "";}
    if (command.indexOf("onBuzzer")>= 0){ringBuzzer=1; command = "";}
    if (command.indexOf("offBuzzer")>= 0){ringBuzzer=0; command = "";} 
    if (command.indexOf("set")>= 0){readyToStart=1; command = "";}    
    delay(100);
  }
  
}
//================


void send() {    
    bool rslt;
    rslt = radio.write( &dataToSend, sizeof(dataToSend) );
    print_arr(dataToSend);
    if (rslt) {
        if ( radio.isAckPayloadAvailable() ) {
            radio.read(&ackData, sizeof(ackData));
            newData = true;
            
        }
        else {
            Serial.println("  Acknowledge but no data ");
        }
        
    }
    else {
        Serial.println(" Tx failed ");
    }

 }

//=================

void showData() {
    if (newData == true) {
        Serial.print("  Acknowledge data ");
        Serial.println(ackData);
        newData = false;
    }
}

//================

void updateMessage() {
  Serial.println(step_counter);
  if(bluetoothSerial.available()>0){
    bleData=bluetoothSerial.read();
    Serial.println(bleData);
    if(start==0 && bleData=='1'){
    start=1;
    i=0;
    select_color=1;
    }
    else if(start==1 && bleData=='0'){
      start=0;             
    }
    else if(bleData=='2'){
      readyToStart=0;
      start=0;
    }
  }
  
  else if(start==1 && mode==1){
    
    if(ackData==3){      
      if(select_color==1){
          step_counter=step_counter+1;
          if(random_pod==1){
            podNumber=select_prog_pod();
          }         
          if(podNumber==1){
            radio.openWritingPipe(slaveAddress[0]);
          }
          else if(podNumber==2){
            radio.openWritingPipe(slaveAddress[1]);
          }
          else if(podNumber==3){
            radio.openWritingPipe(slaveAddress[2]);
          }
          
          select_prog_color();
          
        
        select_color=0;
      }
      if(limited_steps==1){
        if(step_counter>steps){
          start=0; 
        }
        else{
          generate_output(podNumber,1);
        }
      }
      else{
        generate_output(podNumber,1);
      }
      
    }
    else if(ackData==4){
      select_color=1;
      
    }
  }
  else if(start==1 && mode==2){
    int blinking_time=2000;
    if(select_color==1){
        step_counter=step_counter+1;
        if(random_pod==1){
          podNumber=select_prog_pod();
        }
        if(podNumber==1){
          radio.openWritingPipe(slaveAddress[0]);
        }
        else if(podNumber==2){
          radio.openWritingPipe(slaveAddress[1]);
        }
        else if(podNumber==3){
          radio.openWritingPipe(slaveAddress[2]);
        }
      lastMillis=millis();
        select_prog_color();
      
      Serial.println("salam");
      select_color=0;
    }
    if(millis()-lastMillis>blinking_time){
      select_color=1;
      generate_output(podNumber,0);
      if(limited_steps==1){
        if(step_counter>=steps){
          start=0; 
        }
      }
    }
    else{
      generate_output(podNumber,2);
    }
    
 }
 else if(start==1 && mode==3){
    int blinking_time=2000;
    
    if(select_color==1){
      
      if(i==0){
        podNumber=1;
        select_prog_color();
        
        radio.openWritingPipe(slaveAddress[0]);
        
      }
      else if(i==1){
        radio.openWritingPipe(slaveAddress[1]);
        
      }
      else if(i==2){
        radio.openWritingPipe(slaveAddress[2]);
        
      }
      generate_output(1,2);
      i=i+1;
      if(i==3){
        select_color=0;
        lastMillis=millis();
        i=0;
        step_counter=step_counter+1;
      }
      
    }
    else if(millis()-lastMillis>blinking_time){
      
      if(i==0){
        radio.openWritingPipe(slaveAddress[0]); 
      }
      else if(i==1){
        radio.openWritingPipe(slaveAddress[1]);
      }
      else if(i==2){
        radio.openWritingPipe(slaveAddress[2]);
      }
      generate_output(1,0);
      i++;
      if(i==3){
        select_color=1;
        i=0;
        if(limited_steps==1){
          if(step_counter>=steps){
            start=0; 
          }
        }
      }
    }
    else{
      generate_output(1,2);
    }
 }

 if (start==0){
    step_counter=0;
    colorPattern_counter=0;
    if(mode==3){
      if(i==0){
        radio.openWritingPipe(slaveAddress[0]);
      }
      else if(i==1){
        radio.openWritingPipe(slaveAddress[1]);
        
      }
      else if(i==2){
        radio.openWritingPipe(slaveAddress[2]);
      }
      generate_output(1,0);
      i++;
      if(i==3){
        i=0;
      }
    }
    else{
      generate_output(1,0);
    }
  }
}

//==============

void select_prog_color(){
  int color;
  if(color_pattern[colorPattern_counter]!=';'){
    color=int(color_pattern[colorPattern_counter])-48;
    colorPattern_counter=colorPattern_counter+1;
  }
  else{
    colorPattern_counter=0;
    color=int(color_pattern[colorPattern_counter])-48;
    colorPattern_counter=colorPattern_counter+1;
  }
  if(color==0){
    int L=0;
    for(int j = 0; j < 10; j++){
      if(randomColor_list[j]==';'){
        break;
      }
      else{
        L++;
      }
    }
    color=int(randomColor_list[random(0,L)])-48;
  }
  switch(color){
    case 1:
      if(podNumber==1){
        pod1.red=255;
        pod1.green=0;
        pod1.blue=0;
      }
      else if(podNumber==2){
        pod2.red=255;
        pod2.green=0;
        pod2.blue=0;
      }
      else{
        pod3.red=255;
        pod3.green=0;
        pod3.blue=0;
      }
      break;
      
    case 2:
      if(podNumber==1){
        pod1.red=0;
        pod1.green=255;
        pod1.blue=0;
      }
      else if(podNumber==2){
        pod2.red=0;
        pod2.green=255;
        pod2.blue=0;
      }
      else{
        pod3.red=0;
        pod3.green=255;
        pod3.blue=0;
      }
      break;

    case 3:
      if(podNumber==1){
        pod1.red=0;
        pod1.green=0;
        pod1.blue=255;
      }
      else if(podNumber==2){
        pod2.red=0;
        pod2.green=0;
        pod2.blue=255;
      }
      else{
        pod3.red=0;
        pod3.green=0;
        pod3.blue=255;
      }
      break;

    case 4:
      if(podNumber==1){
        pod1.red=255;
        pod1.green=255;
        pod1.blue=0;
      }
      else if(podNumber==2){
        pod2.red=255;
        pod2.green=255;
        pod2.blue=0;
      }
      else{
        pod3.red=255;
        pod3.green=255;
        pod3.blue=0;
      }
      break;

    case 5:
      if(podNumber==1){
        pod1.red=255;
        pod1.green=0;
        pod1.blue=255;
      }
      else if(podNumber==2){
        pod2.red=255;
        pod2.green=0;
        pod2.blue=255;
      }
      else{
        pod3.red=255;
        pod3.green=0;
        pod3.blue=255;
      }
      break;

    case 6:
      if(podNumber==1){
        pod1.red=0;
        pod1.green=255;
        pod1.blue=255;
      }
      else if(podNumber==2){
        pod2.red=0;
        pod2.green=255;
        pod2.blue=255;
      }
      else{
        pod3.red=0;
        pod3.green=255;
        pod3.blue=255;
      }
      break;

    case 7:
      if(podNumber==1){
        pod1.red=255;
        pod1.green=255;
        pod1.blue=255;
      }
      else if(podNumber==2){
        pod2.red=255;
        pod2.green=255;
        pod2.blue=255;
      }
      else{
        pod3.red=255;
        pod3.green=255;
        pod3.blue=255;
      }
      break;
  }
}

//==============

int select_prog_pod(){
  int number=int(pod_pattern[podPattern_counter])-48;
  if(number==0){
    int L=0;
    for(int j = 0; j < 10; j++){
      if(randomPod_list[j]==';'){
        break;
      }
      else{
        L++;
      }
    }
    number=int(randomPod_list[random(0,L)])-48;
  }
  return number;
}

//==============

void generate_output(int podNumber,int state){
  int output[10];
  int red;
  int green;
  int blue;
  switch(podNumber){
    case 1:
      red=pod1.red;
      green=pod1.green;
      blue=pod1.blue;
      break;
    case 2:
      red=pod2.red;
      green=pod2.green;
      blue=pod2.blue;
      break;
    case 3:
      red=pod3.red;
      green=pod3.green;
      blue=pod3.blue;
      break;
  }
  if(state==0){
    dataToSend[0]=0;
    dataToSend[1]=0;
    dataToSend[2]=0;
    dataToSend[3]=0;
    dataToSend[4]=0;
    dataToSend[5]=0;
    dataToSend[6]=0;
    dataToSend[7]=0;
    dataToSend[8]=0;
    dataToSend[9]=0;
    dataToSend[10]=0;
  }
  else if(state==1){
    dataToSend[0]=1;
    dataToSend[3]=red%10;
    dataToSend[2]=(red/10)%10;
    dataToSend[1]=(red/100)%10;
    dataToSend[6]=green%10;
    dataToSend[5]=(green/10)%10;
    dataToSend[4]=(green/100)%10;
    dataToSend[9]=blue%10;
    dataToSend[8]=(blue/10)%10;
    dataToSend[7]=(blue/100)%10;
    dataToSend[10]=ringBuzzer;
  }
  else if(state==2){
    dataToSend[0]=2;
    dataToSend[3]=red%10;
    dataToSend[2]=(red/10)%10;
    dataToSend[1]=(red/100)%10;
    dataToSend[6]=green%10;
    dataToSend[5]=(green/10)%10;
    dataToSend[4]=(green/100)%10;
    dataToSend[9]=blue%10;
    dataToSend[8]=(blue/10)%10;
    dataToSend[7]=(blue/100)%10;
    dataToSend[10]=ringBuzzer;
  }
  
}

//==============

int countDigits(int num){
  int count=0;
  while(num){
    num=num/10;
    count++;
  }
  return count;
}

//==============

void print_arr(int myArray[10]){
  Serial.print("data sent = ");
  for(int i = 0; i < 10; i++){
    Serial.print(myArray[i]);
  }
  Serial.println();
}

//====================
