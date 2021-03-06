/*
  dht22.cpp
  Digital Humidity and Temperature AM2302 (DHT22)
  AM2302 supply voltage range 3.3V - 5.5V, recommended supply voltage is 5V.
  Accuracy +- 5% temperature, +- 2% humidity
  http://www.aosong.com/
  Author: Paulo Morais
  Mention datasheet AM2302 and library SimpleDHT.h from  Winlin 2016-2017
  2021, Fevereiro de 2021
  This library was developed with the basic reading of the AM2302 (DHT22) sensor.
  It is possible to use several AM2302 sensors. The library uses the Arduino IDE.
  Está biblioteca foi desenvolvida com a leitura básica do sensor AM2302 (DHT22).
  É possível utilizar vários sensores AM2302. A biblioteca utiliza a IDE do Arduíno.
*/

# include "dht22.h"

dht22::dht22(int pin)
{
  this -> Dht22Port = pin;
}

void dht22::dht22Data()
{
  MsgError = "\0";
  this -> Temp = 0;
  this -> Hum = 0;
  byte Dht22Data[40];
  unsigned long tempo = 0;
  memset(Dht22Data, 0, 40);
  pinMode(Dht22Port, OUTPUT);
  digitalWrite(Dht22Port, LOW); //1.T(be), PULL LOW 1ms(0.8-20ms).
  delayMicroseconds(1000);
  digitalWrite(Dht22Port, HIGH);
  pinMode(Dht22Port, INPUT); //2.T(go), PULL HIGH 30us(20-200us).
  delayMicroseconds(40);
  if (digitalRead(Dht22Port) == LOW) {
    tempo = micros();
    while (digitalRead(Dht22Port) == LOW) {
      if ((tempo + 85) <= micros()) {
        MsgError = "ERR:RespLOWtime_S";
        return;
      }
    }
  }
  tempo = micros() - tempo;
  if (tempo < 30) {
    MsgError = "ERR:RelLOWtime_S";//3.T(rel), PULL LOW 80us(75-85us).
  }
  if (digitalRead(Dht22Port) == HIGH) {
    tempo = micros();
    while (digitalRead(Dht22Port) == HIGH) {
      if ((tempo + 85) <= micros()) {
        MsgError = "ERR:RelHIGHtime_S";
        return;
      }
    }
  }
  tempo = micros() - tempo;
  if (tempo < 50) MsgError = "ERR:RehHIGHtime_S"; //4.T(reh), PULL HIGH 80us(75-85us).
  for (int i = 0; i < 40; i++ ) {
    if (digitalRead(Dht22Port) == LOW) {
      tempo = micros();
      while (digitalRead(Dht22Port) == LOW) {
        if ((tempo + 85) <= micros()) {
          MsgError = "ERR:RehLOWtime_S";
          return;
        }
      }
    }
    tempo = micros() - tempo;
    if (tempo < 24) MsgError = "ERR:TLow_S"; //5.T(LOW), 1bit start, PULL LOW 50us(48-55us).
    if (digitalRead(Dht22Port) == HIGH) {
      tempo = micros();
      while (digitalRead(Dht22Port) == HIGH) {
        if ((tempo + 85) <= micros()) {
          MsgError = "ERR:TLow_S";
          return;
        }
      }
    }
    tempo = micros() - tempo;
    if (tempo < 11) {
      MsgError = "ERR:DataRead_S"; //6.T(H0), PULL HIGH 26us(22-30us), bit(0)
    }
    Dht22Data[i] = (tempo > 40 ? 1 : 0); //7.T(H1), PULL HIGH 70us(68-75us), bit(1)
  }
  if (digitalRead(Dht22Port) == LOW) {
    tempo = micros();
    while (digitalRead(Dht22Port) == LOW) {
      if ((tempo + 85) <= micros()) {
        MsgError = "ERR:DataRead_S";
        return;
      }
    }
  }
  tempo = micros() - tempo;
  if (tempo < 24) {
    MsgError = "ERR:DataEOF_S"; //8.T(en), PULL LOW 50us(45-55us).
  }
  else {
    short humidity = bits2byte(Dht22Data);
    short humidity2 = bits2byte(Dht22Data + 8);
    short temperature = bits2byte(Dht22Data + 16);
    short temperature2 = bits2byte(Dht22Data + 24);
    byte check = bits2byte(Dht22Data + 32);
    byte expect = (byte)humidity + (byte)humidity2 + (byte)temperature + (byte)temperature2;
    if (check != expect)MsgError = "ErrDataCheck_S";
    temperature = temperature << 8 | temperature2;
    humidity = humidity << 8 | humidity2;
    Temp = ((temperature & 0x8000 ? -1 : 1) * (temperature & 0x7FFF)) / 10.0;
    Hum = humidity / 10.0;
    MsgError = "SuccessData";
  }
}

byte dht22::bits2byte(byte data[8]) {
  byte v = 0;
  for (int i = 0; i < 8; i++) {
    v += data[i] << (7 - i);
  }
  return v;
}