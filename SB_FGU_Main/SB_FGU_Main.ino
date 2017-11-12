#include <Servo.h>
#include <Firebase.h>
#include <FirebaseArduino.h>
#include <FirebaseCloudMessaging.h>
#include <FirebaseError.h>
#include <FirebaseHttpClient.h>
#include <FirebaseObject.h>
#include <ESP8266WiFi.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Config Firebase 
#define FIREBASE_HOST "sb-fgu.firebaseio.com"
#define FIREBASE_AUTH "1CMvHocG4TPd2ivn1YTWKDcI7B9f3AMLNGI0vKHz"

// Config connect WiFi
#define WIFI_SSID "ESP_Test"
#define WIFI_PASSWORD "123456789"

//Countdown time
#define Set_minuite 3
#define Set_second 60

Servo myservo;


/*Setup Interupt Pin*/
const byte IF_2 = D2;
const byte Bt_Pin = D7;

/*State Program*/
int STATE = 0;
int ST_Gencode = 0;
int LogIn_Status = 0;
int StandBy_State  = 0;

/*Count Bottle*/
int CheckState_Bottle = 0;
int Count = 0;    

/*Defined Input-Output Devices*/
//Input
int IF_1; 
int buttonState;
//Output
const byte ButtonPin = D7 ;
const byte Buzzer = D4;

/*Setup TimerCounter*/
unsigned long previousTime = 0;
unsigned long showTime;
int minuite = 0;

/*Parameter Random Code*/
long randNumber;
long prevRand = 0;


void Count_Bot(){
    CheckState_Bottle  = 1;
}

void Gen_Code(){
    ST_Gencode = 1;
}


void setup() {
    Serial.begin(115200);
   /*Setup Server Pin  & Position*/
    myservo.attach(D3);
    myservo.write(180);
   

   /*Setup I2C Pin  & Cursor Position*/
   Wire.begin(D6,D5); //(SDA,SCL)
   lcd.begin();
   /*Setup infared input*/
    pinMode(D1, INPUT);
    pinMode(D2, INPUT);
   /*Setup Servo Output*/
    pinMode(D3, OUTPUT);
    /*Setup LED Output*/
    pinMode(D8, OUTPUT);
    /*Setup Buzzer Output*/
    pinMode(Buzzer, OUTPUT);
    /*Setup Random Number*/
    randomSeed(analogRead(0)); // nothing connected to 0 so read sees noise

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
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("   connected   ");
    delay(200);
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    Serial.println();  
    Serial.print("connected: ");
    Serial.println(WiFi.localIP());
    
    /*Setup Interupt Functions*/
    attachInterrupt(digitalPinToInterrupt(IF_2),Count_Bot,CHANGE);
    attachInterrupt(digitalPinToInterrupt(Bt_Pin), Gen_Code , CHANGE);

    // Firebase Connect database
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    FirebaseObject Data = Firebase.get("LogUser/Lasted");
}

