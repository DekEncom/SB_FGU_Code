#include <Servo.h>
#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>


Servo myservo;  
// Config Firebase
#define FIREBASE_HOST "dbtest-3d359.firebaseio.com"
#define FIREBASE_AUTH "E66Eqkdc0IMgxeMhn18NekVAnRo4VMRgVNdIkIIU"

// Config connect WiFi
#define WIFI_SSID "ESP_Test"
#define WIFI_PASSWORD "123456789"


const byte interruptPin = D2;
const byte buttonPin = A0;
int Buzzer = D4; 
int IF_1; 
int IF_2;
int Number = 562000;
int i = 0;
int buttonState;
int Count = 0;
int state = 0;
int Status;
int Round =1;

void Count_Bot(){
    state = 1;
}

void beep(){
      digitalWrite(Buzzer , HIGH);
      delay(50);
      digitalWrite(Buzzer , LOW);
      delay(50);
} 

void Long_beep(){
      digitalWrite(Buzzer , HIGH);
      delay(500);
      digitalWrite(Buzzer , LOW);
      delay(500);
} 

// Define Firebase parameter
String *Pass, *List ;
String Name ; 
int rnd = 1;
int State = 0;
int PW,GPW;


void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);                                        //เชื่อมต่อ Wifi
    WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
    Serial.println("\nConnecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
          Serial.print("..");
           delay(1000);
    }
     Serial.println("\nWiFi Connecting  ");
     Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
     delay(10);
    // Serial.println("Enter your password: "); 

    pinMode(D1, INPUT);
    pinMode(D2, INPUT);
    pinMode(D3, OUTPUT);
    pinMode(Buzzer, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(interruptPin), Count_Bot , CHANGE);
    myservo.attach(D3);
}

void loop() {
     // put your main code here, to run repeatedly:

     if(State == 0){
        State = 1;
        Serial.println("Enter your password: ");
       
      
      }
     else if(State == 1){         
          while(Serial.available()) {  
            Serial.println("...");            
            Pass  = Serial.readString();// read the incoming data as string
            Serial.print("Your password is :   ");
            Serial.println(Pass); 
          //  strcpy(Test , Pass);
           PW = atoi(Pass);
            //Getdata_FB(Pass); 
            State = 2;
           
     }
          
     }
     else if(State == 2){
        while(rnd > 0){
         Serial.print(rnd);
           FirebaseObject Data = Firebase.get("Userdetail/ID/"+ String(rnd));
            //Firebase.setInt("Userdetail/ID/1/LogDate/" , Count);
           if (Firebase.failed()) {
                  Serial.print("get Password failed:");
                  Serial.println(Firebase.error());  
                  return;
           }      
          else{
                 Serial.println("get Password OK  ");
                 List = Data.getString("Password/");
                // Name = Data.getString("fname/");
                /* const char *Str1 = List;
                 const char *Str2 = Pass;
                 int ret = strcmp( &Str1,&Str2 );*/
                 GPW = atoi(List);
                 //Serial.println(List);
                 rnd++;
                 /* char* str[6];
                  strcpy(str, Pass) ; 
                  int ret = strcmp(List,str);
                 */
                  if(GPW == PW){  
                         Serial.print(rnd);
                         Serial.println("  <== Log In Success");  
                         Serial.print(Name);
                         Serial.print(" : ");  
                         Serial.println(List);
                         delay(500); 
                         rnd = 0; 
                                                               
                  }
                                                                 
          }   
             
      }State = 0;
      
       // WriteData_FB(String(rnd));
      // delay(1000);
   //  State = 3;
       } 
      
     
}


/*String EnterPass(){
  while(Serial.available()) {             
            Pass  = Serial.readString();// read the incoming data as string
            Serial.print("Your password is :   ");
            Serial.println(Pass);                         
            //Getdata_FB(Pass); 
            State = 2;
             return Pass; 
     }
      //Serial.println("Enter your password: ");
   // 
}*/

/*void Getdata_FB(String PATH){
    
}*/

void WriteData_FB(String id){
     IF_1 =  digitalRead(D1);
     buttonState = analogRead(buttonPin);
    //FirebaseObject Data = Firebase.get("Userdetail/ID/"+ String(rnd));
    
     // myservo.write(180);
     // Serial.println("Ready to Insert");
   //  while(Round != 0){
     if(IF_1 == 0){
           delay(2000);
           myservo.write(90);
           Serial.println("Servo On");
           beep();
           delay(50);
           beep();
           delay(700);
           myservo.write(180);
           Serial.println("Servo Off");
           Long_beep();
 //          delay(200);
 
           
          }
     
          switch (state) {
             case 1:{
                   // beep();
                   // delay(500);
                  //  beep();
                    Count++;
                   // Serial.print("Stat: ");
                   // Serial.println(state);
                    Serial.print("Count: ");
                    Serial.println(Count);
                    state = 0;
                    delay(500);
                    // Code
             }break;
             default:
                   // Serial.println(state);// Code
                break;
              if(buttonState > 1000){
                   // myservo.write(180);
                   //Serial.println("Ready to Insert");
                   Firebase.setInt("Userdetail/ID/"+ id + "LogBottle/" , Count);
                   if (Firebase.failed()) {
                        Serial.print("set Data failed:");
                       Serial.println(Firebase.error());  
                       return ;
                   } 
                   Serial.println("Set Data OK  ");
                   delay(1000);
                  // Round = 0;
                   Status  = 1;
             }
   
        }//Round++;
     }

