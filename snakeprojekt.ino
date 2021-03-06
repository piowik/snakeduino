#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define DISPLAY_WIDTH 84        //Lcd 84x48
#define DISPLAY_HEIGHT 40       // 48-8 reserved for text

#define NOTE_E5  659

const byte snakeLogo[] PROGMEM = {
        B00000000,B00000000,B00000000,B00111111,B11100000,B00000000,B00000000,B00000000,B00111111,B11110000,B00000000,
        B00000000,B00000000,B00000011,B11111111,B11111111,B00000000,B00000000,B00000011,B11111111,B11111100,B00000000,
        B00000000,B00000000,B00001111,B10000000,B00000001,B11000000,B00000000,B00001111,B10000000,B00001111,B11100000,
        B00000000,B00000000,B00001100,B00000000,B00000000,B11100000,B00000000,B00111100,B00000000,B00001111,B11000000,
        B00000000,B00000000,B00011000,B00000000,B00000000,B01110000,B00000000,B01111000,B00011111,B11111000,B00000000,
        B00000000,B00000000,B00011000,B00000000,B00000000,B01110000,B00000000,B01111000,B00011111,B11111000,B00000000,
        B00000000,B00000000,B00011000,B00000111,B11000000,B01110000,B00000000,B01111000,B00011111,B11111000,B00000000,
        B00000000,B00000000,B00110000,B01111111,B11110000,B00011100,B00000000,B01110000,B01111111,B11100000,B00000000,
        B00000000,B00000000,B00110000,B11111000,B00011110,B00011100,B00000000,B11000000,B11100000,B00000000,B00000000,
        B00000000,B00000000,B00100000,B11000000,B00011110,B00011100,B00000000,B11000000,B11000000,B00000000,B00000000,
        B00000000,B00000000,B00100000,B11000000,B00011110,B00011100,B00000000,B11000000,B11000000,B00000000,B00000000,
        B00000000,B00000000,B00100000,B11000000,B00011110,B00011100,B00000000,B11000000,B11000000,B00000000,B00000000,
        B00000000,B00000000,B00100000,B11000000,B00011110,B00011100,B00000000,B11000000,B11000000,B00000000,B00000000,
        B00000000,B00000000,B00100000,B11000000,B00011110,B00011100,B00000000,B11000000,B11000000,B00000000,B00000000,
        B00000000,B00000000,B00100000,B11000000,B00011111,B00001110,B00000000,B11000000,B11000000,B00000000,B00000000,
        B00011110,B00000001,B11100011,B11000000,B00001111,B00001110,B00000011,B11000011,B10000000,B00000000,B00000000,
        B00011110,B00000001,B11100011,B11000000,B00001111,B00001110,B00000011,B11000011,B10000000,B00000000,B00000000,
        B01110001,B11111111,B11000011,B10000000,B00001111,B00000011,B11111111,B10000011,B10000000,B00000000,B00000000,
        B11100000,B11111110,B00000111,B00000000,B00000001,B10000000,B01111100,B00000111,B00000000,B00000000,B00000000,
        B01110000,B00000000,B00001100,B00000000,B00000000,B11100000,B00000000,B00001100,B00000000,B00000000,B00000000,
        B01110000,B00000000,B00001100,B00000000,B00000000,B11100000,B00000000,B00001100,B00000000,B00000000,B00000000,
        B01111000,B00000000,B00111100,B00000000,B00000000,B01110000,B00000000,B01111000,B00000000,B00000000,B00000000,
        B00001111,B11111111,B11110000,B00000000,B00000000,B00011111,B11111111,B11110000,B00000000,B00000000,B00000000,
        B00001111,B11111111,B11110000,B00000000,B00000000,B00011111,B11111111,B11110000,B00000000,B00000000,B00000000,
        B00001111,B11111111,B11110000,B00000000,B00000000,B00011111,B11111111,B11110000,B00000000,B00000000,B00000000,
        B00000001,B11111111,B11000000,B00000000,B00000000,B00000001,B11111111,B00000000,B00000000,B00000000,B00000000,
};

// pin config
Adafruit_PCD8544 display = Adafruit_PCD8544(4, 3, 2, 1, 0);  // SCLK, DIN, DC, CS, RST
const int buzzer = 5;
// joystick
const int SW_pin = 7; // digital pin
const int X_pin = 0; // analog pin
const int Y_pin = 1; // analog pin

const int threshold = 100; // joystick threshold

//game config
const int thickness = 5;
const int startLength = 4;
const unsigned long startGameSpeed = 330;
const int maxGameSpeed = 80;
const unsigned long gameSpeedIncrease = 10;

const int mapWidth = DISPLAY_WIDTH / thickness;
const int mapHeight = DISPLAY_HEIGHT / thickness;
const int maxLength = mapWidth * mapHeight;

bool hasStarted = false;

int snakeDirection = 1; // 0 - up, 1 - right, - 2 down, 3 - left
int lastDirection = 0;
int x[maxLength], y[maxLength]; // holds snake positions
int snakeLength;
int foodX, foodY;
unsigned long gameSpeed = startGameSpeed;
int score;

