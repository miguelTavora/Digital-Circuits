#define A0 47
#define A1 49
#define A2 51
#define A3 53
#define B0 23
#define B1 25
#define B2 27
#define B3 29
#define C0 39
#define C1 41

#define LED0 11
#define LED1 10
#define LED2 9
#define LED3 8
#define LED4 12
#define LED5 13
#define LED6 7
#define button 6

bool A[] = {0,0,0,0};
bool B[] = {0,0,0,0};
bool C[] = {0,0};
bool Cy1bit,Bw1bit, Cy1bitH, Bw1bitASR;

bool S0[] = {0,0,0,0};
bool S1[] = {0,0,0,0};
bool S2[] = {0,0,0,0};
bool S3[] = {0,0,0,0};

bool Z0,Z1,Z2,Z3;

bool Ov0,Ov1;

////ALU OUT/////
bool SOUT[] = {0,0,0,0};
bool CyBwOUT;
bool OvOUT;
bool Zout;


////ASR////
bool mux1, mux2, mux3, mux4, enableI, enableS;
bool clock1 = 0;
bool subIn[] = {0,0,0,0};
bool subOut[] = {0,0,0,0};
bool shiftRightOUT[] = {0,0,0,0};
bool Q = 1;

bool mux2x1Out[] = {0,0,0,0};
bool mux2x1Out2[] = {0,0,0,0};
bool mux2x1Out3[] = {0,0,0,0};
bool mux2x1Out4[] = {0,0,0,0};
bool registoIOut[] = {0,0,0,0};
bool registoSOut[] = {0,0,0,0};
bool ZeroASR = 0;

bool Q0 = 0, Q1 = 0, D0 = 0, D1 = 0;

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(B0, INPUT_PULLUP);
  pinMode(B1, INPUT_PULLUP);
  pinMode(B2, INPUT_PULLUP);
  pinMode(B3, INPUT_PULLUP);
  pinMode(C1, INPUT_PULLUP);
  pinMode(C0, INPUT_PULLUP);
  pinMode(button, INPUT_PULLUP);
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);
  pinMode(LED6, OUTPUT);
  getAandBandC();
  alu(A,B,C[1],C[0]);
  aluOUTprint();
  //printNumbersBinary();
  //printNumbers();

}

void loop() {
  
  getAandBandC();
  moduloControlo();
  alu(A,B,C[1],C[0]);
  ledF();
  if(clock1){
    printASR();
  }
  

  
}

void getAandBandC(){
  A[3] = !digitalRead(A3);
  A[2] = !digitalRead(A2);
  A[1] = !digitalRead(A1);  
  A[0] = !digitalRead(A0);

  
  B[3] = !digitalRead(B3);
  B[2] = !digitalRead(B2);
  B[1] = !digitalRead(B1);  
  B[0] = !digitalRead(B0);

  C[1] = !digitalRead(C1);  
  C[0] = !digitalRead(C0);
  clock1 = !digitalRead(button);


  
}

void ledF(){
  digitalWrite(LED0,SOUT[0]);
  digitalWrite(LED1,SOUT[1]);
  digitalWrite(LED2,SOUT[2]);
  digitalWrite(LED3,SOUT[3]);
  digitalWrite(LED4,OvOUT);
  digitalWrite(LED5,CyBwOUT);
  digitalWrite(LED6,Zout);
  
  
}
void alu(bool A[], bool B[], bool CC1, bool CC0){
  
  somador4bits(A[3], A[2], A[1], A[0], B[3], B[2], B[1], B[0]);
  subtrator4bits(A[3], A[2], A[1], A[0], B[3], B[2], B[1], B[0]);
  shiftRight2(A[3], A[2], A[1], A[0], B[3], B[2], B[1], B[0]);
  shiftRight4(A[3], A[2], A[1], A[0], B[3], B[2], B[1], B[0]);
  NOR4bit(A[3], A[2], A[1], A[0], B[3], B[2], B[1], B[0]);
  
  SOUT[3]= mux4x2(S0[3],S1[3],registoSOut[3],S3[3], CC1,CC0);
  SOUT[2]= mux4x2(S0[2],S1[2],registoSOut[2],S3[2], CC1,CC0);
  SOUT[1]= mux4x2(S0[1],S1[1],registoSOut[1],S3[1], CC1,CC0);
  SOUT[0]= mux4x2(S0[0],S1[0],registoSOut[0],S3[0], CC1,CC0);
  OvOUT = mux2x1(Ov0,Ov1,CC0);
  Zout = mux4x2(Z0,Z1,Z2,Z3,CC1,CC0);
  CyBwOUT = mux2x1(Cy1bitH,Bw1bit,CC0);

}
void moduloControlo(){
  if(clock1){
    FES();
    FS();
  
  }
  
}

