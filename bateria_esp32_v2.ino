#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <WiFiManager.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <SPIFFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "esp32-hal-adc.h" // needed for adc pin reset
#include "soc/sens_reg.h" // needed for adc pin reset
uint64_t reg_b; // Used to store ADC2 control register
#include <driver/adc.h>
#include <AudioFileSourcePROGMEM.h>


#include "Standard.h"
#include "pop1.h"
#define LED_BUILTIN 2


#include <AudioGeneratorMIDI.h>

AudioFileSourcePROGMEM *sf2;
AudioFileSourcePROGMEM *mid;
AudioGeneratorMIDI *midii;

#include "der1cl.h"
#include "der1op.h"
#include "der2.h"
#include "der3.h"
#include "izq1.h"
#include "izq2.h"
#include "izq3.h"
#include "pie_der.h"
#include "pie_izq_cer.h"
#include "AudioGeneratorTalkie.h"
AudioGeneratorTalkie *talkie;

uint8_t spZERO[] PROGMEM = {0x69, 0xFB, 0x59, 0xDD, 0x51, 0xD5, 0xD7, 0xB5, 0x6F, 0x0A, 0x78, 0xC0, 0x52, 0x01, 0x0F, 0x50, 0xAC, 0xF6, 0xA8, 0x16, 0x15, 0xF2, 0x7B, 0xEA, 0x19, 0x47, 0xD0, 0x64, 0xEB, 0xAD, 0x76, 0xB5, 0xEB, 0xD1, 0x96, 0x24, 0x6E, 0x62, 0x6D, 0x5B, 0x1F, 0x0A, 0xA7, 0xB9, 0xC5, 0xAB, 0xFD, 0x1A, 0x62, 0xF0, 0xF0, 0xE2, 0x6C, 0x73, 0x1C, 0x73, 0x52, 0x1D, 0x19, 0x94, 0x6F, 0xCE, 0x7D, 0xED, 0x6B, 0xD9, 0x82, 0xDC, 0x48, 0xC7, 0x2E, 0x71, 0x8B, 0xBB, 0xDF, 0xFF, 0x1F};
uint8_t spONE[] PROGMEM = {0x66, 0x4E, 0xA8, 0x7A, 0x8D, 0xED, 0xC4, 0xB5, 0xCD, 0x89, 0xD4, 0xBC, 0xA2, 0xDB, 0xD1, 0x27, 0xBE, 0x33, 0x4C, 0xD9, 0x4F, 0x9B, 0x4D, 0x57, 0x8A, 0x76, 0xBE, 0xF5, 0xA9, 0xAA, 0x2E, 0x4F, 0xD5, 0xCD, 0xB7, 0xD9, 0x43, 0x5B, 0x87, 0x13, 0x4C, 0x0D, 0xA7, 0x75, 0xAB, 0x7B, 0x3E, 0xE3, 0x19, 0x6F, 0x7F, 0xA7, 0xA7, 0xF9, 0xD0, 0x30, 0x5B, 0x1D, 0x9E, 0x9A, 0x34, 0x44, 0xBC, 0xB6, 0x7D, 0xFE, 0x1F};
uint8_t spTWO[]  PROGMEM = {0x06, 0xB8, 0x59, 0x34, 0x00, 0x27, 0xD6, 0x38, 0x60, 0x58, 0xD3, 0x91, 0x55, 0x2D, 0xAA, 0x65, 0x9D, 0x4F, 0xD1, 0xB8, 0x39, 0x17, 0x67, 0xBF, 0xC5, 0xAE, 0x5A, 0x1D, 0xB5, 0x7A, 0x06, 0xF6, 0xA9, 0x7D, 0x9D, 0xD2, 0x6C, 0x55, 0xA5, 0x26, 0x75, 0xC9, 0x9B, 0xDF, 0xFC, 0x6E, 0x0E, 0x63, 0x3A, 0x34, 0x70, 0xAF, 0x3E, 0xFF, 0x1F};
uint8_t spTHREE[] PROGMEM = {0x0C, 0xE8, 0x2E, 0x94, 0x01, 0x4D, 0xBA, 0x4A, 0x40, 0x03, 0x16, 0x68, 0x69, 0x36, 0x1C, 0xE9, 0xBA, 0xB8, 0xE5, 0x39, 0x70, 0x72, 0x84, 0xDB, 0x51, 0xA4, 0xA8, 0x4E, 0xA3, 0xC9, 0x77, 0xB1, 0xCA, 0xD6, 0x52, 0xA8, 0x71, 0xED, 0x2A, 0x7B, 0x4B, 0xA6, 0xE0, 0x37, 0xB7, 0x5A, 0xDD, 0x48, 0x8E, 0x94, 0xF1, 0x64, 0xCE, 0x6D, 0x19, 0x55, 0x91, 0xBC, 0x6E, 0xD7, 0xAD, 0x1E, 0xF5, 0xAA, 0x77, 0x7A, 0xC6, 0x70, 0x22, 0xCD, 0xC7, 0xF9, 0x89, 0xCF, 0xFF, 0x03};
uint8_t spFOUR[] PROGMEM = {0x08, 0x68, 0x21, 0x0D, 0x03, 0x1C, 0x90, 0xC0, 0x88, 0x92, 0xB2, 0x29, 0x87, 0x76, 0x2B, 0x2D, 0x5C, 0x7B, 0x06, 0xD9, 0xED, 0x74, 0x64, 0x51, 0x54, 0x4D, 0xA2, 0xBA, 0x99, 0xB7, 0x3A, 0xF8, 0xEA, 0x74, 0xCA, 0xD3, 0x87, 0xBE, 0x94, 0x3D, 0xA4, 0xD4, 0x4E, 0xC9, 0x5F, 0xF7, 0x72, 0xA7, 0x9C, 0x1C, 0x63, 0xDE, 0xF2, 0x9B, 0xBE, 0x34, 0x84, 0x27, 0x2D, 0x4B, 0xF8, 0x53, 0x09, 0x9C, 0x0E, 0xB7, 0xEE, 0xCB, 0x5B, 0x70, 0xDD, 0xB2, 0xFC, 0x3F};
uint8_t spFIVE[] PROGMEM = {0x08, 0x68, 0x4E, 0x9D, 0x02, 0x1C, 0x60, 0xC0, 0x8C, 0x69, 0x12, 0xB0, 0xC0, 0x28, 0xAB, 0x8C, 0x9C, 0xC0, 0x2D, 0xBB, 0x38, 0x79, 0x31, 0x15, 0xA3, 0xB6, 0xE4, 0x16, 0xB7, 0xDC, 0xF5, 0x6E, 0x57, 0xDF, 0x54, 0x5B, 0x85, 0xBE, 0xD9, 0xE3, 0x5C, 0xC6, 0xD6, 0x6D, 0xB1, 0xA5, 0xBF, 0x99, 0x5B, 0x3B, 0x5A, 0x30, 0x09, 0xAF, 0x2F, 0xED, 0xEC, 0x31, 0xC4, 0x5C, 0xBE, 0xD6, 0x33, 0xDD, 0xAD, 0x88, 0x87, 0xE2, 0xD2, 0xF2, 0xF4, 0xE0, 0x16, 0x2A, 0xB2, 0xE3, 0x63, 0x1F, 0xF9, 0xF0, 0xE7, 0xFF, 0x01};
uint8_t spSIX[] PROGMEM = {0x04, 0xF8, 0xB9, 0x55, 0x01, 0xBF, 0xA5, 0x1B, 0xE0, 0x37, 0x0F, 0x03, 0xFC, 0x96, 0x61, 0x80, 0xDF, 0x3C, 0x0C, 0xF0, 0x93, 0xDA, 0x4A, 0x87, 0x49, 0x53, 0x91, 0x37, 0x2B, 0x6D, 0xD2, 0x55, 0x23, 0xDE, 0xAC, 0xAA, 0xB9, 0x16, 0x09, 0xFF, 0xB2, 0xFA, 0x66, 0x93, 0x3C, 0x63, 0xF3, 0x9C, 0x10, 0x38, 0x45, 0xA9, 0x0A, 0xD5, 0x00, 0x00, 0x02, 0x14, 0x97, 0xAA, 0x80, 0xAB, 0x6B, 0x0D, 0xF0, 0x63, 0xA5, 0x01, 0x7E, 0xCB, 0x30, 0xC0, 0x6F, 0xE9, 0x0A, 0xF8, 0x25, 0x93, 0x01, 0x3F, 0x57, 0xFC, 0x3F};
uint8_t spSEVEN[] PROGMEM = {0x0C, 0x78, 0xC6, 0x95, 0x01, 0x3F, 0xA5, 0x28, 0xE0, 0xB7, 0x52, 0x0B, 0x04, 0xE0, 0xB7, 0x54, 0x07, 0xFC, 0x52, 0x66, 0x80, 0x9F, 0xCB, 0x56, 0x59, 0x6C, 0x98, 0x47, 0x3E, 0x59, 0x4D, 0x89, 0x69, 0x66, 0xB9, 0x7A, 0x34, 0x39, 0x86, 0x9B, 0xD9, 0xA6, 0xD4, 0xE7, 0x10, 0xAE, 0xCA, 0xAD, 0x47, 0x97, 0x5D, 0x9A, 0xA9, 0xAE, 0x1A, 0x75, 0xB1, 0xA5, 0x66, 0xF9, 0xB2, 0xD4, 0x4D, 0x63, 0xB6, 0x84, 0x9A, 0x52, 0x76, 0x45, 0x11, 0x1A, 0xAA, 0x4B, 0x31, 0x0C, 0x79, 0x4A, 0xB0, 0x49, 0xC5, 0x50, 0xA2, 0x41, 0xA1, 0xC9, 0x65, 0xD9, 0xAB, 0x89, 0x56, 0x84, 0xFF, 0x07};
uint8_t spEIGHT[] PROGMEM = {0xAD, 0x1B, 0xDE, 0x58, 0x23, 0x17, 0xAF, 0x6E, 0xE8, 0x20, 0xEB, 0x5C, 0xBC, 0xEA, 0x61, 0x03, 0xAD, 0x73, 0xF6, 0xAA, 0x66, 0x4A, 0xD0, 0xCC, 0xD9, 0xAB, 0x1E, 0x45, 0xC1, 0xA3, 0x66, 0xAF, 0x61, 0x36, 0x03, 0x8B, 0x5A, 0x9C, 0xA6, 0x51, 0x04, 0xA2, 0x66, 0x96, 0xBB, 0xBA, 0x47, 0xEC, 0xED, 0x8A, 0x08, 0x9C, 0xA2, 0x54, 0x85, 0x6A, 0x00, 0x80, 0x02, 0x86, 0x74, 0x63, 0xC0, 0xE6, 0x6A, 0xFF, 0x0F};
uint8_t spNINE[] PROGMEM = {0x62, 0x75, 0xDC, 0x33, 0xB5, 0x62, 0xB9, 0x25, 0x0B, 0x8E, 0x34, 0x8F, 0xD3, 0xA6, 0xAE, 0x59, 0xD2, 0x34, 0xE9, 0x18, 0x87, 0x27, 0xC9, 0xD0, 0x78, 0xAB, 0x6F, 0xCE, 0xC2, 0x45, 0x1B, 0xAF, 0xB6, 0xF8, 0x51, 0x73, 0x9D, 0xB3, 0x9A, 0x12, 0xC7, 0x2C, 0x6D, 0xC9, 0xAA, 0x73, 0x4B, 0xF3, 0x88, 0x26, 0xAB, 0xCE, 0x2D, 0xCC, 0x23, 0xEA, 0xAC, 0x26, 0xB7, 0xD0, 0x88, 0x98, 0xB3, 0xBA, 0x5C, 0x43, 0x32, 0x7D, 0xCE, 0x1A, 0x4A, 0x0A, 0xCD, 0xF4, 0x39, 0x6B, 0xAA, 0x3E, 0x24, 0xCB, 0x67, 0xAF, 0xB5, 0xD9, 0xE4, 0x18, 0x5F, 0xB4, 0xF6, 0x6E, 0x92, 0x62, 0x7D, 0xD1, 0x38, 0xBA, 0x09, 0xF2, 0x8D, 0x59, 0xED, 0xE8, 0x3A, 0xD8, 0x27, 0x16, 0xA6, 0xBB, 0x7B, 0xF2, 0x74, 0x4F, 0x92, 0x9E, 0x1E, 0xC8, 0x4A, 0xC3, 0x49, 0x7A, 0x9A, 0x25, 0x2B, 0xF5, 0x24, 0xE9, 0xEE, 0xD1, 0x24, 0x25, 0x27, 0xBA, 0xBB, 0x25, 0x51, 0xD3, 0xAA, 0x24, 0xAE, 0x5E, 0x54, 0x55, 0x3A, 0xD4, 0xFF, 0x03};

