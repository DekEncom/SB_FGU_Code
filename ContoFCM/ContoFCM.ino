#include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>
#include <Servo.h>
#include <ESP8266WiFi.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h> //ประกาศ Library ของจอ I2C
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Config Firebase 
#define FIREBASE_HOST "sb-fgu.firebaseio.com"
#define FIREBASE_AUTH "1CMvHocG4TPd2ivn1YTWKDcI7B9f3AMLNGI0vKHz"


// Config connect WiFi
#define WIFI_SSID "ESP_Test"
#define WIFI_PASSWORD "123456789"

Servo myservo; 

const byte interruptPin = D2;
const byte buttonPin = A0;
int Buzzer = D4; 
int IF_1; 
int buttonState;
int Count = 0;
int state = 0;
int ST = 0;
int SD_STATUS = 0;
int Show_LOG = 0;
int num = 0;
String Previus_UID;

void Count_Bot(){
    state = 1;
}

void beep(int Delay){
      digitalWrite(Buzzer , HIGH);
      delay(Delay);
      digitalWrite(Buzzer , LOW);
      delay(Delay);
} 

void Long_beep(int Delay){
      digitalWrite(Buzzer , HIGH);
      delay(Delay);
      digitalWrite(Buzzer , LOW);
      delay(Delay);
} 

void setup() {
  myservo.write(180);
  Wire.begin(D6,D5); //(SDA,SCL)
  lcd.begin();
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), Count_Bot , CHANGE);
  myservo.attach(D3);
 
 // WiFi Config & Setup 
  WiFi.mode(WIFI_STA);
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting"); 
  while (WiFi.status() != WL_CONNECTED){
    ConnectWiFi();
    Serial.print(".");
    delay(500);
  }
  Serial.println();  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   connected   ");
  delay(200);
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  // End WiFi Config & Setup
  
  // Firebase Connect database
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
 // FirebaseObject Data = Firebase.get("LogUser/Lasted");
 // Previus_UID = Data.getString("/Uid");
}

