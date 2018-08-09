#include "OLED_LIB_VGY12864L_03.h"

void OledDisplay::SetLowerColAddr(UI8 value) const {
   CommandDisplay(
         0,
         0,
         0,
         0,
         bitRead(value, 3),
         bitRead(value, 2),
         bitRead(value, 1),
         bitRead(value, 0)
         );
}

void OledDisplay::SetHigherColAddr(UI8 value) const {
   CommandDisplay(
         0,
         0,
         0,
         1,
         bitRead(value, 3),
         bitRead(value, 2),
         bitRead(value, 1),
         bitRead(value, 0)
         );
}

void OledDisplayParallel::SetDataPinsOutput() const {
   pinMode(DB0, OUTPUT);
   pinMode(DB1, OUTPUT);
   pinMode(DB2, OUTPUT);
   pinMode(DB3, OUTPUT);
   pinMode(DB4, OUTPUT);
   pinMode(DB5, OUTPUT);
   pinMode(DB6, OUTPUT);
   pinMode(DB7, OUTPUT);
}

void OledDisplayParallel::WriteData(UI8 d7, UI8 d6, UI8 d5, UI8 d4, UI8 d3, UI8 d2, UI8 d1, UI8 d0) const {
   digitalWrite(RW, LOW);
   digitalWrite(ERD, HIGH);
   digitalWrite(CS, LOW);  // data write operation is initiated when this
   // pin (R/W#) is pulled LOW and the CS# is pulled LOW

   digitalWrite(DB0, d0);
   digitalWrite(DB1, d1);
   digitalWrite(DB2, d2);
   digitalWrite(DB3, d3);
   digitalWrite(DB4, d4);
   digitalWrite(DB5, d5);
   digitalWrite(DB6, d6);
   digitalWrite(DB7, d7);

   digitalWrite(CS, HIGH);
   digitalWrite(ERD, LOW);
   digitalWrite(RW, HIGH);
}

void OledDisplaySpi::WriteData(UI8 d7, UI8 d6, UI8 d5, UI8 d4, UI8 d3, UI8 d2, UI8 d1, UI8 d0) const {
   UI8 buffer_data = 0x0;

   buffer_data |= (0x01 & d0) << 0;
   buffer_data |= (0x01 & d1) << 1;
   buffer_data |= (0x01 & d2) << 2;
   buffer_data |= (0x01 & d3) << 3;
   buffer_data |= (0x01 & d4) << 4;
   buffer_data |= (0x01 & d5) << 5;
   buffer_data |= (0x01 & d6) << 6;
   buffer_data |= (0x01 & d7) << 7;

   digitalWrite(ERD, HIGH);
   digitalWrite(RW, LOW);
   digitalWrite(CS, LOW);

   SPI.beginTransaction(SPISettings());
   SPI.transfer(buffer_data);

   digitalWrite(CS, HIGH);
   SPI.endTransaction();
}

void OledDisplay::CommandDisplay(UI8 d7, UI8 d6, UI8 d5, UI8 d4, UI8 d3, UI8 d2, UI8 d1, UI8 d0) const {
   digitalWrite(DC, LOW); // D7-D0 to command register
   WriteData(d7, d6, d5, d4, d3, d2, d1, d0);
}

void OledDisplay::DataDisplay(UI8 value) const {
   digitalWrite(DC, HIGH); // D7-D0 to display data
   WriteData(
         bitRead(value, 7),
         bitRead(value, 6),
         bitRead(value, 5),
         bitRead(value, 4),
         bitRead(value, 3),
         bitRead(value, 2),
         bitRead(value, 1),
         bitRead(value, 0)
         );
}

void OledDisplay::Nop() const {
   CommandDisplay(1, 1, 1, 0, 0, 0, 1, 1);
}

void OledDisplay::Reset() const {
   digitalWrite(RES, LOW);
   digitalWrite(RES, HIGH);
}

void OledDisplay::SetContrastReg(UI8 value) const {
   CommandDisplay(1, 0, 0, 0, 0, 0, 0, 1);
   CommandDisplay(
         bitRead(value, 7),
         bitRead(value, 6),
         bitRead(value, 5),
         bitRead(value, 4),
         bitRead(value, 3),
         bitRead(value, 2),
         bitRead(value, 1),
         bitRead(value, 0)
         );
}

void OledDisplay::SetColAddr(UI8 col) const {
   SetLowerColAddr(0xF & col);
   SetHigherColAddr(col >> 4);
}

void OledDisplay::SetPageAddr(UI8 value) const {
   CommandDisplay(
         1,
         0,
         1,
         1,
         0,
         bitRead(value, 2),
         bitRead(value, 1),
         bitRead(value, 0)
         );
}