uint8_t spEIGHTY[] PROGMEM = {0x63, 0xEA, 0xD2, 0x28, 0x37, 0x67, 0xAD, 0x7E, 0xF9, 0x26, 0xAC, 0x58, 0xB3, 0x9A, 0x91, 0x1C, 0x34, 0xB3, 0xC9, 0x68, 0x46, 0x13, 0x54, 0xEF, 0x25, 0xA3, 0x1E, 0x85, 0xD1, 0x7C, 0x96, 0x88, 0x21, 0x52, 0xB3, 0x71, 0x4F, 0xA4, 0x80, 0x25, 0x42, 0x4B, 0x37, 0x82, 0x82, 0xD9, 0x2C, 0x1E, 0xD3, 0x4C, 0x06, 0xEA, 0xB5, 0x64, 0xAC, 0xCB, 0x3B, 0x50, 0xE5, 0xE2, 0xB2, 0x2F, 0xEB, 0x40, 0x53, 0x8F, 0xCB, 0x39, 0xBD, 0x21, 0x75, 0x2D, 0xCE, 0x6F, 0x7A, 0x67, 0x12, 0x90, 0xEC, 0xD9, 0xFF, 0x0F};
uint8_t spPOINT[] PROGMEM = {0x0A, 0xC8, 0x2E, 0x8C, 0x01, 0xD1, 0x85, 0x95, 0x34, 0x8B, 0xAA, 0x74, 0x56, 0xBD, 0xCA, 0xE8, 0xA6, 0xCA, 0x29, 0xCB, 0x2A, 0x92, 0xEE, 0xCA, 0xA4, 0x36, 0x2B, 0x2F, 0xAA, 0x3C, 0x43, 0x9A, 0xAC, 0xBC, 0x78, 0xF3, 0xD0, 0x68, 0xD4, 0xB2, 0x6A, 0x45, 0x4A, 0x2A, 0xA6, 0xC9, 0x93, 0xC4, 0x29, 0x09, 0xC7, 0x24, 0x8D, 0xB1, 0xD3, 0xCD, 0x08, 0x21, 0x70, 0x8A, 0x52, 0x15, 0xAA, 0x01, 0xE0, 0x80, 0x63, 0xC2, 0x14, 0x70, 0x94, 0x19, 0x01, 0xA6, 0x32, 0xFB, 0x7F};
uint8_t spAND[] PROGMEM = {0x6B, 0xA8, 0x2E, 0x79, 0xAC, 0x37, 0xAE, 0xA5, 0xBA, 0xE4, 0xCE, 0x5C, 0xB8, 0xA6, 0xE6, 0x92, 0xB3, 0x62, 0xD1, 0x9A, 0xAA, 0x2B, 0x8E, 0x8A, 0x45, 0x6B, 0xAA, 0xA6, 0x38, 0x33, 0x66, 0x97, 0xA9, 0x39, 0xCC, 0xD0, 0xF0, 0x9C, 0xA6, 0x66, 0xB1, 0x5C, 0xD2, 0xB3, 0x99, 0x93, 0xA4, 0x0E, 0x19, 0x17, 0x64, 0xE3, 0xBA, 0x34, 0xB3, 0x6D, 0x96, 0xA7, 0x69, 0x53, 0x57, 0xDD, 0xD2, 0x96, 0x6A, 0x8A, 0x45, 0xA2, 0xF5, 0xFF, 0x03};
uint8_t spCHECK[] PROGMEM = {0x0C, 0xD8, 0x39, 0xC4, 0x01, 0xBB, 0xA6, 0x04, 0x60, 0xB6, 0x90, 0x04, 0x8C, 0x54, 0xBE, 0x8A, 0x9A, 0x5D, 0x3D, 0x7D, 0xCD, 0x2E, 0x56, 0xD1, 0x7C, 0x49, 0x94, 0xAF, 0x5E, 0x79, 0x2B, 0xC5, 0xA1, 0xB6, 0x18, 0x81, 0x53, 0x94, 0xAA, 0x50, 0x0D, 0x00, 0x06, 0xD4, 0x39, 0xC4, 0x80, 0xBA, 0x8C, 0x11, 0x10, 0x1D, 0xF1, 0xFF, 0x03};

