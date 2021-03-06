#include <dht22.h>

dht22 dht01(26);
dht22 dht02(25);
dht22 dht03(33);

void setup() {
  Serial.begin(115200);
}

void loop() {
  dht01.dht22Data();
  Serial.println (dht01.Temp);
  Serial.println (dht01.Hum);
  Serial.println (dht01.MsgError + ":dht01\n");
  dht02.dht22Data();
  Serial.println (dht02.Temp);
  Serial.println (dht02.Hum);
  Serial.println (dht02.MsgError + ":dht02\n");
  dht03.dht22Data();
  Serial.println (dht03.Temp);
  Serial.println (dht03.Hum);
  Serial.println (dht03.MsgError + ":dht03\n");
  delay(2000);
}
