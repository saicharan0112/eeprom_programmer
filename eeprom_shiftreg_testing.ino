#define SHIFT_DATA 2 // pin 11 in 595 
#define SHIFT_CLK 3 // pin 12 in 595
#define SHIFT_LATCH 4 // pin 14 in 595
#define EEPROM_DO 5 
#define EEPROM_D7 12
#define WRITE_ENABLE 13

void setAddress(int address, bool outputEnable){

  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80) );
  shiftOut(SHIFT_DATA, SHIFT_CLK, MSBFIRST, address);
  
  digitalWrite(SHIFT_LATCH, LOW);
  digitalWrite(SHIFT_LATCH, HIGH);
  digitalWrite(SHIFT_LATCH, LOW);

}

byte readEEPROM(int address){

  for (int pin = 5; pin <= 12; pin += 1){
    pinMode(pin, INPUT);
  }
  setAddress(address, true);
  byte data = 0;
  for (int pin = 12; pin >= 5; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}

void writeEEPROM(int address, byte data) {

  for (int pin = 5; pin <= 12; pin += 1) {
    pinMode(pin, OUTPUT);
  }
  setAddress(address, false);
  for (int pin = 5; pin <= 12; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }
  digitalWrite(13, LOW);
  delayMicroseconds(10);
  digitalWrite(13, HIGH);
  delay(10);
}

void printContents(){
  
  for (int base = 0; base <= 255; base += 16) {
    byte data[16];
    for (int offset = 0; offset <= 15; offset += 1){
      data[offset] = readEEPROM(base + offset);
    }
    char buf[80];
    sprintf(buf, "%03x: %02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x", 
    base, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9],
    data[10], data[11], data[12], data[13], data[14], data[15]);

    Serial.println(buf);

  }

}

byte data[] = { 0x01, 0x4f, 0x12, 0x06, 0x4c, 0x24, 0x20, 0x0f, 0x00, 0x04, 0x08, 0x60, 0x31, 0x42, 0x30, 0x38 };

void setup() {
  // put your setup code here, to run once:

  pinMode(SHIFT_DATA, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT); 
  pinMode(SHIFT_LATCH, OUTPUT); 

  digitalWrite(13, HIGH);
  pinMode(13, OUTPUT);

  Serial.begin(9600);

  // Read individual address
  // Serial.println(readEEPROM(2));

  // Writing entire EEPROM with 0's
  for (int address = 0; address <= 2047; address += 1){
    writeEEPROM(address, 0xff);
  }
  
  // Program 16 bits with BCD codes
  for (int address = 0; address <= 15; address += 1) {

    writeEEPROM(address, data[address]);
  }

  // write data to a given address into EEPROM
  // writeEEPROM(0, 0x0a);

  // print data inside EEPROM
  printContents(); 

}

void loop() {
  // put your main code here, to run repeatedly:

}