void FES(){
  D0 = !Q1&&!Q0;
  D1 = Q1^Q0;
  flipFlopD0(D0);
  flipFlopD1(D1);
}
void FS(){
  mux1 =Q0;
  mux2 =(registoIOut[3] || registoIOut[2] || registoIOut[1]|| registoIOut[0]);
  mux3 =(registoIOut[3] || registoIOut[2] || registoIOut[1]|| registoIOut[0]);
  mux4 = Q0;
  enableI = 1;
  enableS = 1;
}
void flipFlopD0(bool D0){
  Q0 = (clock1)?D0:Q0;
}

void flipFlopD1(bool D1){
  Q1 = (clock1)?D1:Q1;
}

void shiftRight2(bool aa3, bool aa2, bool aa1, bool aa0, bool bb3, bool bb2, bool bb1, bool bb0){
  mux2x1Out2[3] = mux2x1(0,subOut[3],mux2);
  mux2x1Out2[2] = mux2x1(0,subOut[2],mux2);
  mux2x1Out2[1] = mux2x1(0,subOut[1],mux2);
  mux2x1Out2[0] = mux2x1(0,subOut[0],mux2);
  
  mux2x1Out[3] = mux2x1(mux2x1Out2[3], bb3,mux1);
  mux2x1Out[2] = mux2x1(mux2x1Out2[2], bb2,mux1);
  mux2x1Out[1] = mux2x1(mux2x1Out2[1], bb1,mux1);
  mux2x1Out[0] = mux2x1(mux2x1Out2[0], bb0,mux1);
  RegistoI(mux2x1Out[3], mux2x1Out[2], mux2x1Out[1], mux2x1Out[0], enableI&&clock1);

  subASR(registoIOut[3], registoIOut[2], registoIOut[1], registoIOut[0], 0, 0, 0, 1);
  
  mux2x1Out2[3] = mux2x1(0,subOut[3],mux2);
  mux2x1Out2[2] = mux2x1(0,subOut[2],mux2);
  mux2x1Out2[1] = mux2x1(0,subOut[1],mux2);
  mux2x1Out2[0] = mux2x1(0,subOut[0],mux2);
  
  mux2x1Out[3] = mux2x1(mux2x1Out2[3], bb3,mux1);
  mux2x1Out[2] = mux2x1(mux2x1Out2[2], bb2,mux1);
  mux2x1Out[1] = mux2x1(mux2x1Out2[1], bb1,mux1);
  mux2x1Out[0] = mux2x1(mux2x1Out2[0], bb0,mux1);
  
  shiftRight3();
  


  
  
  
}
void shiftRight3(){
  mux2x1Out3[3] = mux2x1(registoSOut[3],shiftRightOUT[3],mux3);
  mux2x1Out3[2] = mux2x1(registoSOut[2],shiftRightOUT[2],mux3);
  mux2x1Out3[1] = mux2x1(registoSOut[1],shiftRightOUT[1],mux3);
  mux2x1Out3[0] = mux2x1(registoSOut[0],shiftRightOUT[0],mux3);
  

  mux2x1Out4[3] = mux2x1(mux2x1Out3[3], A[3], mux4);
  mux2x1Out4[2] = mux2x1(mux2x1Out3[2], A[2], mux4);
  mux2x1Out4[1] = mux2x1(mux2x1Out3[1], A[1], mux4);
  mux2x1Out4[0] = mux2x1(mux2x1Out3[0], A[0], mux4);
  
  RegistoS(mux2x1Out4[3], mux2x1Out4[2], mux2x1Out4[1], mux2x1Out4[0], enableS&&clock1);
  shiftRight(registoSOut[3],registoSOut[2],registoSOut[1],registoSOut[0], 0, 0, 0, 1);
  
  mux2x1Out3[3] = mux2x1(registoSOut[3],shiftRightOUT[3],mux3);
  mux2x1Out3[2] = mux2x1(registoSOut[2],shiftRightOUT[2],mux3);
  mux2x1Out3[1] = mux2x1(registoSOut[1],shiftRightOUT[1],mux3);
  mux2x1Out3[0] = mux2x1(registoSOut[0],shiftRightOUT[0],mux3);

  mux2x1Out4[3] = mux2x1(mux2x1Out3[3], A[3], mux4);
  mux2x1Out4[2] = mux2x1(mux2x1Out3[2], A[2], mux4);
  mux2x1Out4[1] = mux2x1(mux2x1Out3[1], A[1], mux4);
  mux2x1Out4[0] = mux2x1(mux2x1Out3[0], A[0], mux4);

  Z2 = mux2x1(1,0,(registoSOut[0]||registoSOut[1]||registoSOut[2]||registoSOut[3]));
  

}

