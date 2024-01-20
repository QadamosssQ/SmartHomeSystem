//display_functions.cpp:
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

extern Adafruit_ST7789 tft; 


void zero() {

  tft.setCursor(10, 20);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_MAGENTA);
  tft.setTextSize(4);
  tft.println("Scan card");

  tft.fillRoundRect(30, 80, 190, 230, 10, ST77XX_MAGENTA);
  tft.fillRoundRect(35, 85, 180, 220, 10, ST77XX_BLACK);
  tft.fillRect(55, 85, 35, 220, ST77XX_MAGENTA);
  tft.fillCircle(145, 140, 40, ST77XX_MAGENTA);
  tft.fillCircle(145, 140, 25, ST77XX_BLACK);
  tft.fillRoundRect(140, 170, 15, 110, 10, ST77XX_MAGENTA);
  tft.fillRect(155, 240, 15, 10, ST77XX_MAGENTA);
  tft.fillRect(155, 260, 15, 10, ST77XX_MAGENTA);
  tft.fillRect(155, 250, 5, 10, ST77XX_MAGENTA);
}

void denied() {

  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(50, 10);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(4);

  tft.println("Access");
  tft.setCursor(50, 50);
  tft.println("danied");
  tft.setTextSize(3.5);
  tft.setCursor(30, 100);
  tft.println("wrong card");

  tft.fillCircle(120, 200, 50, ST77XX_RED);
  tft.fillCircle(120, 200, 35, ST77XX_BLACK);
  tft.fillRoundRect(55, 200, 130, 100, 15, ST77XX_RED);
  tft.fillCircle(120, 250, 15, ST77XX_BLACK);
  tft.fillRect(113, 260, 15, 25, ST77XX_BLACK);
}

void granted() {

  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(50, 20);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(4);
  tft.println("Access");
  tft.setCursor(40, 60);
  tft.println("granted");

  tft.fillCircle(120, 190, 50, ST77XX_GREEN);
  tft.fillCircle(120, 190, 35, ST77XX_BLACK);
  tft.fillRect(70, 180, 60, 20, ST77XX_BLACK);
  tft.fillRoundRect(55, 200, 130, 100, 15, ST77XX_GREEN);
  tft.fillCircle(120, 250, 15, ST77XX_BLACK);
  tft.fillRect(113, 260, 15, 25, ST77XX_BLACK);
}


void switch_granted() {

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.setTextSize(3);
  tft.setCursor(15, 20);
  tft.println("Safty switch");
  tft.setCursor(40, 60);
  tft.println("triggered");
  tft.setCursor(30, 100);
  tft.println("doors open");

  tft.fillCircle(120, 190, 50, ST77XX_GREEN);
  tft.fillCircle(120, 190, 35, ST77XX_BLACK);
  tft.fillRect(70, 180, 60, 20, ST77XX_BLACK);
  tft.fillRoundRect(55, 200, 130, 100, 15, ST77XX_GREEN);
  tft.fillCircle(120, 250, 15, ST77XX_BLACK);
  tft.fillRect(113, 260, 15, 25, ST77XX_BLACK);
}