void loop() {
     IF_1 =  digitalRead(D1);
     
     buttonState = digitalRead(ButtonPin);  
    int TimeCount =  Time_Counter(Set_second);
    Serial.print(showTime);
    Serial.print("  ");
    Serial.println(TimeCount);
    if(STATE == 0){
             FirebaseObject Data = Firebase.get("LogUser/Lasted");
             String  Uid = Data.getString("/Uid");
             
             int Status  = Data.getInt("/StatusDevice");
                 if(Status == 0){
                       lcd.clear();
                            lcd.setCursor(0, 0);
                            lcd.print("   Welcome!!!   "); 
                            lcd.setCursor(0, 1);
                            lcd.print(" Smart Bin FGU ");
                            delay(2000);                          
                                for(int i=0; i < 3;i++){   
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
                  }
                  else if(Status == 1){
                           
                             if(!ST_Gencode){ 
                                    lcd.noDisplay();
                                    delay(500); 
                                         lcd.setCursor(0, 0); 
                                         lcd.print("  Press Button  ");
                                         lcd.setCursor(0, 1); 
                                         lcd.print("   To  Gencode  ");
                                         lcd.display();
                                     delay(500);
                                     lcd.clear();
                             }
                             else{
                                    do{
                                          //lcd.clear();
                                           randNumber = random(100000,999999); // generate random number between 1 & 5 (minimum is inclusive, maximum is exclusive)
                                           Serial.println(randNumber); // show the value in Serial Monitor 
                                           Firebase.setString("/LogUser/CodeGen/Uid", Uid);
                                           Firebase.setInt("/LogUser/CodeGen/Code", randNumber);
                                               if (Firebase.failed()) {
                                                 Serial.print("Set Code failed");
                                                 Serial.println(Firebase.error());
                                                     prevRand = randNumber;
                                                  return;
                                                }
                                               else{
                                                   Firebase.setInt("/LogUser/CodeGen/AuthenCode/Status" , 1);
                                                   //Serial.print("Set Code Success!");
                                                   lcd.clear();
                                                   lcd.setCursor(0, 0); 
                                                   lcd.print("Gencode  Success");
                                                   lcd.setCursor(0, 1); 
                                                   lcd.print("Code: ");
                                                   lcd.setCursor(6, 1); 
                                                   lcd.print(randNumber);
                                                   delay(3000);
                                                } 
                                            delay(1000);
                                            
                                      }while(randNumber == prevRand); 
                                          
                                   prevRand = randNumber;   
                                   ST_Gencode = 0; 
                                   STATE = 1;
                                   delay(3000);
                                  
                              }
              
                }
       
      }//End  STATE = 0
     if(STATE == 1){
         int Auth_Success =  Firebase.getInt("/LogUser/CodeGen/AuthenCode/Status");
              if(Auth_Success == 1){
                       if(StandBy_State == 0){
                            Ready_ToPush();
                       }
                       else{
                              //Serial.println(IF_1);
                              //Serial.println(TimeCount);
                               if(IF_1 == 0){
                                            beep(500);     
                                            myservo.write(90);
                                            Serial.println("Servo On");
                                            delay(400);                                           
                                            myservo.write(180);
                                            Serial.println("Servo Off");
                                            delay(200);
                                 }           
                                               switch (CheckState_Bottle){
                                                            case 1:{                                                         
                                                                         Count++; 
                                                                         beep(200);
                                                                         beep(200);   
                                                                         lcd.clear();            
                                                                         lcd.setCursor(0, 0);
                                                                         lcd.print("Bottle Insert!!");
                                                                         lcd.setCursor(0, 1);
                                                                         lcd.print(">>>  ");
                                                                         lcd.setCursor(5, 1);
                                                                         lcd.print(Count);
                                                                         lcd.setCursor(10, 1);
                                                                         lcd.print("Bottles");     
                                                                         CheckState_Bottle = 0;
                                                                          delay(500);
                                                                    }break;                                                        
                                                               default:
                                                                    // Code
                                                                    break;                
                                                }                         
                                 if(buttonState){                                     
                                               Insert_Data(); 
                                               beep(1000);
                                                    for(int i = 0; i <3 ;i++){ 
                                                            lcd.noDisplay();
                                                            delay(500);
                                                                lcd.setCursor(0, 0);
                                                                lcd.print("    SUCCESS!    ");
                                                                lcd.setCursor(0, 1);
                                                                lcd.print(" See You Agian! ");
                                                            lcd.display();
                                                            delay(500);
                                                    }
                                                //LED_Blink(3);
                                                 beep(200);
                                                 beep(200);
                                                 beep(200);
                                                lcd.clear(); 
                                              STATE = 0;
                                              Firebase.setInt("/LogUser/CodeGen/AuthenCode/Status" , 0);
                                              delay(2000);       
                                }
                                
                                /*if(TimeCount == Set_minuite){
                                      Firebase.setInt("LogUser/Lasted/StatusDevice" , 0);   // LOGOUT ON WEB 
                                    // Clear Parameter 
                                      previousTime = 0;
                                      showTime = 0;
                                      TimeCount = 0;   
                                    //Clear STATE  
                                      STATE = 0;
                                }*/
                                
                          
                        }//End else
                    
               }//End if Auth_Success            

      }//End STATE = 1
}

void Insert_Data(void){
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
     
      Firebase.push("History",valueObject);
            
    
      if (Firebase.failed()) {
       Serial.print("Set"+ UID +"FAILED");
       Serial.println(Firebase.error());  
       return ;
       }
      //Serial.println("Set ===> "+ UID +"  ==> OK");
      delay(1000);
      Firebase.setInt("LogUser/Lasted/StatusDevice" , 0);      
      Count = 0;     
}

void Ready_ToPush(){
     //LED_Blink(3);
     lcd.clear();
     lcd.setCursor(0, 0);
     lcd.print("Ready to Insert!");
     for(int i=0; i < 5 ;i++){                                   
          lcd.noDisplay();
          delay(300);             
              lcd.setCursor(0, 1);
              lcd.print("V              V");    
          lcd.display();
          delay(300);           
          lcd.noDisplay();
          delay(300);             
              lcd.setCursor(0, 1);
              lcd.print("  V         V   ");    
          lcd.display();
          delay(300);     
          lcd.noDisplay();
          delay(300);             
              lcd.setCursor(0, 1);
              lcd.print("     V    V     ");    
          lcd.display();
          delay(300);    
          lcd.noDisplay();
          delay(300);             
              lcd.setCursor(0, 1);
              lcd.print("        V       ");    
          lcd.display();
          delay(300); 
       
    } 
      lcd.setCursor(0, 1);
      lcd.print(" Put The Bottle "); 
    beep(500);
    
    StandBy_State = 1;    
} 

/*void LED_Blink(int Round){
  for(int i=0; i < Round ;i++){
      beep(300);
      digitalWrite(D8,HIGH);
      delay(300);
      digitalWrite(D8,LOW);
      delay(200); 
     
  }
}
*/
void beep(int Delay){
      digitalWrite(Buzzer , HIGH);
      delay(Delay);
      digitalWrite(Buzzer , LOW);
      delay(Delay);
} 

int Time_Counter(int Set_Time_Sec){
  showTime = millis();
  unsigned int Second = (showTime - previousTime) / 1000;
      /*  Serial.print("setTime: ");
          Serial.print(Set_Time_Sec); //แสดงค่าเวลาที่ตั้งไว้
          Serial.print("   showTime: ");
          Serial.print(showTime); //แสดงเวลา millis() ของ Arduino ซึ่งจะเป็นค่าที่วิ่งไปเรื่อยๆ
          Serial.print("   previousTime: ");
          Serial.print(previousTime); //แสดงค่าสุดท้ายของ previousTime หลังจากที่กดปุ่ม A ซึ่งจะเป็นค่าคงที่ ไม่วิ่งตาม millis()
          Serial.print("   Time: ");       
          Serial.print(Second); //แสดงผลการคำนวณว่าจับเวลาไปกี่วิแล้ว
          Serial.println(" Sec");*/
          //Serial.println(showTime);
          if(Second  >=  Set_Time_Sec){ //ถ้าเวลาที่จับได้ มากกว่าหรือเท่ากับเวลาที่ตั้งไว้...         
               minuite++;
               previousTime = showTime;
               return minuite;
           }  
                  
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