uint8_t spAT[] PROGMEM = {0xA9, 0xAC, 0x36, 0xB9, 0x32, 0x1F, 0xAD, 0xB5, 0xBA, 0x94, 0x4C, 0x9F, 0xBD, 0xA6, 0xE6, 0x4A, 0x23, 0xED, 0xF1, 0x1A, 0xBB, 0x19, 0xD3, 0xB4, 0xCD, 0x6B, 0xEC, 0x6A, 0xCC, 0xD2, 0x36, 0xAF, 0xB1, 0xCB, 0x91, 0x70, 0x7B, 0x8D, 0xC0, 0x29, 0x4A, 0x55, 0xA8, 0x06, 0x80, 0x00, 0xA6, 0x54, 0x37, 0xC0, 0x55, 0x2A, 0x04, 0x58, 0x9C, 0xF5, 0xFF, 0x01};





uint8_t *spNumber[] = { spZERO, spONE, spTWO, spTHREE, spFOUR, spFIVE, spSIX,
                        spSEVEN, spEIGHT, spNINE
                      };
size_t   spNumberLen[] = { sizeof(spZERO), sizeof(spONE), sizeof(spTWO),
                           sizeof(spTHREE), sizeof(spFOUR), sizeof(spFIVE),
                           sizeof(spSIX), sizeof(spSEVEN), sizeof(spEIGHT),
                           sizeof(spNINE)
                         };

