/*
The MIT License (MIT)

Copyright (c) 2015 Martin Hudoba

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST); //Fast I2C / TWI

#define joyPinX 0
#define joyPinY 1
#define joyPinB 5
#define joyPinX2 2
#define joyPinY2 3
#define joyPinB2 4

uint8_t selX = 2;
uint8_t selY = 2;
uint16_t scoreX, scoreO;

char player = 'X';

char field[3][3]; //field[X][Y]
bool fieldx[3][3]; //fieldx[X][Y]
const uint8_t coords[3] = {6, 27, 48};

void drawCross(uint8_t x, uint8_t y) {
  u8g.drawLine(x, y, x + 9, y + 9); //top left - bottom right slash
  u8g.drawLine(x + 9, y, x, y + 9); //down left - top right slash
}

void drawCircle(uint8_t x, uint8_t y) {
  u8g.drawEllipse(x + 4, y + 5, 5, 5);
}

void playerSwitch() {
  if (player == 'O'){player = 'X';}
  else if (player == 'X'){player = 'O';}
}

void draw(void) {
  u8g.firstPage();
  do {
    // GAME FIELD //////////////////////////
    u8g.drawFrame(0, 0, 64, 64); //frame
    u8g.drawLine(0, 21, 63, 21); //horizontal line
    u8g.drawLine(0, 42, 63, 42); //horizontal line
    u8g.drawLine(21, 0, 21, 63); //vertical line
    u8g.drawLine(42, 0, 42, 63); //vertical line

    //draws the game field elements (X or O)
    for (uint8_t i = 0; i <= 2; i++) {
      for (uint8_t j = 0; j <= 2; j++) {
        if (field[i][j] == 'O'){drawCircle(coords[i], coords[j]);}
        else if (field[i][j] == 'X'){drawCross(coords[i], coords[j]);}
      }
    }
    // GAME FIELD //////////////////////////
    
    // SELECTION BOX ///////////////////////
    uint8_t selBoxX, selBoxY;
    if (selX == 1){selBoxX = 1;}
    else if (selX == 2){selBoxX = 22;}
    else if (selX == 3){selBoxX = 43;}
    
    if (selY == 1){selBoxY = 1;}
    else if (selY == 2){selBoxY = 22;}
    else if (selY == 3){selBoxY = 43;}
    
    u8g.drawFrame(selBoxX, selBoxY, 20, 20);
    u8g.drawFrame(selBoxX+1, selBoxY+1, 18, 18);
    // SELECTION BOX ///////////////////////
    
    u8g.setFont(u8g_font_helvB08);
    u8g.drawStr(81, 10, "SKORE");
    u8g.drawStr(75, 24, "X:");
    u8g.setPrintPos(88, 24);
    u8g.print(scoreX);
    u8g.drawStr(75, 35, "O:");
    u8g.setPrintPos(88, 35);
    u8g.print(scoreO);
    
    u8g.setFont(u8g_font_5x8);
    u8g.drawStr(75, 46, "Na rade:");
    if (player == 'X'){drawCross(93, 50);}
    else if (player == 'O'){drawCircle(93 , 50);}
  } while(u8g.nextPage());
}

void drawWin(char player) {
  //handles the score table
  if (player == 'X') {
    scoreX++;
  } else if (player == 'O') {
    scoreO++;
  }
  playerSwitch(); //switches the players
  draw(); //draws the board with players switched
  delay(2000); //delay for players to check which 3 squares led to the win - winning squares highlighting can be added

  //clears the game field arrays
  for (uint8_t i = 0; i <= 2; i++) {
    for (uint8_t j = 0; j <= 2; j++) {
       field[i][j] = ' ';
       fieldx[i][j] = false;
    }
  }

  //draws the win screen  
  u8g.firstPage();
 do {
    u8g.setFont(u8g_font_9x18B);      
    if (player == 'X') {
      u8g.drawStr(14, 22, "Vyhral hrac");
      u8g.drawLine(x, y, x + 20, y + 20); //top left - bottom right slash
      u8g.drawLine(x + 20, y, x, y + 20); //down left - top right slash
    } else if (player == 'O') {
      u8g.drawStr(14, 22, "Vyhral hrac");
      u8g.drawEllipse(x + 10, y + 12, 12, 12);
    } else if (player == 'R') {
      u8g.setFont(u8g_font_courB18);
      u8g.drawStr(17, 41, "REMIZA");
    }
  } while(u8g.nextPage());

  selX = 2; //centers the selection box on X axis
  selY = 2; //centers the selection box on Y axis
  
  delay(1500); //show the above for 1500ms
  draw(); //draw the new board
  delay(500); //wait for 500ms to avoid accidental input (player pressing down the button while and after the win screen is showing)
}

void setup() {
  pinMode(joyPinB, INPUT_PULLUP);
  pinMode(joyPinB2, INPUT_PULLUP);
}

void loop() {
  uint16_t joyX, joyY, btnState;
  if (player == 'X') {  
    btnState = digitalRead(joyPinB);
    joyX = analogRead(joyPinX);
    joyY = analogRead(joyPinY);
  } else if (player == 'O') {
    btnState = digitalRead(joyPinB2);
    joyX = analogRead(joyPinX2);
    joyY = analogRead(joyPinY2);
  }
  if (joyX > 823){selX--;}
  else if (joyX < 200){selX++;}
  if (joyY < 200){selY--;}
  else if (joyY > 823){selY++;}
  if (selX > 3){selX = 3;}
  else if (selX < 1){selX = 1;}
  if (selY > 3){selY = 3;}
  else if (selY < 1){selY = 1;}

  //assigns the game field elements their value (X or O) when the joystick button is pressed
  if (btnState == LOW) {
    for (uint8_t i = 0; i <= 2; i++) {
      for (uint8_t j = 0; j <= 2; j++) {
        if ((selX == i+1) && (selY == j+1) && (fieldx[i][j] == false)) {
          if (player == 'X') {
            field[i][j] = 'X';
            fieldx[i][j] = true;
            playerSwitch();
          } else if (player == 'O') {
            field[i][j] = 'O';
            fieldx[i][j] = true;
            playerSwitch();
          }
        }
      }
    }      
  }

  //checks the game field for winning combinations
  if ((field[0][0] == 'X') && (field[1][0] == 'X') && (field[2][0] == 'X')){drawWin('X');}
  else if ((field[0][1] == 'X') && (field[1][1] == 'X') && (field[2][1] == 'X')){drawWin('X');}
  else if ((field[0][2] == 'X') && (field[1][2] == 'X') && (field[2][2] == 'X')){drawWin('X');}
  else if ((field[0][0] == 'X') && (field[0][1] == 'X') && (field[0][2] == 'X')){drawWin('X');}
  else if ((field[1][0] == 'X') && (field[1][1] == 'X') && (field[1][2] == 'X')){drawWin('X');}
  else if ((field[2][0] == 'X') && (field[2][1] == 'X') && (field[2][2] == 'X')){drawWin('X');}
  else if ((field[0][0] == 'X') && (field[1][1] == 'X') && (field[2][2] == 'X')){drawWin('X');}
  else if ((field[2][0] == 'X') && (field[1][1] == 'X') && (field[0][2] == 'X')){drawWin('X');}
  else if ((field[0][0] == 'O') && (field[1][0] == 'O') && (field[2][0] == 'O')){drawWin('O');}
  else if ((field[0][1] == 'O') && (field[1][1] == 'O') && (field[2][1] == 'O')){drawWin('O');}
  else if ((field[0][2] == 'O') && (field[1][2] == 'O') && (field[2][2] == 'O')){drawWin('O');}
  else if ((field[0][0] == 'O') && (field[0][1] == 'O') && (field[0][2] == 'O')){drawWin('O');}
  else if ((field[1][0] == 'O') && (field[1][1] == 'O') && (field[1][2] == 'O')){drawWin('O');}
  else if ((field[2][0] == 'O') && (field[2][1] == 'O') && (field[2][2] == 'O')){drawWin('O');}
  else if ((field[0][0] == 'O') && (field[1][1] == 'O') && (field[2][2] == 'O')){drawWin('O');}
  else if ((field[2][0] == 'O') && (field[1][1] == 'O') && (field[0][2] == 'O')){drawWin('O');}
  else if ((fieldx[0][0] == true) && (fieldx[0][1] == true) && (fieldx[0][2] == true) && (fieldx[1][0] == true) && (fieldx[1][1] == true) && (fieldx[1][2] == true) && (fieldx[2][0] == true) && (fieldx[2][1] == true) && (fieldx[2][2] == true)){drawWin('R');}
  
  draw();
}
