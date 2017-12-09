int inPin_1 = 8;
int inPin_2 = 7;
int inPin_3 = 6;
int inPin_4 = 5;

int val_1 = 0;
int val_2 = 0;
int val_3 = 0;
int val_4 = 0;

void setup() {
  pinMode(inPin_1, INPUT);
  pinMode(inPin_2, INPUT);
  pinMode(inPin_3, INPUT);
  pinMode(inPin_4, INPUT);

  Serial.begin(9600);
}

void loop() {
  val_1 = digitalRead(inPin_1);
  val_2 = digitalRead(inPin_2);
  val_3 = digitalRead(inPin_3);
  val_4 = digitalRead(inPin_4);

  Serial.print(val_1);
  Serial.print(" ");
  Serial.print(val_2);
  Serial.print(" ");
  Serial.print(val_3);
  Serial.print(" ");
  Serial.println(val_4);

  delay(100);
}