unsigned long t0 = millis();
long freqTamborMillis = 50.0; //600bpm
int j = 0;



#include "AppleMIDI.h"
USING_NAMESPACE_APPLEMIDI

#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "AudioOutputMixer.h"

AsyncWebServer server(80);

APPLEMIDI_CREATE_INSTANCE(WiFiUDP, MIDI, "ESP32", DEFAULT_CONTROL_PORT);

boolean wifiEnabled = false;


AudioOutputI2S *out; //internal DAC channel 1 (pin25) on ESP32
AudioOutputMixerStub *stub[8];
AudioOutputMixer *mixer;
AudioGeneratorWAV *wav[8];
AudioFileSourcePROGMEM *file[8];
boolean Running[8];
long start[8];
int adc_gpios[8] = {36,  34, 35, 32, 33, 27, 14, 12};

int valoresMaximos[8];

int configMax[8];
int configMin[8];
double configVol[8];

boolean pedalHit = false;
boolean pararSplash = false;

boolean playMidi = false;


int eepromInicializada = 0; //eeprom pos 1. Está a 1 si la placa está inicializada



void setup()
{
  //TODO: Bajarlo antes de activar el wifi
  reg_b = READ_PERI_REG(SENS_SAR_READ_CTRL2_REG);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  if(!SPIFFS.begin()){ 
    Serial.println("An Error has occurred while mounting SPIFFS");  
  }
  EEPROM.begin(512);
  delay(1000);
  leerEeprom(); //Lee las variables configurables de la Eeprom y si tienen valores inválidos, las inicializa

  if (wifiEnabled)
  {
    WiFiManager wifiManager;
    wifiManager.autoConnect("bateria");
  }
  audioLogger = &Serial;

  out = new AudioOutputI2S();

  mixer = new AudioOutputMixer(32, out);
  for (int i = 0; i < 8; i++)
  {
    stub[i] = mixer->NewInput();
    start[i] = 0;
    pinMode(adc_gpios[i], INPUT);


  }


  pinMode(15, INPUT); //boton para activar wifi
  delay(100);




  if (wifiEnabled)
    initWifi();
  resetValoresMax();

  sf2 = new AudioFileSourcePROGMEM(Standard_sf2, Standard_sf2_len);


}