void RegistoS(bool m3, bool m2, bool m1, bool m0, bool clock2){
  registoSOut[3] = mux2x1( registoSOut[3], m3, clock2);
  registoSOut[2] = mux2x1( registoSOut[2], m2, clock2);
  registoSOut[1] = mux2x1( registoSOut[1], m1, clock2);
  registoSOut[0] = mux2x1( registoSOut[0], m0, clock2);
}

void subASR(bool aa3, bool aa2, bool aa1, bool aa0, bool bb3, bool bb2, bool bb1, bool bb0){
  Bw1bitASR = 0;
  subOut[0] = subtrator1bit(aa0,bb0,0);
  subOut[1] = subtrator1bit(aa1,bb1,Bw1bitASR);
  subOut[2] = subtrator1bit(aa2,bb2,Bw1bitASR);
  subOut[3] = subtrator1bit(aa3,bb3,Bw1bitASR);
}
void RegistoI(bool m3, bool m2, bool m1, bool m0, bool clock2){
  registoIOut[3] = mux2x1( registoIOut[3], m3, clock2);
  registoIOut[2] = mux2x1( registoIOut[2], m2, clock2);
  registoIOut[1] = mux2x1( registoIOut[1], m1, clock2);
  registoIOut[0] = mux2x1( registoIOut[0], m0, clock2);
}
void NOR4bit(bool aa3, bool aa2, bool aa1, bool aa0, bool bb3, bool bb2, bool bb1, bool bb0){
  S3[0] = NOR1bit(aa0, bb0);
  S3[1] = NOR1bit(aa1, bb1);
  S3[2] = NOR1bit(aa2, bb2);
  S3[3] = NOR1bit(aa3, bb3);
  Z3 = mux2x1(1,0,(S3[0]||S3[1]||S3[2]||S3[3]));
}
bool NOR1bit(bool A, bool B){
  return (!A&&!B);
}
void subtrator4bits(bool aa3, bool aa2, bool aa1, bool aa0, bool bb3, bool bb2, bool bb1, bool bb0){
  S1[0] = somador1bit(aa0,!bb0,1);
  S1[1] = somador1bit(aa1,!bb1,Cy1bit);
  S1[2] = somador1bit(aa2,!bb2,Cy1bit);
  S1[3] = somador1bit(aa3,!bb3,Cy1bit);
  Z1 = mux2x1(1,0,(S1[0]||S1[1]||S1[2]||S1[3]));
  Bw1bit = !Cy1bit;
  Ov1 = Ov1bit(aa3,!bb3,S1[3],!Bw1bit);
  
  
  
}

bool subtrator1bit(bool A, bool B, bool BwIn){
  bool S = !A&&!B&&Bw1bitASR || !A&&B&&!Bw1bitASR ||A&&!B&&!Bw1bitASR || A&&B&&Bw1bitASR;
  Bw1bitASR = B&&BwIn || !A&&B || !A&&BwIn;
  return S;
}

