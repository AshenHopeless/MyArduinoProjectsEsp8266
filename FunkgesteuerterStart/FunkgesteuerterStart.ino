  int funk = 3;
  int trigger = 12;
  int span = 5;
  int gnd = 6;
  int span2 = 13;

void setup() {
  // put your setup code here, to run once:

  pinMode(funk, INPUT);
  pinMode(trigger, OUTPUT);
  pinMode(span, OUTPUT);
  pinMode(gnd, OUTPUT);
  pinMode(span2, OUTPUT);
  digitalWrite(span, HIGH);
  digitalWrite(gnd, LOW);
  digitalWrite(trigger, HIGH);
  digitalWrite(trigger, HIGH);
  digitalWrite(span2, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(digitalRead(funk))
  {
    digitalWrite(trigger, HIGH);
  }
  else
  {
    digitalWrite(trigger, LOW);
  }

}
