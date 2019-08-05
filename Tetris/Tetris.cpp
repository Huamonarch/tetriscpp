#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <thread>

using namespace::std;
wstring tetrominoes[7];

int fieldWidth = 12;
int fieldHeight = 18;
unsigned char *pointerField = nullptr;
int nScreenWidth = 80;			// Console Screen Size X (columns)
int nScreenHeight = 30;			// Console Screen Size Y (rows)

int rotate(int px, int py, int r) {
	// Rotation based in single array lookup following this:
	//0   1  2  3
	//4   5  6  7
	//8   9 10 11
	//12 13 14 15

	int pi = 0;

	switch (r % 4)
	{
	case 0:
		pi = py * 4 + px;
		break;
	case 1:
		pi = py + 12 - (px * 4);
		break;
	case 2:
		pi = 15 - (py * 4) - px;
		break;
	case 3:
		pi = 3 - py + (px * 4);
		break;
	default:
		break;
	}
	
	return pi;
	
}

void initTetrominos() {
	tetrominoes[0].append(L"..x.");
	tetrominoes[0].append(L"..x.");
	tetrominoes[0].append(L"..x.");
	tetrominoes[0].append(L"..x.");

	tetrominoes[1].append(L"..x.");
	tetrominoes[1].append(L".xx.");
	tetrominoes[1].append(L"..x.");
	tetrominoes[1].append(L"....");

	tetrominoes[2].append(L"....");
	tetrominoes[2].append(L".xx.");
	tetrominoes[2].append(L".xx.");
	tetrominoes[2].append(L"....");

	tetrominoes[3].append(L".x..");
	tetrominoes[3].append(L".xx.");
	tetrominoes[3].append(L"..x.");
	tetrominoes[3].append(L"..-.");

	tetrominoes[4].append(L"..x.");
	tetrominoes[4].append(L".xx.");
	tetrominoes[4].append(L".x..");
	tetrominoes[4].append(L"....");

	tetrominoes[5].append(L".x..");
	tetrominoes[5].append(L".x..");
	tetrominoes[5].append(L".xx.");
	tetrominoes[5].append(L"....");

	tetrominoes[6].append(L"..x.");
	tetrominoes[6].append(L"..x.");
	tetrominoes[6].append(L".xx.");
	tetrominoes[6].append(L"....");
}

bool checkCollision(int tetromino, int rotation, int posx, int posy) {

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			int tetrominoIndex = rotate(x, y, rotation);

			int fi = (posy + y) * fieldWidth + (posx + x);

				// Check that test is in bounds.Note out of bounds does
				// not necessarily mean a fail, as the long vertical piece
				// can have cells that lie outside the boundary, so we'll
				// just ignore them
				if (posx + x >= 0 && posx + x < fieldWidth)
				{
					if (posy + y >= 0 && posy + y < fieldHeight)
					{
						// In Bounds so do collision check
						if (tetrominoes[tetromino][tetrominoIndex] != L'.' && pointerField[fi] != 0)
							return false; // fail on first hit
					}
				}
		}
	}

	return true;
}

void drawTetrominoe(int nCurrentPiece, int nCurrentRotation, wchar_t *screen, int nCurrentY, int nCurrentX) {
	//Looks up at the tetrominoes wstring array and renders X instances as an alphanumeric character
		//hence we have the 65 here
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++)
			if (tetrominoes[nCurrentPiece][rotate(px, py, nCurrentRotation)] != L'.')
				screen[(nCurrentY + py + 2)*nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;
}

int main()
{
	initTetrominos();
	pointerField = new unsigned char[fieldWidth*fieldHeight]; // Create play field buffer
	for (int x = 0; x < fieldWidth; x++) // Board Boundary
		for (int y = 0; y < fieldHeight; y++)
			pointerField[y*fieldWidth+ x] = (x == 0 || x == fieldWidth - 1 || y == fieldHeight- 1) ? 9 : 0;

	wchar_t *screen = new wchar_t[nScreenWidth*nScreenHeight];
	for (int i = 0; i < nScreenWidth*nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	bool gameOver = false;
	// logic
	int nCurrentPiece = 0;
	int nCurrentRotation = 0;
	int nCurrentX = fieldWidth / 2;
	int nCurrentY = 0;
	bool bKey[4];

	while(!gameOver){
		// Time
		this_thread::sleep_for(50ms);

		// Input
		for (int k = 0; k < 4; k++)								// R   L   DZ
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

		nCurrentX += (bKey[0] && checkCollision(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
		nCurrentX -= (bKey[1] && checkCollision(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
		nCurrentY += (bKey[2] && checkCollision(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;


		// Draw Field
		for (int x = 0; x < fieldWidth; x++)
			for (int y = 0; y < fieldHeight; y++)
				screen[(y + 2)*nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pointerField[y*fieldWidth + x]];

		// Output
		drawTetrominoe(nCurrentPiece, nCurrentRotation, screen, nCurrentY, nCurrentX);
		

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth* nScreenHeight, { 0,0 }, &dwBytesWritten);
	}


	return 0;
}