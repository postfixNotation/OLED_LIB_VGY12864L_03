// @author: Peter Getek (postfixNotation)
// @email:  postfixNotation@gmail.com
// @date:   9. August 2018
// @description:  This is a library for an OLED display module
//                called "VGY12864L-03" or generally 12864L.
//                The version I've been working on has yellow
//                colored pixels.
//                Both a parallel and a serial interface version
//                have been implemented and tested using an Arduino Due.
//                Feel free to ask questions and to submit improvements!
// @github:       https://github.com/postfixNotation/OLED_LIB_VGY12864L_03

#ifndef OLED_LIB_VGY12864L_03_H_
#define OLED_LIB_VGY12864L_03_H_

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <SPI.h>
#include <stdint.h>

using UI8 = uint8_t;
using UI16 = uint16_t;

// pins labels according to the oled display manual
#define DC    22
#define RW    23
#define ERD   24
#define DB0   25
#define DB1   26
#define DB2   27
#define DB3   28
#define DB4   29
#define DB5   30
#define DB6   31
#define DB7   32
#define CS    33
#define RES   34
#define M80   35
#define MS    36

// display data is written pagewise
// the SetPixel() method stores which pixels of a page
// have been set; the purpose is to not overwrite already
// set pixels
#define PAGES 7
#define COLS  128

class OledDisplay {
   private:
      UI8 highest_row_{ 0 }; // for calls to set_multiplex_ratio()
      UI8 byte_set_matrix_[COLS][PAGES];

      virtual void WriteData(UI8, UI8, UI8, UI8, UI8, UI8, UI8, UI8) const = 0;
      void SetLowerColAddr(UI8) const;
      void SetHigherColAddr(UI8) const;
      void Nop() const;

   protected:
      void Reset() const;

   public:
      OledDisplay() {}
      virtual ~OledDisplay() {}

      // send data to command register
      void CommandDisplay(UI8, UI8, UI8, UI8, UI8, UI8, UI8, UI8) const;
      // send data to display data register
      void DataDisplay(UI8) const;
      // set contrast using values from 0-255 (0x00 - 0xFF)
      void SetContrastReg(UI8) const;
      // set column from 0-127
      void SetColAddr(UI8) const;
      // set page from 0-7
      void SetPageAddr(UI8) const;
      void ActivateHorScroll() const;
      void DeactivateHorScroll() const;
      // usually you set this to 'false'
      // this method sets every pixel of the display
      void SetEntireDisplay(bool) const;
      void TurnOnDisplay(bool) const;
      // usually you don't have to call this method
      // explicitly; this method basically sets how many
      // rows are turned on so that pixels set are visible
      // or in other words: "how many rows are illuminated"
      void SetMultiplexRatio(UI8) const;
      void ClearAfterCol(UI8) const;

      // sets a single pixel; parameters from (0,0) to (63, 127) are possible
      // parameters are: (row,column)
      void SetPixel(UI8, UI8);
      void ClearDisplay() const;
      // you actually write pagewise
      // parameters: (page, column, 8-bit value)
      void SetPageValue(UI8, UI8, UI8);
      // the image is draw columnwise, you need a byte-array
      // of 64 x 128 elements
      void DrawImage(UI8*, UI16);
      void DrawImageInverted(UI8*, UI16);
      // you have to call this method first
      virtual void Init() const = 0;

};

class OledDisplayParallel : public OledDisplay {
   private:
      void SetDataPinsOutput() const;
      virtual void WriteData(UI8, UI8, UI8, UI8, UI8, UI8, UI8, UI8) const;
   public:
      virtual void Init() const;
};

class OledDisplaySpi : public OledDisplay {
   private:
      virtual void WriteData(UI8, UI8, UI8, UI8, UI8, UI8, UI8, UI8) const;
   public:
      virtual void Init() const;
};

// use these objects if you want
// to save some memory you should consider to just
// create instances of the class you need
extern OledDisplayParallel oled_parallel;
extern OledDisplaySpi oled_spi;

#endif // OLED_LIB_VGY12864L_03_H_

