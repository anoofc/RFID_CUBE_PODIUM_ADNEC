#define DEBUG     0         // 0 = off, 1 = on
#define RST_PIN   D0        
#define SS_PIN    D8  

#define TAG1 "13BD161D"     
#define TAG2 "231CAFE"
#define TAG3 "A348261B"
#define TAG4 "39BF8F7"
#define TAG5 "931B21D"
#define TAG6 "338221B"
 
#include <SPI.h>
#include <MFRC522.h>

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

String tagID = "";

MFRC522::Uid id;
uint8_t control = 0x00;

//*****************************************************************************************//
//                                      FUNCTIONS                                        //
void checkTag(String tag_ID){
  if      (tag_ID == TAG1) { Serial.println("*A#"); }
  else if (tag_ID == TAG2) { Serial.println("*B#"); }
  else if (tag_ID == TAG3) { Serial.println("*C#"); }
  else if (tag_ID == TAG4) { Serial.println("*D#"); }
  else if (tag_ID == TAG5) { Serial.println("*E#"); }
  else if (tag_ID == TAG6) { Serial.println("*F#"); }
  else    { if (DEBUG){Serial.println("Unknown Tag");} }
}

void storeTag(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
  tagID = "";
  for (int i=0; i<length; i++) {
  tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable 
  }
}
 

 
void cpid(MFRC522::Uid *id){
  memset(id, 0, sizeof(MFRC522::Uid));
  memcpy(id->uidByte, mfrc522.uid.uidByte, mfrc522.uid.size);
  id->size = mfrc522.uid.size;
  id->sak = mfrc522.uid.sak;
}

void readCard(){
  if (!mfrc522.PICC_IsNewCardPresent()) { return; }
  if (!mfrc522.PICC_ReadCardSerial()) { return; }

  cpid(&id);
  storeTag(id.uidByte, id.size);
  tagID.toUpperCase();
  if (DEBUG){ Serial.println("NewCard: "+ tagID); }
  checkTag(tagID);

  while (true) {
    control = 0;
    for (int i = 0; i < 3; i++) {
    if (!mfrc522.PICC_IsNewCardPresent()) {
      if (mfrc522.PICC_ReadCardSerial()){ control |= 0x16; }
      if (mfrc522.PICC_ReadCardSerial()){ control |= 0x16; }
      control += 0x1;
    }
    control += 0x4;
  }
    if (control == 13 || control == 14){}
    else { break; }
  }
  if ( tagID == TAG1 || tagID == TAG2 || tagID == TAG3 || tagID == TAG4 || tagID == TAG5 || tagID == TAG6 ) { Serial.println("*Z#"); }
  
  if (DEBUG){ Serial.println("CARD REMOVED"); }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void setup(){
  Serial.begin(115200);            // Initialize serial communications with the PC
  SPI.begin();                     // Init SPI bus

  // mfrc522.PCD_SetAntennaGain(0x07);
  mfrc522.PCD_Init(); 
  
  delay(50);
  
  Serial.print("Antenna Gain: ");
  Serial.println(mfrc522.PCD_GetAntennaGain(), HEX);
}

void loop() {
  readCard();
}