void loop()
{

  if (wifiEnabled && !midii->isRunning())
    MIDI.read();
  //Serial.println("a:)");
  if (midii->isRunning()) {
    if (!midii->loop()) {
      midii->stop();
      if (playMidi)
      {
        delete mid; delete midii;
        mid = new AudioFileSourcePROGMEM(pop1_mid, pop1_mid_len);

        midii = new AudioGeneratorMIDI();
        midii->SetSoundfont(sf2);
        midii->SetSampleRate(22050);
        midii->begin(mid, out);

      }
    }
  }

  // if condition checks if push button is pressed
  if ( digitalRead(15) == HIGH )
  {
    // Serial.println("c:)");
    EnableWifi();
  }
  leerPiezos();
  //Serial.println("b:)");

  for (int i = 0; i < 8; i++)
  {

    if (i == 1 && pararSplash && Running[i])
    {

      //Serial.println("c:)");

      pararSplash = false;
      //Serial.println("d:)");

      wav[i]->stop();
      //Serial.println("e:)");
      stub[i]->SetGain(0.0);

      stub[i]->stop();
      //Serial.println("f:)");
      // delete wav[i];
      //   delete file[i];
      Running[i] = false; Serial.printf("stopping %d \n", i);


    } else if (Running[i] && wav[i]->isRunning()) {

      //Serial.println("g:)");

      if (!wav[i]->loop()) {
        //   Serial.println("h:)");

        wav[i]->stop();
        //  Serial.println("i:)");
        stub[i]->SetGain(0.0);
        stub[i]->stop();
        //        delete wav[i];
        //    delete file[i];
        Running[i] = false; Serial.printf("stopping %d \n", i);


      }



    }
  }

  // Serial.println("j:)");



}





