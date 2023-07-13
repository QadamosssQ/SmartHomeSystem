#define r 2
#define g 3
#define b 4


void setup() {
  pinMode(r, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(b, OUTPUT);

  analogWrite(r,100);
  analogWrite(g,100);
  analogWrite(b,100);

}

void loop() {

}
