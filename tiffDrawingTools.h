#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SHORT 3
#define LONG 4
#define RATIONAL 5

struct screen {   // image data entity
	uint8_t *dat;
	int w, l, datmax;
	char *name;
	uint8_t r, g, b;
	int c;
} typedef screen;

void drawPixel(screen *s, double x, double y);  // paint one pixel
void setPixel(screen *s, int x, int y);
void setPixelWithColor(screen *s, int x, int y, uint8_t r, uint8_t g, uint8_t b);

void setColor(screen *s, int r, int g, int b);   // change the current color of the brush

void drawLine2d(screen *s, int px1, int py1, int px2, int py2);
void drawLine(screen *s, double px1, double py1, double px2, double py2);

uint8_t *init(int wid, int len,screen *s);
screen *sopen(int wid, int len);

void unInit(screen *s);
void sclose(screen *s);
void write_IFD(uint16_t tag, uint16_t field, uint32_t vals, uint32_t offset, FILE *fp);
void writeHeader(FILE *fp, int w, int l);
void writeFile(char *name, screen *s);