void initialise() {
    score = 0;
    gameSpeed = startGameSpeed;
    snakeLength = startLength;
    snakeDirection = 1;
    foodX = random(0, mapWidth);
    foodY = random(0, mapHeight);

    for (byte i = 0; i <= snakeLength; i++)      // init snake
    {
        x[i] = startLength + 1 - i;
        y[i] = 3;
    }
}

void startGame() {
    hasStarted = false;
    display.clearDisplay();
    initialise();
    hasStarted = true;
    printScore();
    redraw();
}

void printScore() {
    display.setTextSize(1);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(1, 40);
    display.print("Score: ");
    display.print(score);
}

void setup() {
  
    pinMode(SW_pin, INPUT);
    digitalWrite(SW_pin, HIGH);
    display.begin();
    display.setContrast(45);
    display.clearDisplay();
    display.drawBitmap(0, 0, snakeLogo, 84, 26, BLACK);
    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.setCursor(10, 28);
    display.print("SNEJK");
    display.display();
    delay(3000);
    display.clearDisplay();
    display.setCursor(10, 4);
    display.print("SNEJK");
    display.setTextSize(1);
    display.setCursor(0,20);
    display.print(">Press button<");
    display.display();
    while(digitalRead(SW_pin) == 1) {
      delay(10);
    }
    
    tone(buzzer, NOTE_E5, 150);
    startGame();
}

void loop() {
    movesnake();
}

void gameOver() {
    display.clearDisplay();
    display.setTextColor(BLACK);
    display.setTextSize(1);
    display.setCursor(14, 8);
    display.print("Game Over");
    display.setCursor(14, 18);
    display.print("Score: ");
    display.print(score);
    display.setCursor(0, 32);
    display.print(">Press button<");
    display.display();
    //delay(3000);
    while(digitalRead(SW_pin) == 1) {
      delay(10);
    }
    
    tone(buzzer, NOTE_E5, 150);
    startGame();
}

void movesnake()  //Ruchy
{
    if (!hasStarted) return;
    readControls();
    if (millis() % gameSpeed == 0) {

        int nextX = 0;
        int nextY = 0;
        switch (snakeDirection) {
            case 0: // up
                nextX = x[0];
                nextY = y[0] - 1;
                break;
            case 1: // right
                nextX = x[0] + 1;
                nextY = y[0];
                break;
            case 2: // down
                nextX = x[0];
                nextY = y[0] + 1;
                break;
            case 3: // left
                nextX = x[0] - 1;
                nextY = y[0];
                break;
        }
        lastDirection = snakeDirection;
        
        if (checkCollision()) {
            gameOver();
            return;
        }
        
        checkFood();

        if (nextX <= 0) { nextX = mapWidth + nextX; }
        if (nextX >= mapWidth) { nextX = nextX - mapWidth; }
        if (nextY <= 0) { nextY = mapHeight + nextY; }
        if (nextY >= mapHeight) { nextY = nextY - mapHeight; }

        for (byte i = 0; i <= snakeLength; i++) {
            int oldX = x[i];
            int oldY = y[i];
            x[i] = nextX;
            y[i] = nextY;
            nextX = oldX;
            nextY = oldY;
        }
        redraw();
    }
}

boolean checkCollision() {
    for (byte i = 1; i < snakeLength; i++) {
        if (x[i] == x[0] && y[i] == y[0])
            return true;
    }
    return false;
}

void checkFood() {
    if (x[0] == foodX && y[0] == foodY) {
        score++;
        snakeLength++;
        tone(buzzer, NOTE_E5, 50);
        delay(50);
         
        printScore();
        if (gameSpeed >= maxGameSpeed) { gameSpeed -= gameSpeedIncrease; }
        display.fillRoundRect(foodX * thickness, foodY * thickness, thickness, thickness, 3, WHITE); // remove food
        display.display();
        foodX = random(0, mapWidth);
        foodY = random(0, mapHeight);
    }
}

void readControls() {
  int posY = analogRead(Y_pin);
  int posX = analogRead(X_pin);
  if (posY > 1023-threshold and lastDirection != 2) // up
    snakeDirection = 0;
  else if (posY < threshold and lastDirection != 0) // down
    snakeDirection = 2;
  else if (posX > 1023-threshold and lastDirection != 3) // right
    snakeDirection = 1;
  else if (posX < threshold and lastDirection != 1) // left
    snakeDirection = 3;
}

void redraw() {
    //display.clearDisplay();     // no need to clear display every tick
    display.fillRoundRect(foodX * thickness, foodY * thickness, thickness, thickness, 3, BLACK);
    for (byte i = 0; i < snakeLength; i++) {
        display.drawRoundRect(x[i] * thickness, y[i] * thickness, thickness, thickness, 1, BLACK);
    }
    display.drawRoundRect(x[snakeLength] * thickness, y[snakeLength] * thickness, thickness, thickness,
                          1, WHITE);   //remove excess tail
    display.display();
}