void OledDisplay::ActivateHorScroll() const {
   CommandDisplay(0, 0, 1, 0, 1, 1, 1, 1);
}

void OledDisplay::DeactivateHorScroll() const {
   CommandDisplay(0, 0, 1, 0, 1, 1, 1, 0);
}

void OledDisplay::SetEntireDisplay(bool on) const {
   if (on) {
      CommandDisplay(1, 0, 1, 0, 0, 1, 0, 1);
   } else {
      CommandDisplay(1, 0, 1, 0, 0, 1, 0, 0);
   }
}

void OledDisplay::TurnOnDisplay(bool on) const {
   if (on) {
      CommandDisplay(1, 0, 1, 0, 1, 1, 1, 1);
   } else {
      CommandDisplay(1, 0, 1, 0, 1, 1, 1, 0);
   }
}

void OledDisplay::SetMultiplexRatio(UI8 value) const {
   // choose which section of the display
   // is turned on
   // 0-63 -> 1 triggers 2 rows; hardware error?
   // debugging -> if value is greater than 63, set value to 63
   if (value > 63) {
      value = 63;
   }
   CommandDisplay(1, 0, 1, 0, 1, 0, 0, 0);
   CommandDisplay(
         0,
         0,
         bitRead(value, 5),
         bitRead(value, 4),
         bitRead(value, 3),
         bitRead(value, 2),
         bitRead(value, 1),
         bitRead(value, 0)
         );
}

void OledDisplay::ClearAfterCol(UI8 col) const {
   for (UI8 i = 0; i <= 7; i++) {
      SetPageAddr(i); // all rows -> 8 pages x 8 rows / page = 64 rows
      SetColAddr(col);
      for (UI8 j = 0; j <= (127 - col); j++) { // all 128 columns
         DataDisplay(0);
      }
   }
}

void OledDisplay::ClearDisplay() const {
   ClearAfterCol(0);
}

void OledDisplay::SetPageValue(UI8 page, UI8 col, UI8 value) {
   UI8 row = ((page + 1) * 8) - 1;
   if (row > highest_row_) {
      highest_row_ = row;
   }
   SetPageAddr(page);
   SetColAddr(col);
   SetMultiplexRatio(highest_row_);
   DataDisplay(value);
   byte_set_matrix_[col][page] = value;
}

void OledDisplay::SetPixel(UI8 row, UI8 col) {
   UI8 page = row / 8; // int-division
   UI8 bit_in_page = row % 8; // mod-division
   UI8 buffer = byte_set_matrix_[col][page] | (0x1 << bit_in_page);
   SetPageValue(page, col, buffer);
}

void OledDisplay::DrawImage(UI8 *image, UI16 bytes_in_image) {
   for (UI16 i = 0; i < bytes_in_image; ++i) {
      SetPageValue(i % 8, (i / 8), image[i]);
   }
}

void OledDisplay::DrawImageInverted(UI8 *image, UI16 bytes_in_image) {
   for (UI16 i = 0; i < bytes_in_image; ++i) {
      SetPageValue(i % 8, (i / 8), ~image[i]);
   }
}

void OledDisplayParallel::Init() const {
   pinMode(DC, OUTPUT);
   pinMode(RW, OUTPUT);
   pinMode(ERD, OUTPUT);
   pinMode(CS, OUTPUT);
   pinMode(RES, OUTPUT);
   pinMode(M80, OUTPUT);
   pinMode(MS, OUTPUT);
   SetDataPinsOutput();
   Reset();

   digitalWrite(M80, HIGH); // mcu interface input selection pin; S003 & S005
   // only have 8-bit 8080-databus and spi
   digitalWrite(MS, HIGH); // select 8080-parallel interface

   TurnOnDisplay(false);
   DeactivateHorScroll();
   SetContrastReg(0x01);
   SetEntireDisplay(false);
   ClearDisplay();
   TurnOnDisplay(true);
}

void OledDisplaySpi::Init() const {
   pinMode(DC, OUTPUT);
   pinMode(RW, OUTPUT);
   pinMode(ERD, OUTPUT);
   pinMode(CS, OUTPUT);
   pinMode(RES, OUTPUT);
   pinMode(M80, OUTPUT);
   pinMode(MS, OUTPUT);
   Reset();

   digitalWrite(M80, LOW); // mcu interface input selection pin; S003 & S005
   // only have 8-bit 8080-databus and spi
   digitalWrite(MS, LOW);

   SPI.begin();
   TurnOnDisplay(false);
   DeactivateHorScroll();
   SetContrastReg(0x01);
   SetEntireDisplay(false);
   ClearDisplay();
   TurnOnDisplay(true);
}

OledDisplayParallel oled_parallel;
OledDisplaySpi oled_spi;

