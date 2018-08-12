#include <LiquidCrystal.h>
#include <LedControl.h>

typedef enum {MENU, SNAKE, OVER} states;

states state = MENU;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
LedControl lc=LedControl(16,14,15);

unsigned long prevFoodMillis, prevPressMillis, prevMenuMillis = 0;
unsigned long prevMatrix = 0;
bool startLoc = true;

byte snakeDir = 8;
byte snakeLength = 0;
int snakeInterval = 500;

bool moveState = true;
bool pressState = false;

int dotLocX[64], dotLocY[64];
int moveLocX, moveLocY;
int lastLocX, lastLocY = 8;

int foodLocX, foodLocY;
bool showFood = true;
int foodInterval = 250;

byte customY[] = {
  B01110,
  B01110,
  B01110,
  B10101,
  B11011,
  B11011,
  B11011,
  B11111
};
byte customE[] = {
  B00000,
  B01111,
  B01111,
  B00001,
  B01111,
  B01111,
  B00000,
  B11111
};
byte customS[] = {
  B10000,
  B01111,
  B01111,
  B10001,
  B11110,
  B11110,
  B00001,
  B11111
};
byte customN[] = {
  B01110,
  B01110,
  B00110,
  B01010,
  B01100,
  B01110,
  B01110,
  B11111
};
byte customO[] = {
  B10001,
  B01110,
  B01110,
  B01110,
  B01110,
  B01110,
  B10001,
  B11111
};
byte customA[] = {
   B10001,
  B01110,
  B01110,
  B01110,
  B00000,
  B01110,
  B01110,
  B11111
};
byte customR[] = {
  B00001,
  B01110,
  B01110,
  B00001,
  B01011,
  B01101,
  B01110,
  B11111
};
byte customT[] = {
  B00000,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11011,
  B11111
};

void setup() {

  lcd.createChar(0, customY);
  lcd.createChar(1, customE);
  lcd.createChar(2, customS);
  lcd.createChar(3, customN);
  lcd.createChar(4, customO);
  lcd.createChar(5, customA);
  lcd.createChar(6, customR);
  lcd.createChar(7, customT);
  lcd.home();
  
  randomSeed(analogRead(3));
  pinMode(10, INPUT);
  lcd.begin(16, 2);
  lcd.setCursor(3, 0);
  lcd.print("Snake YES NO");
  lcd.setCursor(5, 1);
  lcd.print("START");

  lc.shutdown(0, false);
  lc.setIntensity(0,5);
  lc.clearDisplay(0);

  foodLocX = random(1, 8);
  foodLocY = random(1, 8);
  
}

void MenuLoop(){
  unsigned long currentMillis = millis();
  if ((currentMillis - prevMenuMillis) >= 500){
    lcd.setCursor(3, 0);
    lcd.print("Snake YES NO");
    lcd.setCursor(5,1);
    if (startLoc == true){
      lcd.print("START");
    } else {
      lcd.write(2);
      lcd.write(7);
      lcd.write(5);
      lcd.write(6);
      lcd.write(7);
    }
    startLoc = !startLoc;
    prevMenuMillis = currentMillis;

    if (digitalRead(10) == HIGH){
      if (pressState == false){
        state = SNAKE;
      }
    } else {
      pressState = false;
    }
  }
  
}

void JoyStickRead(){

  int yVal = analogRead(A0);
  int xVal = analogRead(A1);

  if (pressState == false){
    if (yVal <= 300){
      if (snakeDir != 2){
        snakeDir = 8;
        moveState = true;
        pressState = true;
      }
    } else if (yVal >= 800){
      if (snakeDir != 8){
        snakeDir = 2;
        moveState = true;
        pressState = true;
      }
    }else if (xVal <=300){
      if (snakeDir != 6){
        snakeDir = 4;
        moveState = true;
        pressState = true;
      }
    } else if (xVal >= 800){
      if (snakeDir != 4){
        snakeDir = 6;
        moveState = true;
        pressState = true;
      }
    }
    prevPressMillis = millis();
  } 

  if (xVal >300){
    if (xVal < 800){
      if (yVal >300){
        if (yVal < 800){
          pressState = false;
        }
      }
    }
  }
}

