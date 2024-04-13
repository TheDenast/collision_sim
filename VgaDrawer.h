#ifndef VGADRAWER_H
#define VGADRAWER_H
#include "DE1SoCfpga.h"

using namespace std;
	
class VgaDrawer : public DE1SoCfpga{
	private:

	public:
		void PaintPixel(int posx, int posy, int color); 					   // Paints a specific pixel
		void PaintLine(int posx, int posy, int direct, int length, int color); // Paints a straight line, uses PaintPixel
		void Paint2PL(int x1, int x2, int y1, int y2, int color);			   // Paints a line given two points
		void PaintShape(int posx, int posy, int shape, int size, int color);   // Paints a shape of choice using PaintLine
		void ClearScreen();													   // Clears the screen completely
		int  CheckPixel(int posx, int posy);
		VgaDrawer();
		~VgaDrawer();

};
#endif