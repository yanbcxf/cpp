#include "fxlib.h"
#include <stdlib.h>
#include <stdio.h>


int ipow(int base, int exp)
{
	int result = 1;
	while (exp)
	{
	if (exp & 1)
		result *= base;
		exp >>= 1;
		base *= base;
	}

	if(exp = 0) result = 1;	
	return result;
}

int AddIn_main(int isAppli, unsigned short OptionNum)
{
    unsigned int key;
	char buffer[20];
	short X = 0, Y = 0, xcounter, ycounter;
	int binary = 0;
	short picture[16][16] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	};

	short paintPicture = 1;
	short changePixel = 0;
	short reverseColor = 0;
	short instantDraw = 0;

	locate(1,3);
	Print((const unsigned char*)"SpritePainter [EXE]");
	GetKey(&key);
	key = 0;
	
	while(0==0){
	while(paintPicture){
		Bdisp_AllClr_VRAM();
		switch(key){
			case KEY_CTRL_UP:
				if(Y > 0) Y--;
				break;
			case KEY_CTRL_DOWN:
				if(Y < 15) Y++;
				break;
			case KEY_CTRL_LEFT:
				if(X > 0) X--;
				break;
			case KEY_CTRL_RIGHT:
				if(X < 15) X++;
				break;
			case KEY_CTRL_EXE:
				goto ending;
				break;
			case KEY_CTRL_F1:
				changePixel = 1;
				break;
			case KEY_CTRL_F2:
				instantDraw = !instantDraw;
				changePixel = 1;
				break;
		}
		if(changePixel) picture[X][Y] = !picture[X][Y];
		if(!instantDraw) changePixel = 0;

		for(xcounter = 0; xcounter < 64; xcounter++){
			for(ycounter = 0; ycounter < 64; ycounter++){
				Bdisp_SetPoint_VRAM(32+xcounter, ycounter, picture[xcounter/4][ycounter/4]);		
			}
		}

		if(picture[X][Y] == 0) reverseColor = 1;
		if(picture[X][Y] == 1) reverseColor = 0;

		Bdisp_SetPoint_VRAM(32+(X*4), (Y*4), reverseColor);
		Bdisp_SetPoint_VRAM(33+(X*4), (Y*4), reverseColor);
		Bdisp_SetPoint_VRAM(34+(X*4), (Y*4), reverseColor);
		Bdisp_SetPoint_VRAM(35+(X*4), (Y*4), reverseColor);

		Bdisp_SetPoint_VRAM(32+(X*4), (Y*4)+1, reverseColor);
		Bdisp_SetPoint_VRAM(32+(X*4), (Y*4)+2, reverseColor);
		Bdisp_SetPoint_VRAM(35+(X*4), (Y*4)+1, reverseColor);
		Bdisp_SetPoint_VRAM(35+(X*4), (Y*4)+2, reverseColor);

		Bdisp_SetPoint_VRAM(32+(X*4), (Y*4)+3, reverseColor);
		Bdisp_SetPoint_VRAM(33+(X*4), (Y*4)+3, reverseColor);
		Bdisp_SetPoint_VRAM(34+(X*4), (Y*4)+3, reverseColor);
		Bdisp_SetPoint_VRAM(35+(X*4), (Y*4)+3, reverseColor);

		Bdisp_DrawLineVRAM(31, 0, 31, 63);
		Bdisp_DrawLineVRAM(96, 0, 96, 63);

		locate(18,1);
		sprintf(buffer, "X%d ",X);
		Print((const unsigned char*)buffer);
		locate(18,2);
		sprintf(buffer, "Y%d ",Y);
		Print((const unsigned char*)buffer);
		
		Bdisp_PutDisp_DD();
		GetKey(&key);	
    }

	ending:

	for(ycounter = 0; ycounter < 16; ycounter++){
		for(xcounter = 0; xcounter < 16; xcounter++){
			if(picture[xcounter][ycounter] == 1) binary += ipow(2,15-xcounter);
		}
		Bdisp_AllClr_DDVRAM();
		locate(1,1);
		sprintf(buffer, "0x%04X[%d]", binary, ycounter);
		Print((const unsigned char*)buffer);
		binary = 0;
		GetKey(&key);
	}

	paintPicture = 1;
	key = 0;
	}
	

    return 1;
}


#pragma section _BR_Size
unsigned long BR_Size;
#pragma section


#pragma section _TOP


int InitializeSystem(int isAppli, unsigned short OptionNum)
{
    return INIT_ADDIN_APPLICATION(isAppli, OptionNum);
}

#pragma section