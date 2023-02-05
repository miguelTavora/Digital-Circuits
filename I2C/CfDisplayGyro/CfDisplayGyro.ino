#include <Wire.h>

//L3G420
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24
#define dps 250 //250 or 500 or 2000

//display
#define RS 0x01 //comandos ou dados
#define EN 0x04
#define LUZ 0x08
#define ENDERECO 0x27

int L3G4200D_Address = 0x69; //I2C address of the L3G4200D
int x, y, z;
byte t;

unsigned char msb, lsb, xlsb;

void setup(){

  Wire.begin();
  Serial.begin(9600);
  
  Serial.println("Iniciando Display");
  DisplaySetup();
  
  Serial.println("Iniciando L3G420");
  setupL3G420(dps); // Configure L3G420  - 250, 500 or 2000 deg/sec
  delay(1500); //wait for the sensor to be ready 
}

void loop(){
  getGyroValues();  // This will update x, y, and z with new values
  gyroCalc();
  Display();
  //randomGeneratorTest();
  printsForPython();

  delay(100); //Just here to slow down the serial to make it more readable
}
void printsForPython(){
  Serial.println("x");
  Serial.println(x); 
                   
  Serial.println("y");
  Serial.println(y);

  delay(1000);
}
void randomGeneratorTest(){//utilizado para testes entre serial e python
  
  int x = random(-180,180);
  Serial.println("x");
  Serial.println(x);
  
  int y = random(-180,180);
  Serial.println("y");
  Serial.println(y);
}
void DisplaySetup() {
  Wire.begin();//inicializacao da interface
  delay(60);
  escreverComandos4(0x03);
  delay(10);
  escreverComandos4(0x03);
  delayMicroseconds(205);
  escreverComandos4(0x03);
  escreverComandos4(0x02);
  //display modo 2 linhas e 16 caracteres
  escreverComandos4(0x02);
  escreverComandos4(0x08);
  delayMicroseconds(130);
  escreverComandos4(0x00);
  escreverComandos4(0x0F);//display on, cursor on, blink on
  delayMicroseconds(130);
  escreverComandos8(0x01);//clear
  delay(10);
}

void Display(){
  escreverComandos8(0x01);//clear
  delay(10);
  DisplaySetCursor(0,0);
  DisplayString("X:");
  DisplayInt(x);
  DisplayString(" Y:");
  DisplayInt(y);
  DisplayString(" Z:");
  DisplayInt(z); 
  DisplaySetCursor(1,0);
  DisplayString("Temperatura:");
  DisplayInt(t);
  DisplayString("C");
  
}

void escreverDados8(byte oitoBits) {
  escreverDados4(oitoBits >> 4);
  escreverDados4(oitoBits);
  }

void escreverComandos8(byte oitoBits) {
  escreverComandos4(oitoBits >> 4);
  escreverComandos4(oitoBits);
  }
  
void DisplaySetCursor(byte linha, byte coluna) {
  escreverComandos8(0x80 | (0x40 * linha) | coluna);
  delayMicroseconds(130);
}

void escreverDados4 (byte quatrobits) {
  Wire.beginTransmission(ENDERECO);
  Wire.write((quatrobits << 4) | LUZ | RS);
  Wire.endTransmission();
  Wire.beginTransmission(ENDERECO);
  Wire.write((quatrobits << 4) | LUZ | RS | EN );
  Wire.endTransmission();
  delayMicroseconds(1); // enable ativo >450ns
  Wire.beginTransmission(ENDERECO);
  Wire.write((quatrobits << 4) | LUZ | RS);
  Wire.endTransmission();
  delayMicroseconds(40); // tempo > 37us para comando fazer efeito
}