bool Ov1bit(bool A, bool B, bool S, bool Cy){
  return A^B^S^Cy;
}
void somador4bits(bool aa3, bool aa2, bool aa1, bool aa0, bool bb3, bool bb2, bool bb1, bool bb0){
  Cy1bit = 0;
  S0[0] = somador1bit(aa0,bb0,0);
  S0[1] = somador1bit(aa1,bb1,Cy1bit);
  S0[2] = somador1bit(aa2,bb2,Cy1bit);
  S0[3] = somador1bit(aa3,bb3,Cy1bit);
  Z0 = mux2x1(1,0,(S0[0]||S0[1]||S0[2]||S0[3]));
  Ov0 = Ov1bit(aa3,bb3,S0[3],Cy1bit);
  Cy1bitH = Cy1bit;
  
  
}
bool somador1bit(bool A,bool B,bool CarryIn){
  bool S = CarryIn^A^B;
  //bool S = CarryIn&((!A&&!B)||(A&&B))||!CarryIn&&((!A&&B)||(A&&!B));
  Cy1bit = A&&B || CarryIn&&(A^B);
  return S;
  
}


bool mux2x1(bool A, bool B, bool C){
  return (!C)? A:B;
}

bool mux4x2(bool AA, bool BB, bool CC, bool DD, bool E1, bool E0){
  return (!E1&&!E0&&AA || !E1&&E0&&BB || E1&&!E0&&CC || E1&&E0&&DD);
}

void shiftRight(bool aa3, bool aa2, bool aa1, bool aa0, bool bb3, bool bb2, bool bb1, bool bb0){//A numero relativo
  bool sinal = aa3;
  bool SS0 = mux4x2(aa0,aa1,aa2,aa3, bb1,bb0);
  bool SS1 = mux4x2(aa1,aa2,aa3,sinal, bb1,bb0);
  bool SS2 = mux4x2(aa2,aa3,sinal,sinal, bb1,bb0);
  bool SS3 = mux4x2(aa3,sinal,sinal,sinal, bb1,bb0);
  shiftRightOUT[0] = mux2x1(SS0,sinal,bb2||bb3);
  shiftRightOUT[1] = mux2x1(SS1,sinal,bb2||bb3);
  shiftRightOUT[2] = mux2x1(SS2,sinal,bb2||bb3);
  shiftRightOUT[3] = mux2x1(SS3,sinal,bb2||bb3);
  ZeroASR = mux2x1(1,0,(registoSOut[0]||registoSOut[1]||registoSOut[2]||registoSOut[3]));
}
void shiftRight4(bool aa3, bool aa2, bool aa1, bool aa0, bool bb3, bool bb2, bool bb1, bool bb0){//A numero relativo, funcao auxiliar para saber a resposta final
  bool sinal = aa3;
  bool SS0 = mux4x2(aa0,aa1,aa2,aa3, bb1,bb0);
  bool SS1 = mux4x2(aa1,aa2,aa3,sinal, bb1,bb0);
  bool SS2 = mux4x2(aa2,aa3,sinal,sinal, bb1,bb0);
  bool SS3 = mux4x2(aa3,sinal,sinal,sinal, bb1,bb0);
  S2[0] = mux2x1(SS0,sinal,bb2||bb3);
  S2[1] = mux2x1(SS1,sinal,bb2||bb3);
  S2[2] = mux2x1(SS2,sinal,bb2||bb3);
  S2[3] = mux2x1(SS3,sinal,bb2||bb3);
  
}