//inicializa el wifi
void initWifi()
{
  Serial.println("connected...yeey :)");

  //server.reset(new ESPAsyncWebServer(WiFi.localIP(), 80));

server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200);
      }, handleUpload);

      
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    for (int i = 0; i < 8; i++)
    {
      if (request->hasParam("piezo_" + (String) i + "_max"))
      {
        int valor = request->getParam("piezo_" + (String) i + "_max")->value().toInt();
        EEPROM.put(10 + (i * 10), valor);
        EEPROM.commit();
        Serial.println("Nuevo parametro piezo_" + (String) i + "_max: " + String(valor));
        configMax[i] = valor;

      }

      if (request->hasParam("piezo_" + (String) i + "_min"))
      {
        int valor = request->getParam("piezo_" + (String) i + "_min")->value().toInt();
        EEPROM.put(90 + (i * 10), valor);
        EEPROM.commit();
        Serial.println("Nuevo parametro piezo_" + (String) i + "_min: " + String(valor));
        configMin[i] = valor;
      }
      if (request->hasParam("vol_" + (String) i ))
      {
        float valor = request->getParam("vol_" + (String) i )->value().toFloat();
        EEPROM.put(170 + (i * 10), valor);
        EEPROM.commit();
        Serial.println("Nuevo parametro vol_" + (String) i + String(valor));
        configVol[i] = valor;
      }

    }


    if (request->hasParam("accion") && request->getParam("accion")->value() == "ResetMax")
      resetValoresMax();

    if (request->hasParam("accion") && request->getParam("accion")->value() == "PlayMidi")
    {
      mid = new AudioFileSourcePROGMEM(pop1_mid, pop1_mid_len);

      midii = new AudioGeneratorMIDI();
      midii->SetSoundfont(sf2);
      midii->SetSampleRate(22050);
      Serial.printf("BEGIN...\n");
      midii->begin(mid, out);
      playMidi = true;
    }


    if (request->hasParam("accion") && request->getParam("accion")->value() == "StopMidi")
    {
      midii->stop();
      delete mid; delete midii;

      playMidi = false;
    }


    String strRoot = "<!DOCTYPE HTML><html>\
    <body>\
    <h1>ESP32 BATERÍA</h1>";
    for (int i = 0; i < 8; i++)
    {
      strRoot = strRoot + "<br><form>\
    <label for=\"fname\">PIEZO " + (String) i + " (Máx actual: " + (String)valoresMaximos[i] + ") </label>\
    Umbral Min: <input type=\"number\" style=\"width: 4em;\" min=\"1\" max=\"4095\" id=\"piezo_" + (String) i + "_min\" name=\"piezo_" + (String) i + "_min\" value=\"" +  (String)configMin[i] + "\">\
    Umbral Max: <input type=\"number\" style=\"width: 4em;\" min=\"1\" max=\"4095\" id=\"piezo_" + (String) i + "_max\" name=\"piezo_" + (String) i + "_max\" value=\"" +  (String)configMax[i] + "\">\
    Volumen: <input type=\"number\" step=\"0.01\" style=\"width: 4em;\" min=\"0\" max=\"10\" id=\"vol_" + (String) i + "\" name=\"vol_" + (String) i + "\" value=\"" +  (String)configVol[i] + "\">\
    <input type=\"submit\" value=\"Envia\"><br>\
  </form><br>";
    }

    strRoot = strRoot + "<form>\
    <button name=\"accion\" type=\"submit\" value=\"ResetMax\">Resetea máximos</button><br>\
    </form>\
    <form>\
    <button name=\"accion\" type=\"submit\" value=\"PlayMidi\">Play Midi</button><br>\
    </form>\
    <form>\
    <button name=\"accion\" type=\"submit\" value=\"StopMidi\">Stop Midi</button><br>\
    </form>\
    <form action=\"upload\" method=\"post\" enctype=\"multipart/form-data\" name=\"data\"><input type=\"file\"><button type=\"submit\">Upload midi</button></form>\
    </body>\
    </html>";


    request->send(200, "text/html", strRoot);
  });
  server.begin();                //Start server


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.print(F("IP address is "));
  Serial.println(WiFi.localIP());

  Serial.println(F("OK, now make sure you an rtpMIDI session that is Enabled"));
  Serial.print(F("Add device named Arduino with Host/Port "));
  Serial.print(WiFi.localIP());
  Serial.println(F(":5004"));
  Serial.println(F("Then press the Connect button"));
  Serial.println(F("Then open a MIDI listener (eg MIDI-OX) and monitor incoming notes"));

  /* ESP8266SAM *sam = new ESP8266SAM;

    String msg_ip = "Wifi and midi is enabled. Port to connect midi 5004. i p " + (String)WiFi.localIP().toString() + ".";
    const char*  msg_ip2 = msg_ip.c_str();
    sam->Say(out, msg_ip2);*/
  // String addre = (String)WiFi.localIP().toString();
  talkie = new AudioGeneratorTalkie();
  talkie->begin(nullptr, out);
  String addre = String(WiFi.localIP()[0]) + "." + WiFi.localIP()[1] + "." + WiFi.localIP()[2] + "." + WiFi.localIP()[3];
  talkie->say(spCHECK, sizeof(spCHECK));
  for (int i = 0; i < addre.length(); i++)
  {
    if (addre[i] == '.')
      talkie->say(spPOINT, sizeof(spPOINT));
    else
      talkie->say(spNumber[addre[i] - '0'], spNumberLen[addre[i] - '0']);



  }
  talkie->say(spAT, sizeof(spAT));
  talkie->say(spEIGHTY, sizeof(spEIGHTY));
  talkie->say(spAND, sizeof(spAND));
  for (int i = 0; i < addre.length(); i++)
  {
    if (addre[i] == '.')
      talkie->say(spPOINT, sizeof(spPOINT));
    else
      talkie->say(spNumber[addre[i] - '0'], spNumberLen[addre[i] - '0']);



  }
  talkie->say(spAT, sizeof(spAT));
  talkie->say(spNumber[5], spNumberLen[5]);
  talkie->say(spNumber[0], spNumberLen[0]);
  talkie->say(spNumber[0], spNumberLen[0]);
  talkie->say(spNumber[4], spNumberLen[4]);





  MIDI.begin(1);
  Beginplay(3, der3, der3_len, 0.1, 51);

}