void escreverComandos4 (byte quatrobits) {
  
  Wire.beginTransmission(ENDERECO);
  Wire.write((quatrobits << 4) | LUZ);
  Wire.endTransmission();
  Wire.beginTransmission(ENDERECO);
  Wire.write((quatrobits << 4) | LUZ | EN );
  Wire.endTransmission();
  delayMicroseconds(1); // enable ativo >450ns
  Wire.beginTransmission(ENDERECO);
  Wire.write((quatrobits << 4) | LUZ);
  Wire.endTransmission();
  delayMicroseconds(40); // tempo > 37us para comando fazer efeito
}

void DisplayInt(long s) {
  String num = String(s);
  int str_len = num.length() + 1; 
  char char_array[str_len];
  num.toCharArray(char_array, str_len);
  DisplayString(char_array);
  
}

void DisplayString(char s[]) {
  int i = 0;
  while (s[i] != '\0') {
    escreverDados8((byte)s[i]);
    delayMicroseconds(130);
    i++;
  }
}

byte escreverRegisto8bits (byte endR, byte b){

  Wire.beginTransmission(0x77);
  Wire.write (0xF4);
  Wire.write (b);
  Wire.endTransmission();
  delay (5);
  
  }


void getGyroValues(){

  byte xMSB = readRegister(L3G4200D_Address, 0x29);
  byte xLSB = readRegister(L3G4200D_Address, 0x28);
  x = ((xMSB << 8) | xLSB);

  byte yMSB = readRegister(L3G4200D_Address, 0x2B);
  byte yLSB = readRegister(L3G4200D_Address, 0x2A);
  y = ((yMSB << 8) | yLSB);

  byte zMSB = readRegister(L3G4200D_Address, 0x2D);
  byte zLSB = readRegister(L3G4200D_Address, 0x2C);
  z = ((zMSB << 8) | zLSB);

  byte temp = readRegister(L3G4200D_Address, 0x26);
  t = (temp<<1)|0;
}

int setupL3G420(int scale){
  //From  Jim Lindblom of Sparkfun's code

  // Enable x, y, z and turn off power down:
  writeRegister(L3G4200D_Address, CTRL_REG1, 0b00001111);

  // If you'd like to adjust/use the HPF, you can edit the line below to configure CTRL_REG2:
  writeRegister(L3G4200D_Address, CTRL_REG2, 0b00000000);

  // Configure CTRL_REG3 to generate data ready interrupt on INT2
  // No interrupts used on INT1, if you'd like to configure INT1
  // or INT2 otherwise, consult the datasheet:
  writeRegister(L3G4200D_Address, CTRL_REG3, 0b00001000);

  // CTRL_REG4 controls the full-scale range, among other things:

  if(scale == 250){
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00000000);
  }else if(scale == 500){
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00010000);
  }else{
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00110000);
  }

  // CTRL_REG5 controls high-pass filtering of outputs, use it
  // if you'd like:
  writeRegister(L3G4200D_Address, CTRL_REG5, 0b00000000);
}

void writeRegister(int deviceAddress, byte address, byte val) {
    Wire.beginTransmission(deviceAddress); // start transmission to device 
    Wire.write(address);       // send register address
    Wire.write(val);         // send value to write
    Wire.endTransmission();     // end transmission
}

int readRegister(int deviceAddress, byte address){

    int v;
    Wire.beginTransmission(deviceAddress);
    Wire.write(address); // register to read
    Wire.endTransmission();

    Wire.requestFrom(deviceAddress, 1); // read a byte

    while(!Wire.available()) {
        // waiting
    }

    v = Wire.read();
    return v;
}
void gyroCalc(){
  if(dps==250){//escala 250dps
    x=x*8.75*pow(10,-3);
    y=y*8.75*pow(10,-3);
    z=z*8.75*pow(10,-3);
    
  }else if(dps==500){//escala 500dps
    x=x*17.5*pow(10,-3);
    y=y*17.5*pow(10,-3);
    z=z*17.5*pow(10,-3);
    
  }else{//escala 2000dps
    x=x*70*pow(10,-3);
    y=y*70*pow(10,-3);
    z=z*70*pow(10,-3);
    
  }
}