void MovementCalc(){
  switch (snakeDir){
    case 8:
      dotLocY[0]++;
      break;
    case 2:
      dotLocY[0]--;
      break;
    case 4:
      dotLocX[0]--;
      break;
    case 6:
      dotLocX[0]++;
      break;
  }

  if (dotLocX[0] >= 8){
    dotLocX[0] = 0;
    lc.setLed(0, dotLocX[0], 7, 0);
  } else if (dotLocX[0] < 0){
    dotLocX[0] = 7;
  }
  
  if (dotLocY[0] >=8){
    dotLocY[0] = 0;
    lc.setLed(0, 7, dotLocY[0], 0);
  } else if (dotLocY[0] < 0){
    dotLocY[0] = 7;
  }
}

void FoodMove(){
  //int lastX, lastY;
  //lastX = foodLocX; lastY= foodLocY;
  for (int i = 0; i <= snakeLength; i++){
    while ((dotLocX[i] == foodLocX) && (dotLocY[i] == foodLocY)){
      foodLocX = random(0, 8);
      foodLocY = random(0, 8);
      i = 0;
    }
  }
  snakeLength++;
  if (snakeInterval > 160){
    snakeInterval = snakeInterval - 20;
  }
}

void Collision(){
  for (int i = 1 ; i <= snakeLength; i++){
    while ((dotLocX[0] == dotLocX[i]) && (dotLocY[0] == dotLocY[i])){
      state = OVER;
      lcd.clear();
      return;
    }
  }
}

void SnakeLoop(){
  unsigned long currentMillis = millis();
  JoyStickRead();

  if ((currentMillis - prevFoodMillis) >= foodInterval){
    if (showFood == true){
      lc.setLed(0, foodLocX, foodLocY, 1);
      foodInterval = 250;
    } else {
      lc.setLed(0, foodLocX, foodLocY, 0);
      foodInterval = 100;
    }
    prevFoodMillis = currentMillis;
    showFood = !showFood;
  }
      
  if ((currentMillis - prevMatrix) >= snakeInterval){
    MovementCalc();
    Collision();

    if (state == OVER){
     return;
    }

    lc.setLed(0, lastLocX, lastLocY, 0);
    lastLocY = dotLocY[snakeLength];
    lastLocX = dotLocX[snakeLength];
    
    if (snakeLength != 0){
      for (int i = snakeLength; i > 0; i--){ 
        dotLocX[i] = dotLocX[i-1];
        dotLocY[i] = dotLocY[i-1];
        lc.setLed(0, dotLocX[i], dotLocY[i], 1);
      }
    } else {
      lc.setLed(0,dotLocX[0], dotLocY[0], 1);
    }
    prevMatrix = currentMillis;
  } 

  if ((dotLocX[0] == foodLocX) && (dotLocY[0] == foodLocY)){
    FoodMove();
  }

  if ((currentMillis - prevPressMillis) >= snakeInterval){
    pressState = false;
  }
}

void OverLoop(){
  unsigned long currentMillis = millis();
  int xVal = analogRead(A1);
  
  lcd.setCursor(4, 0);
  lcd.print("Play again?");
  lcd.setCursor(4,1);

  if (startLoc == true){
    lcd.write(byte(0));
    lcd.write(1);
    lcd.write(2);
    lcd.print("  NO");
  } else {
    lcd.print("YES  ");
    lcd.write(3);
    lcd.write(4);
  }

  if (xVal >= 800){
    startLoc = false;
  } else if (xVal <= 300){
    startLoc = true;
  }

  if (digitalRead(10) == HIGH){
    if(startLoc == true){
      state = SNAKE;
    } else {
      state = MENU;
      pressState = HIGH;
      lcd.clear();
    }
    snakeLength = 0;
    snakeInterval = 500; 
    dotLocX[0] = 0; dotLocY[0] = 0;
    snakeDir = 8;
    lc.clearDisplay(0);

    foodLocX = random(1, 8);
    foodLocY = random(1, 8);
  }
}

void loop() {
  if (state == MENU){
    MenuLoop();
  } else if (state == SNAKE){
    SnakeLoop();    
  } else if (state == OVER){
    OverLoop();
  }
  
}
