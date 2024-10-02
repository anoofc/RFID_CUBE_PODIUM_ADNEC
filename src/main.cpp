#define DEBUG     0         // 0 = DEBUG OFF, 1 = DEBUG ON
#define RST_PIN   D0        
#define SS_PIN    D8  

#define TAG1 "8321E1F4"     
#define TAG2 "5BA903A"
#define TAG3 "539F21D"
#define TAG4 "733733E"
#define TAG5 "2C79D87D"
#define TAG6 "93A03DE"

#include <Arduino.h> 
#include <SPI.h>
#include <MFRC522.h>

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance

String tagID = "";                  // String to store the tag ID

MFRC522::Uid id;                    // Create a struct to store the ID of the card
uint8_t control = 0x00;             // Variable to store the control value

//*****************************************************************************************//
//                                      FUNCTIONS                                        //
/**
 * Checks the tag ID and Serial Print corresponding Command.
 * 
 * This function takes a tag ID as input and checks it against predefined tag IDs (TAG1, TAG2, TAG3, TAG4, TAG5, TAG6). 
 * If the tag ID matches any of the predefined tags, it prints a corresponding Command to the serial monitor. 
 * If the tag ID does not match any of the predefined tags and the DEBUG flag is set, it prints an "Unknown Tag" message to the serial monitor.
 *
 * @param tag_ID The tag ID to be checked.
 */
void checkTag(String tag_ID){
  if      (tag_ID == TAG1) { Serial.println('A'); }
  else if (tag_ID == TAG2) { Serial.println('B'); }
  else if (tag_ID == TAG3) { Serial.println('C'); }
  else if (tag_ID == TAG4) { Serial.println('D'); }
  else if (tag_ID == TAG5) { Serial.println('E'); }
  else if (tag_ID == TAG6) { Serial.println('F'); }
  else    { if (DEBUG){Serial.println("Unknown Tag");} }
}

/**
 * Stores the tag ID by converting the given 8-bit data into a hexadecimal string with leading zeroes.
 * 
 * This function takes an array of 8-bit data and its length as parameters. 
 * It then converts each byte of the data into a hexadecimal string and concatenates them to form the tag ID. 
 * The resulting tag ID is stored in the global variable 'tagID'.
 * 
 * @param data Pointer to the array of 8-bit data.
 * @param length Length of the data array.
 */
void storeTag(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
  tagID = "";
  for (int i=0; i<length; i++) {
  tagID.concat(String(mfrc522.uid.uidByte[i], HEX)); // Adds the 4 bytes in a single String variable 
  }
}
 

 
/**
 * Copies the UID and related information from the `mfrc522` object to the provided `id` object.
 * 
 * This function initializes the `id` object by setting all its bytes to zero using `memset`. 
 * Then, it copies the UID bytes from the `mfrc522` object to the `id` object using `memcpy`. 
 * It also sets the `size` and `sak` properties of the `id` object to the corresponding values from the `mfrc522` object.
 * 
 * id A pointer to the `MFRC522::Uid` object where the UID and related information will be copied.
 */
void cpid(MFRC522::Uid *id){
  memset(id, 0, sizeof(MFRC522::Uid));
  memcpy(id->uidByte, mfrc522.uid.uidByte, mfrc522.uid.size);
  id->size = mfrc522.uid.size;
  id->sak = mfrc522.uid.sak;
}

/**
 * Reads an RFID card and processes its ID.
 *
 * This function checks for the presence of a new RFID card and reads its serial number.
 * It then processes the card ID by converting it to uppercase, storing it, and checking it against known tags.
 * If a known tag is detected, a specific message is printed. The function also handles the card removal process.
 *
 *  The function includes debug print statements if the DEBUG flag is set.
 */
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
  if ( tagID == TAG1 || tagID == TAG2 || tagID == TAG3 || tagID == TAG4 || tagID == TAG5 || tagID == TAG6 ) { Serial.println('Z'); }
  
  if (DEBUG){ Serial.println("CARD REMOVED"); }
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

/**
 * Initializes the serial communication, SPI bus, and RFID reader.
 * 
 * This function sets up the necessary components for the RFID reader to function.
 * It initializes the serial communication at a baud rate of 115200 for debugging purposes,
 * starts the SPI bus, and initializes the RFID reader. After a short delay, it prints
 * the current antenna gain setting of the RFID reader to the serial monitor.
 */
void setup(){
  Serial.begin(115200);            // Initialize serial communications with the PC
  SPI.begin();                     // Init SPI bus

  // mfrc522.PCD_SetAntennaGain(0x07);
  mfrc522.PCD_Init(); 
  
  delay(50);
  
  // Serial.print("Antenna Gain: ");
  // Serial.println(mfrc522.PCD_GetAntennaGain(), HEX);
}

void loop() {
  readCard();
}