void printASR(){
  if(clock1){
    Serial.println();
    Serial.println("CLOCK");
    Serial.println();
  }
  Serial.print("SubOUT  ");
  Serial.print(subOut[3]);
  Serial.print(subOut[2]);
  Serial.print(subOut[1]);
  Serial.println(subOut[0]);
  Serial.print("muxOut2  ");
  Serial.print(mux2x1Out2[3]);
  Serial.print(mux2x1Out2[2]);
  Serial.print(mux2x1Out2[1]);
  Serial.println(mux2x1Out2[0]);
  Serial.print("muxOut  ");
  Serial.print(mux2x1Out[3]);
  Serial.print(mux2x1Out[2]);
  Serial.print(mux2x1Out[1]);
  Serial.println(mux2x1Out[0]);
  Serial.print("RegistoI  ");
  Serial.print(registoIOut[3]);
  Serial.print(registoIOut[2]);
  Serial.print(registoIOut[1]);
  Serial.println(registoIOut[0]);
  Serial.print("ASROUT  ");
  Serial.print(shiftRightOUT[3]);
  Serial.print(shiftRightOUT[2]);
  Serial.print(shiftRightOUT[1]);
  Serial.println(shiftRightOUT[0]);
  Serial.print("muxOut3  ");
  Serial.print(mux2x1Out3[3]);
  Serial.print(mux2x1Out3[2]);
  Serial.print(mux2x1Out3[1]);
  Serial.println(mux2x1Out3[0]);
  Serial.print("muxOut4  ");
  Serial.print(mux2x1Out4[3]);
  Serial.print(mux2x1Out4[2]);
  Serial.print(mux2x1Out4[1]);
  Serial.println(mux2x1Out4[0]);
  Serial.print("RegistoS  ");
  Serial.print(registoSOut[3]);
  Serial.print(registoSOut[2]);
  Serial.print(registoSOut[1]);
  Serial.println(registoSOut[0]);
  Serial.print("FlagZero ");
  Serial.println(ZeroASR);
  Serial.println();
  Serial.println();
  
  
  delay(200);
  
  
  
}
void aluOUTprint(){
  Serial.print(A[3]);
  Serial.print(A[2]);
  Serial.print(A[1]);
  Serial.print(A[0]);
  if(!C[1]&&!C[0]){
    Serial.print(" + ");
  }
  else if(!C[1]&&C[0]){
    Serial.print(" - ");
  }
  else if(C[1]&&!C[0]){
    Serial.print(" >> ");
  }
  else{
    Serial.print(" NOR ");
  }
  Serial.print(B[3]);
  Serial.print(B[2]);
  Serial.print(B[1]);
  Serial.print(B[0]);
  Serial.print(" = ");
  if(C[1]&&!C[0]){
      Serial.print(S2[3]);
      Serial.print(S2[2]);
      Serial.print(S2[1]);
      Serial.println(S2[0]);
  }else{
      Serial.print(SOUT[3]);
      Serial.print(SOUT[2]);
      Serial.print(SOUT[1]);
      Serial.println(SOUT[0]);
  }
  Serial.print("Cy/Bw = ");
  if(!C[1]){
    Serial.println(CyBwOUT);
  }
  else{
    Serial.println("-");
  }
  Serial.print("Ov = ");
  if(!C[1]){
    Serial.println(OvOUT);
  }
  else{
    Serial.println("-");
  }
  Serial.print("Z = ");
  Serial.println(Zout);
  Serial.println();
}

