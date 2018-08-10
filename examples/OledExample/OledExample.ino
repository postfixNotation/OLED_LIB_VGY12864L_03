#include <OLED_LIB_VGY12864L_03.h>
// #include <image.h> // include this header of you
// have an uin8_t array called "image" for instance
// which contains data of an image

// if you run this sketch 8 pixels or to be more precise
// a display page is supposed to be set
void setup() {
  oled_spi.Init();
  // oled_parallel.Init(); // this is how parallel inteface calls look like
  oled_spi.SetContrastReg(0xFF); // 0x00 - 0xFF OR 0 - 255
  oled_spi.SetPageValue(0, 0, 0xFF);
  // oled_spi.DrawImage(image, sizeof(image) / sizeof(UI8));
}

// It is also recommended to look inside the respective header-file
// since there are more comments that might help you deal with this
// library.

void loop() {
}

// More information is provided in my GitHub repository:
// https://github.com/postfixNotation/OLED_LIB_VGY12864L_03
