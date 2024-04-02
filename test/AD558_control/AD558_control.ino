void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  static byte data = B00001111;
  digitalWrite(2, data & 0b00000001);
  digitalWrite(3, (data >> 1) & 0b00000001);
  digitalWrite(4, (data >> 2) & 0b00000001);
  digitalWrite(5, (data >> 3) & 0b00000001);
  digitalWrite(6, (data >> 4) & 0b00000001);
  digitalWrite(9, (data >> 5) & 0b00000001);
  digitalWrite(11, (data >> 6) & 0b00000001);
  digitalWrite(12, (data >> 7) & 0b00000001);

  //data++;
  Serial.println(data);
  //delay(1000);

}