void loop() {
          IF_1 =  digitalRead(D1);
          buttonState = analogRead(buttonPin);              
          FirebaseObject Data = Firebase.get("LogUser/Lasted");
          Previus_UID = Data.getString("/Uid");   
           
               if(ST == 0){
                     Serial.println("Welcome to Smart bin");
                     int Status  = Data.getInt("/StatusDevice");
                    
                    if(Status == 1){
                          ST = 1;
                          SD_STATUS = 1;
                          //Show_LOG = 0;                          
                       }
                    else{
                          lcd.clear();
                          lcd.setCursor(0, 0);
                          lcd.print("   Welcome!!!   "); 
                          lcd.setCursor(0, 1);
                          lcd.print(" Smart Bin FGU ");
                          delay(2000);
                          
                          if(!Show_LOG){
                            for(int i=0; i <3;i++){   
                              lcd.clear();                                
                                lcd.noDisplay();
                                delay(500);             
                                   lcd.setCursor(0, 0);
                                   lcd.print("  Please LogIn! ");
                                   lcd.setCursor(0, 1);
                                   lcd.print("   On Webpage   ");
                                lcd.display();
                                delay(500);
                            } 
                            lcd.clear(); 
                            // Show_LOG = 1;    
                          }
                          
                      }             
               }
               if(ST == 1){
                      if(Firebase.failed()) {
                            Serial.print("FAILED");
                            Serial.println(Firebase.error());  
                            return;
                      }      
                      else{
                           if(SD_STATUS){
                               StandBy();  
                            }
                            //Serial.println("SUCCESS");                                               
                                   if(IF_1 == 0){
                                       delay(1000);
                                       myservo.write(90);
                                       Serial.println("Servo On");
                                       Long_beep(700);
                                       myservo.write(180);
                                       Serial.println("Servo Off");
                                       delay(200);
                                      }    
                                                switch (state) {
                                                   case 1:{                                                         
                                                            Count++; 
                                                            beep(150);
                                                            beep(150);   
                                                            lcd.clear();            
                                                            lcd.setCursor(0, 0);
                                                            lcd.print("Bottle Insert!!");
                                                            lcd.setCursor(0, 1);
                                                            lcd.print(">>>  ");
                                                            lcd.setCursor(5, 1);
                                                            lcd.print(Count);
                                                            lcd.setCursor(10, 1);
                                                            lcd.print("Bottles");     
                                                            state = 0;
                                                            delay(500);
                                                         }break;                                                        
                                                   default:
                                                        // Code
                                                      break;                
                                                }
                                    
                                     if(buttonState > 1000){                                     
                                      SetData(); 
                                      Long_beep(1000);
                                    for(int i=0; i <3;i++){ 
                                        lcd.noDisplay();
                                        delay(500);
                                        lcd.setCursor(0, 0);
                                        lcd.print("    SUCCESS!    ");
                                        lcd.setCursor(0, 1);
                                        lcd.print(" See You Agian! ");
                                        lcd.display();
                                        delay(500);
                                     }
                                      beep(100);
                                      beep(100);
                                      LED2Blink();
                                      LED2Blink();
                                      lcd.clear();
                                    
                                     // LED2Blink();
                                   
                                    ST = 0;
                                    delay(1000);       
                               }
                         
                          }
                   }                         
}
void SetData(){
     FirebaseObject Data = Firebase.get("LogUser/Lasted");
     String UID = Data.getString("/Uid");
    
     FirebaseObject LogData = Firebase.get("users/"+UID);
     String Username = LogData.getString("/username");
     String Profile_pic = LogData.getString("/profile_picture");
     String Email = LogData.getString("/email");
     String Bin_ID = LogData.getString("/SBNumber");

      
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& valueObject = jsonBuffer.createObject();
      valueObject["UID"] = UID;
      valueObject["Username"] = Username;
      valueObject["SBNumber"] = Bin_ID;
      valueObject["Logbottle"] = Count;         
     
        num = num+1;   
        String(num);
        Firebase.push("History",valueObject);
            
    
      if (Firebase.failed()) {
       Serial.print("Set"+ UID +"FAILED");
       Serial.println(Firebase.error());  
       return ;
       } 

      Serial.println("Set ===> "+ UID +"  ==> OK");
      delay(1000);
      Firebase.setInt("LogUser/Lasted/StatusDevice" , 0);      
      Count = 0;     
 }
void StandBy(){

     beep(100);
     beep(100); 
     Long_beep(500);
     LED1Blink();
     LED1Blink();
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("Ready to Insert!");
     for(int i=0; i <3;i++){                                   
          lcd.noDisplay();
          delay(500);             
          lcd.setCursor(0, 1);
          lcd.print(" V V V V V V V ");  
          lcd.display();
          delay(500);
    }  
    //delay(500);
    LED1Blink();
    digitalWrite(D7,LOW);
    SD_STATUS = 0;    
} 

void LED2Blink(){
  digitalWrite(D8,HIGH);
  delay(300);
  digitalWrite(D8,LOW);
  delay(200); 
  digitalWrite(D8,HIGH);
  delay(300);
  digitalWrite(D8,LOW);
  delay(200); 
}

void LED1Blink(){
  digitalWrite(D7,HIGH);
  delay(300);
  digitalWrite(D7,LOW);
  delay(200); 
  digitalWrite(D7,HIGH);
  delay(300);
  digitalWrite(D7,LOW);
  delay(200); 
  digitalWrite(D7,HIGH);
  delay(200); 
}

void ConnectWiFi(){
    lcd.setCursor(0, 0);
    lcd.print("   connecting   ");   
    lcd.setCursor(0, 1);
    lcd.print("===="); 
    delay(500);
    lcd.setCursor(4, 1);
    lcd.print("====");
    delay(500);
    lcd.setCursor(8, 1);
    lcd.print("====");
    delay(500);
    lcd.setCursor(12, 1);
    lcd.print("===>");
    delay(500);  
    lcd.clear();

}

