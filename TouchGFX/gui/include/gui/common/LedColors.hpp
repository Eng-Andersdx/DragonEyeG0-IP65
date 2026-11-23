#ifndef LEDCOLORS_HPP
#define LEDCOLORS_HPP

/* LED ring colours for presenter->setAmbientLightRGB(...), 0..255 per colour.
 * Usage: presenter->setAmbientLightRGB(LED_PURPLE);   or any colour: (255, 128, 0)
 * Same colours as the BSP's RGB_xxx (0..100 %) in dragoneye_leds.h, converted to 0..255.
 * The BSP hook (knobSetAmbientLightRGB) converts back to 0..100 %. */
#define LED_RED          255,   0,   0
#define LED_GREEN          0, 255,   0
#define LED_BLUE           0,   0, 255
#define LED_YELLOW       255, 255,   0
#define LED_CYAN           0, 255, 255
#define LED_MAGENTA      255,   0, 255
#define LED_WHITE        255, 255, 255
#define LED_ORANGE       255, 179,   0
#define LED_DARK_ORANGE  255, 102,  26
#define LED_BROWN        255, 153,  51
#define LED_SKY_BLUE      79, 150, 255
#define LED_PURPLE       128,   0, 179
#define LED_DARK_RED     128,   0,   0
#define LED_DARK_GREEN     0, 128,   0
#define LED_DARK_BLUE      0,   0, 128
#define LED_OLIVE        128, 128,   0
#define LED_TEAL           0, 128, 128
#define LED_GREY          64,  64,  64
#define LED_MEDIUM_RED   191,   0,   0
#define LED_MEDIUM_GREEN   0, 191,   0
#define LED_MEDIUM_BLUE    0,   0, 191
#define LED_LIGHT_YELLOW 191, 191,   0
#define LED_LIGHT_CYAN     0, 191, 191
#define LED_LIGHT_MAGENTA 191,  0, 191
#define LED_COFFEE       115,  69,  23
#define LED_OVEN         166,  89,  20
#define LED_OFF            0,   0,   0

#endif // LEDCOLORS_HPP