void printNumbersBinary(){

  Serial.print("A = ");
  Serial.print(A[3]);
  Serial.print(A[2]);
  Serial.print(A[1]);
  Serial.println(A[0]);

  Serial.print("B = ");
  Serial.print(B[3]);
  Serial.print(B[2]);
  Serial.print(B[1]);
  Serial.println(B[0]);
  Serial.println();

  Serial.print(A[3]);
  Serial.print(A[2]);
  Serial.print(A[1]);
  Serial.print(A[0]);
  Serial.print(" + ");
  Serial.print(B[3]);
  Serial.print(B[2]);
  Serial.print(B[1]);
  Serial.print(B[0]);
  Serial.print(" = ");
  Serial.print(S0[3]);
  Serial.print(S0[2]);
  Serial.print(S0[1]);
  Serial.println(S0[0]);
  Serial.print("Cy/Bw = ");
  if(!0){
    Serial.println(Cy1bitH);
  }
  else{
    Serial.println("-");
  }
  Serial.print("Ov = ");
  if(!0){
    Serial.println(Ov0);
  }
  else{
    Serial.println("-");
  }
  Serial.print("Z = ");
  Serial.println(Z0);
  Serial.println();

  Serial.print(A[3]);
  Serial.print(A[2]);
  Serial.print(A[1]);
  Serial.print(A[0]);
  Serial.print(" - ");
  Serial.print(B[3]);
  Serial.print(B[2]);
  Serial.print(B[1]);
  Serial.print(B[0]);
  Serial.print(" = ");
  Serial.print(S1[3]);
  Serial.print(S1[2]);
  Serial.print(S1[1]);
  Serial.println(S1[0]);
  Serial.print("Cy/Bw = ");
  if(!0){
    Serial.println(Bw1bit);
  }
  else{
    Serial.println("-");
  }
  Serial.print("Ov = ");
  if(!0){
    Serial.println(Ov1);
  }
  else{
    Serial.println("-");
  }
  Serial.print("Z = ");
  Serial.println(Z1);
  Serial.println();

  Serial.print(A[3]);
  Serial.print(A[2]);
  Serial.print(A[1]);
  Serial.print(A[0]);
  Serial.print(" >> ");
  Serial.print(B[3]);
  Serial.print(B[2]);
  Serial.print(B[1]);
  Serial.print(B[0]);
  Serial.print(" = ");
  Serial.print(S2[3]);
  Serial.print(S2[2]);
  Serial.print(S2[1]);
  Serial.println(S2[0]);
  Serial.print("Cy/Bw = ");
  if(!1){
    Serial.println(CyBwOUT);
  }
  else{
    Serial.println("-");
  }
  Serial.print("Ov = ");
  if(!1){
    Serial.println(OvOUT);
  }
  else{
    Serial.println("-");
  }
  Serial.print("Z = ");
  Serial.println(Z2);
  Serial.println();
  
  Serial.print(A[3]);
  Serial.print(A[2]);
  Serial.print(A[1]);
  Serial.print(A[0]);
  Serial.print(" NOR ");
  Serial.print(B[3]);
  Serial.print(B[2]);
  Serial.print(B[1]);
  Serial.print(B[0]);
  Serial.print(" = ");
  Serial.print(S3[3]);
  Serial.print(S3[2]);
  Serial.print(S3[1]);
  Serial.println(S3[0]);
  Serial.println();
  Serial.print("Cy/Bw = ");
  if(!1){
    Serial.println(CyBwOUT);
  }
  else{
    Serial.println("-");
  }
  Serial.print("Ov = ");
  if(!1){
    Serial.println(OvOUT);
  }
  else{
    Serial.println("-");
  }
  Serial.print("Z = ");
  Serial.println(Z3);
  Serial.println();

}
void printNumbers(){
  Serial.print("A = ");
  Serial.println(A[0] + 2*A[1] + 4*A[2] + 8*A[3]);

  Serial.print("B = ");
  Serial.println(B[0] + 2*B[1] + 4*B[2] + 8*B[3]);

  Serial.print(A[0] + 2*A[1] + 4*A[2] + 8*A[3]);
  Serial.print( " + " );
  Serial.print(B[0] + 2*B[1] + 4*B[2] + 8*B[3]);
  Serial.print(" = ");
  Serial.println(S0[0] + 2*S0[1] + 4*S0[2] + 8*S0[3] + 16*Cy1bit);
  
  Serial.print("Carry = ");
  Serial.println((Cy1bit)?"TRUE":"FALSE");
  Serial.println("");
  Serial.print(A[0] + 2*A[1] + 4*A[2] + 8*A[3]);
  Serial.print( " - " );
  Serial.print(B[0] + 2*B[1] + 4*B[2] + 8*B[3]);
  Serial.print(" = ");
  if(!Bw1bit){

    Serial.println(S1[0] + 2*S1[1] + 4*S1[2] + 8*S1[3]);
    Serial.println("BORROW = FALSE");
  }else{
    Serial.print("-");
    Serial.println(!S1[0] + 2*!S1[1] + 4*!S1[2] + 8*!S1[3] + 1);
   
    Serial.println("BORROW = TRUE");
  }
  Serial.println("");
  
  
}