//si es la primera vez que conectamos la placa, se inicializa la eeprom
void inicializarEeprom()
{
  Serial.println("Inicializando Eeprom, tardaremos 10 segundos...");

  for (int i = 0; i < 16; i++)
  {
    EEPROM.put(10 + (i * 10), i);
    EEPROM.commit();

  }
}



//Lee las variables configurables de la Eeprom y si tienen valores inválidos, las inicializa
void leerEeprom()
{
  for (int i = 0; i < 16; i++)
  {
    int valor = 0;
    EEPROM.get(10 + (i * 10), valor);
    if (valor < 1 || valor > 4095)
    {
      valor = 512;
      EEPROM.put(10 + (i * 10), valor);
      EEPROM.commit();
    }
    if (i < 8)
    {
      configMax[i] = valor;
    }
    else
    {
      configMin[i - 8] = valor;

    }



  }

  for (int i = 16; i < 24; i++)
  {
    float valor = 1.0;
    EEPROM.get(10 + (i * 10), valor);
    if (valor < 0.0 || valor > 10.0)
    {
      valor = 1.0;
      EEPROM.put(10 + (i * 10), valor);
      EEPROM.commit();
    }

    configVol[i - 16] = valor;




  }



  EEPROM.get(1, eepromInicializada);
  if (eepromInicializada != 1)
  {
    inicializarEeprom();
    eepromInicializada = 1;
    EEPROM.put(1, eepromInicializada);
    EEPROM.commit();
  }
}


void Beginplay(int Channel, const void *wavfilename, int sizewav, float Volume, byte note) {
  if (millis() - start[Channel] > freqTamborMillis)
  {
    if (midii->isRunning())
    {
      midii->playSingleNote(note, Volume);
    }
    else
    {
      //  Volume=1.0;

      Serial.printf("CH:");
      Serial.print(Channel);

      stub[Channel]->SetGain(Volume);

      //    delete wav[Channel];


      wav[Channel] = new AudioGeneratorWAV();

      //  delete file[Channel]; // housekeeping ?
      file[Channel] = new AudioFileSourcePROGMEM( wavfilename, sizewav );


      wav[Channel]->begin(file[Channel], stub[Channel]);

      Running[Channel] = true;
      Serial.printf("> at volume :");
      Serial.println(Volume);
    }
    start[Channel] = millis();
    t0 = start[Channel];


    if (wifiEnabled && !midii->isRunning() )
    {
      byte velocity = Volume * 127;
      byte channel = 1;

      MIDI.sendNoteOn(note, velocity, channel);
      MIDI.sendNoteOff(note, velocity, channel);
    }
  }
}

