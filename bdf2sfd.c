#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if 0
/* pretend to be size 16 (12 at 96 dpi) instead of 15 (11.25 at 96 dpi) */
/* to play better with windows */
#define UNIT (2048/16)
#define ASCENT 1664
#define DESCENT 384
#define LINEGAP 128
#else
#define UNIT (2048/fontsize)
#define ASCENT ascent*UNIT
#define DESCENT descent*UNIT
#define LINEGAP 128
#endif

char line[256];
char *fontname;
char *facename;
char *familyname;
char *charname;
int fontsize;
int fbbox;
int ascent;
int descent;
int fbbx[4];
int encoding;
int swidth;
int dwidth;
int bbx[4];
char bitmap[256][256];

int charcount = 0;

#define get1(buf,x) ((buf[x >> 3] >> ( 7 - (x & 7) ) ) & 1 )

void
emithead(void)
{
	puts("SplineFontDB: 3.0");
	puts("Weight: Medium");
	puts("Version: 001.000");
	puts("ItalicAngle: 0");
	puts("UnderlinePosition: -100");
	puts("UnderlineWidth: 50");
	puts("LayerCount: 2");
	puts("Layer: 0 0 \"Back\"  1");
	puts("Layer: 1 0 \"Fore\"  0");
	puts("OS2Version: 0");
	puts("OS2_WeightWidthSlopeOnly: 0");
	puts("OS2_UseTypoMetrics: 1");
	printf("LineGap: %d\n", LINEGAP);
	printf("OS2TypoAscent: %d\n", ASCENT);
	puts("OS2TypoAOffset: 0");
	printf("OS2TypoDescent: %d\n", -DESCENT);
	puts("OS2TypoDOffset: 0");
	printf("OS2TypoLinegap: %d\n", LINEGAP);
	printf("OS2WinAscent: %d\n", ASCENT);
	puts("OS2WinAOffset: 0");
	printf("OS2WinDescent: %d\n", -DESCENT);
	puts("OS2WinDOffset: 0");
	printf("HheadAscent: %d\n", ASCENT);
	puts("HheadAOffset: 0");
	printf("HheadDescent: %d\n", -DESCENT);
	puts("HheadDOffset: 0");
	puts("OS2Vendor: 'PfEd'");
//	puts("DEI: 91125");
	puts("GaspTable: 1 65535 0");
	puts("Encoding: ISO8859-1");
	puts("UnicodeInterp: none");
	puts("NameList: Adobe Glyph List");
	puts("AntiAlias: 0");
	puts("FitToEm: 1");
	puts("WinInfo: 0 29 15");
	puts("OnlyBitmaps: 1");
	puts("DisplaySize: -24");

	if (facename[0] == '"')
		facename++;
	if (facename[strlen(facename)-1] == '"')
		facename[strlen(facename)-1] = 0;
	if (familyname[0] == '"')
		familyname++;
	if (familyname[strlen(familyname)-1] == '"')
		familyname[strlen(familyname)-1] = 0;

	printf("FontName: %s\n", fontname);
	printf("FullName: %s\n", facename);
	printf("FamilyName: %s\n", familyname);
	printf("Ascent: %d\n", ASCENT);
	printf("Descent: %d\n", DESCENT);

	printf("BeginChars: 256 256\n");
}

int
unhex(int c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return 0;
}

void
unhexrow(char *raw, char *hex)
{
	int a, b;
	while (hex[0] && hex[1])
	{
		a = hex[0];
		b = hex[1];
		hex += 2;
		*raw++ = unhex(a) * 16 + unhex(b);
	}
	*raw = 0;
}

void
readbitmap(void)
{
	int y;
	for (y = 0; y < bbx[1]; y++)
	{
		gets(line);
		unhexrow(bitmap[y], line);
	}
}

void
emitchar(void)
{
	int x, y, c, row, col;
	int p = 0;

	printf("StartChar: %s\n", charname);
	printf("Encoding: %d %d %d\n", encoding, encoding, ++charcount);
	printf("Width: %d\n", dwidth * UNIT);
	printf("Fore\n");
	printf("SplineSet\n");

	for (row=0; row<bbx[1]; row++)
	{
		for (col=0; col<bbx[0]; col++)
		{
			c = get1(bitmap[row], col);
			if (c)
			{
				x = (bbx[2] + col) * UNIT;
				y = (bbx[3] + bbx[1] - row - 1) * UNIT;
				printf("%d %d m 1\n", x, y+UNIT);
				printf("%d %d l 1\n", x+UNIT, y+UNIT);
				printf("%d %d l 1\n", x+UNIT, y);
				printf("%d %d l 1\n", x, y);
				printf("%d %d l 1\n", x, y+UNIT);
			}
			// putchar(c ? '#' : '_');
		}
		// putchar('\n');
	}

	printf("EndSplineSet\n");
	printf("EndChar\n");
}

int
main(int argc, char **argv)
{
	while(gets(line))
	{
		if (strstr(line, "FONT ") == line)
			fontname = strdup(line + 5);
		if (strstr(line, "FACE_NAME ") == line)
			facename = strdup(line + 10);
		if (strstr(line, "FAMILY_NAME ") == line)
			familyname = strdup(line + 12);
		if (strstr(line, "PIXEL_SIZE ") == line)
			sscanf(line, "%*s %d", &fontsize);
		if (strstr(line, "FONTBOUNDINGBOX") == line)
			sscanf(line, "%*s %d %d %d %d", fbbx+0, fbbx+1, fbbx+2, fbbx+3);
		if (strstr(line, "FONT_ASCENT") == line)
			sscanf(line, "%*s %d", &ascent);
		if (strstr(line, "FONT_DESCENT") == line)
			sscanf(line, "%*s %d", &descent);
		if (strstr(line, "ENDPROPERTIES") == line)
			emithead();

		if (strstr(line, "STARTCHAR") == line)
			charname = strdup(line + 10);
		if (strstr(line, "ENCODING") == line)
			encoding = atoi(line + 9);
		if (strstr(line, "SWIDTH") == line)
			swidth = atoi(line + 7);
		if (strstr(line, "DWIDTH") == line)
			dwidth = atoi(line + 7);
		if (strstr(line, "BBX") == line)
			sscanf(line + 4, "%d %d %d %d", bbx+0, bbx+1, bbx+2, bbx+3);
		if (strstr(line, "BITMAP") == line)
			readbitmap();
		if (strstr(line, "ENDCHAR") == line)
			emitchar();
	}
}