void leerPiezos() {

  /* if (!midii->isRunning())
    {*/
  int max = 0;
  int valorMax = -1;
  for (int i = 0; i < 8  ; i++)
  {

    if (i > 4 && wifiEnabled)
    {
      WRITE_PERI_REG(SENS_SAR_READ_CTRL2_REG, reg_b);
      //VERY IMPORTANT: DO THIS TO NOT HAVE INVERTED VALUES!
      SET_PERI_REG_MASK(SENS_SAR_READ_CTRL2_REG, SENS_SAR2_DATA_INV);

    }

    int val = analogRead(adc_gpios[i]);
    if (i > 4 && val == 4095 && wifiEnabled)
      val = 0;


    if (i == 0 && pedalHit && configMin[i] >= val)
    {

      pedalHit = false;


      Beginplay(0, pie_izq_cer, pie_izq_cer_len, 0.2, 44);
      pararSplash = true;

    }
    //

    if (i == 0 && configMin[i] < val)
    {
      pedalHit = true;

    }
    if (val > max && i > 0)
    {
      max = val;
      valorMax = i;

    }
    /**
       Single
    */
    //int val=analogRead(A0);
    // if(i==7)
    // Serial.printf("valorrrrr %d : %d \n", i, val);

    if (valoresMaximos[i] < val)
      valoresMaximos[i] = val;
  }

  /*if(configMin[valorMax] < max && valorMax >-1)
    {
    Serial.printf("valorrrrr %d : %d \n", valorMax, max);
    }*/

  if (configMin[valorMax] < max)
  {

    if (valorMax == 1 && pedalHit)
      Beginplay(1, der1op, der1op_len, ((float)max / (float)configMax[valorMax])*configVol[valorMax], 46);
    else if (valorMax == 1 && !pedalHit)
      Beginplay(1, der1cl, der1cl_len, ((float)max / (float)configMax[valorMax])*configVol[valorMax], 42);

    else if (valorMax == 2)
    {
      /* float volu=((float)max / (float)configMax[valorMax])*configVol[valorMax];
         Serial.printf("antes del BEGIN. valorMax: ");
         Serial.print(valorMax);
         Serial.print(" . Max: ");
         Serial.print(max);
         Serial.print(".  ConfigMax: ");
         Serial.print(configMax[valorMax]);
         Serial.print(". configVol: ");
         Serial.print(configVol[valorMax]);
         Serial.print(" . Volu: ");
          Serial.print(volu);*/

      Beginplay(2, der2, der2_len, ((float)max / (float)configMax[valorMax])*configVol[valorMax], 49);
    }
    else if (valorMax == 3)
      Beginplay(3, der3, der3_len, ((float)max / (float)configMax[valorMax])*configVol[valorMax], 51);
    else if (valorMax == 4)
      Beginplay(4, izq1, izq1_len, ((float)max / (float)configMax[valorMax])*configVol[valorMax], 41);
    else if (valorMax == 5)
      Beginplay(5, izq2, izq2_len, ((float)max / (float)configMax[valorMax])*configVol[valorMax], 38);
    else if (valorMax == 6)
      Beginplay(6, izq3, izq3_len, ((float)max / (float)configMax[valorMax])*configVol[valorMax], 50);
    else if (valorMax == 7)
      Beginplay(7, pie_der, pie_der_len, ((float)max / (float)configMax[valorMax])*configVol[valorMax], 36);








    //  }



  }
}

void resetValoresMax() {
  for (int i = 0; i < 8; i++)
  {
    valoresMaximos[i] = 0;


  }
}

void EnableWifi()
{
  wifiEnabled = true;

  WiFiManager wifiManager;

  wifiManager.autoConnect("bateria");

  /* ESP8266SAM *sam = new ESP8266SAM;


    sam->Say(out, "No wifi found. An access point has been createad to config your wifi parameters. The ip off the captive portal is 192 168 4 1. ");
    delay(5000);

  */
  initWifi();
}



void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  if(!index){
     Serial.println((String)"UploadStart: " + filename);
    // open the file on first call and store the file handle in the request object
    request->_tempFile = SPIFFS.open("/"+filename, "w");
  }
  if(len) {
    // stream the incoming chunk to the opened file
    request->_tempFile.write(data,len);
  }
  if(final){
    Serial.println((String)"UploadEnd: " + filename + ",size: " + index+len);
    // close the file handle as the upload is now done
    request->_tempFile.close();
    request->redirect("/");
  }
}
