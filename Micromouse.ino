#include <stdbool.h>
//#define MAZESIZE 8
#include <stdlib.h>

///////////////////////////////////////////////////////////////////////////////

#define RH_ENCODER_A 3 
#define RH_ENCODER_B 12
#define LH_ENCODER_A 2
#define LH_ENCODER_B 8

#define RH_Motor1 11 //
#define RH_Motor2 9  //
#define LH_Motor1 5  //
#define LH_Motor2 6  //

#define RightSensor_EN 13
#define LeftSensor_EN 7
#define CenterSensor_EN 4

#define RightSensor_RD A1
#define LeftSensor_RD A2
#define CenterSensor_RD A0

#define DigitalIR A3
///////////////////////////////////////////////////////////////////////////////

volatile unsigned long leftCount = 0;
volatile unsigned long rightCount = 0;
int vel=40;  // global speed
int tempRight=0;
int tempLeft=0;
int unit=45;  // how many tips per square
int leftRead=0;
int rightRead=0;
int centerRead=0;
int vmax=80;
int asdf=0;
int contCiclos=0;
int leftWall, rightWall, frontWall;

//////////////////////////////////////////////////////////////////////////////
int MAZESIZE = 8;
double leftLimit, rightLimit, centerLimit;

int initialX = 7;
int initialY = 0;

int currentX = initialX;
int currentY = initialY;

boolean continueCode = true; 

char currentDir = 'n';

struct Cell {
  int walls[4];
  int value;
  int visited;
  int xCoord;
  int yCoord;
};

Cell cells[8][8];
Cell neighbors[4];

struct Tuple {
  Cell TCell;
  char smallest;
};


void setup() { 
  leftWall = rightWall = 1;
  frontWall = 0;
  
  pinMode(RH_Motor1,OUTPUT);
  pinMode(RH_Motor2,OUTPUT);
  pinMode(LH_Motor1,OUTPUT);
  pinMode(LH_Motor2,OUTPUT);
  pinMode(LH_ENCODER_A, INPUT);
  pinMode(LH_ENCODER_B, INPUT);
  pinMode(RH_ENCODER_A, INPUT);
  pinMode(RH_ENCODER_B, INPUT);
  pinMode(RightSensor_EN,OUTPUT);
  pinMode(LeftSensor_EN,OUTPUT);
  pinMode(CenterSensor_EN,OUTPUT);
  pinMode(DigitalIR,INPUT);

  analogReference(INTERNAL);
  
  // initialize hardware interrupts
  attachInterrupt(0, leftEncoderEvent, CHANGE);
  attachInterrupt(1, rightEncoderEvent, CHANGE);
  
  Serial.begin(9600);

  digitalWrite(RightSensor_EN,HIGH);
  digitalWrite(LeftSensor_EN,HIGH);
  digitalWrite(CenterSensor_EN,HIGH);

  
  startMaze();
  //SETUP LISTO. Se puede iniciar el movimiento del micromouse
}

void loop() {

  readSensors();
  
  if(cells[currentX][currentY].value != 0 && continueCode){
    continueCode = false; 
    continueCode = explore(cells, leftWall, rightWall, frontWall);
  }

}

// encoder event for the interrupt call
void leftEncoderEvent() {
  if (digitalRead(LH_ENCODER_A) == HIGH) {
    if (digitalRead(LH_ENCODER_B) == LOW) {
      leftCount--;
    } else {
      leftCount++;
    }
  } else {
    if (digitalRead(LH_ENCODER_B) == LOW) {
      leftCount++;
    } else {
      leftCount--;
    }
  }
}
 
// encoder event for the interrupt call
void rightEncoderEvent() {
  if (digitalRead(RH_ENCODER_A) == HIGH) {
    if (digitalRead(RH_ENCODER_B) == LOW) {
      rightCount++;
    } else {
      rightCount--;
    }
  } else {
    if (digitalRead(RH_ENCODER_B) == LOW) {
      rightCount--;
    } else {
      rightCount++;
    }
  }
}

void goForward() {
 
    rightCount=0;
    leftCount=0;
    
    tempRight=unit;
    tempLeft=unit;

    for(int q=0;q<115;q++){
      if(digitalRead(DigitalIR)==0){
        analogWrite(RH_Motor2,vel-3);
        analogWrite(LH_Motor1,vel);
      }else{
        analogWrite(RH_Motor1,0);
        analogWrite(LH_Motor1,vel);
        analogWrite(RH_Motor2,vel-8);
        analogWrite(LH_Motor2,0);
      }
      delay(5);
    }
   
    STOP();
    delay(1000);
}

void goBackward() {
  analogWrite(RH_Motor1,vel);
  analogWrite(RH_Motor2,0);
  analogWrite(LH_Motor1,0);
  analogWrite(LH_Motor2,vel);

  delay(200);
  STOP();
}

void STOP() {
  analogWrite(RH_Motor1,0);
  analogWrite(RH_Motor2,0);
  analogWrite(LH_Motor1,0);
  analogWrite(LH_Motor2,0);
}

int readFrontSensor(){
  analogReference(DEFAULT);
  digitalWrite(CenterSensor_EN,HIGH);
  delay(1);
  centerRead=analogRead(CenterSensor_RD);
  
  digitalWrite(CenterSensor_EN,LOW);
  return(centerRead);
}

int readLeftSensor(){
  analogReference(INTERNAL);
  leftRead=analogRead(LeftSensor_RD);
  return(leftRead);
}

int readRightSensor(){
  analogReference(INTERNAL);
  rightRead=analogRead(RightSensor_RD);
  return(rightRead);
}

void turnRight(){
  rightCount=0;
  leftCount=0;

  analogWrite(RH_Motor1,vel*0.86);
  analogWrite(LH_Motor1,vel);
  analogWrite(RH_Motor2,0);
  analogWrite(LH_Motor2,0);
  
  delay(225);
  STOP();
  delay(500);
  goForward();
  
}

void turnLeft(){
  rightCount=0;
  leftCount=0;

  analogWrite(RH_Motor1,0);
  analogWrite(LH_Motor1,0);
  analogWrite(RH_Motor2,vel*0.83);
  analogWrite(LH_Motor2,vel);
  
  delay(235);
  STOP();
  delay(500);
  goForward();
  
}

void turnBack(){
  analogWrite(RH_Motor1,0);
  analogWrite(LH_Motor1,0);
  analogWrite(RH_Motor2,vel*0.83);
  analogWrite(LH_Motor2,vel);
  
  delay(440);
  STOP();
  delay(500);
  goForward();
}

void readSensors(){
  if(readRightSensor()>600){
    rightWall=0;
  }
  else{
    rightWall=1;
  }
  
  if(readLeftSensor()>700){
    leftWall=0;
  }
  else{
    leftWall=1;
  }
  
  if(readFrontSensor()>600){
    frontWall=1;
  }
  else{
    frontWall=0;
  }
}


void startMaze() {
  int i;
  int j;
  for (i = 0; i < MAZESIZE; i++) {
    for (j = 0; j < MAZESIZE; j++) {
      cells[i][j].value = startValue(i, j);
      if (i == 0) { // North
        cells[i][j].walls[0] = 1;
      }
      if (i == MAZESIZE - 1) { // South
        cells[i][j].walls[1] = 1;
      }
      if (j == MAZESIZE - 1) { // East
        cells[i][j].walls[2] = 1;
      }
      if (j == 0) { // West
        cells[i][j].walls[3] = 1;
      }
      cells[i][j].xCoord = i;
      cells[i][j].yCoord = j;
    }
  }
}

int startValue(int x, int y) {
  int centerX = 4;
  int centerY = 4;
  if (x <= centerX - 1) {
    centerX -= 1;
  }
  if (y <= centerY - 1) {
    centerY -= 1;
  }
  int xDiff = abs(centerX - x);
  int yDiff = abs(centerY - y);
  int res = xDiff + yDiff;
  return res;
}

bool explore(Cell cells[8][8], int leftWall, int rightWall, int frontWall) {
  switch (currentDir) {
    case 'n':
      // CURRENT CELL 
      cells[currentX][currentY].walls[0] = frontWall;  // front north
      cells[currentX][currentY].walls[2] = rightWall;  // right east
      cells[currentX][currentY].walls[3] = leftWall;  // left west 
      
      // NEIGHBOR
      cells[currentX - 1][currentY].walls[1] = frontWall; // front neighbor update south wall 
      cells[currentX][currentY + 1].walls[3] = rightWall;// righ neighbor update west wall 
      cells[currentX][currentY - 1].walls[2] = leftWall;// left neighbor update east wall 
      break;
      
    case 's':
      cells[currentX][currentY].walls[1] = frontWall;  // front south
      cells[currentX][currentY].walls[3] = rightWall;  // right west
      cells[currentX][currentY].walls[2] = leftWall;  // left east
      
      // NEIGHBOR
      cells[currentX + 1][currentY].walls[0] = frontWall;// front neighbor update north wall 
      cells[currentX][currentY - 1].walls[2] = rightWall;// righ neighbor update left wall 
      cells[currentX][currentY + 1].walls[3] = leftWall;// left neighbor update right wall 
      break;
    case 'e':
      // CURRENT CELL 
      cells[currentX][currentY].walls[2] = frontWall;  // front east
      cells[currentX][currentY].walls[1] = rightWall;  // right south
      cells[currentX][currentY].walls[0] = leftWall;  // left north
      
      // NEIGHBOR
      cells[currentX][currentY + 1].walls[3] = frontWall;// front neighbor update west wall 
      cells[currentX + 1][currentY].walls[0] = rightWall;// righ neighbor update left wall 
      cells[currentX - 1][currentY].walls[1] = leftWall;// left neighbor update right wall 
      
      break;
    case 'w':
      // CURRENT CELL 
      cells[currentX][currentY].walls[3] = frontWall;  // front west
      cells[currentX][currentY].walls[0] = rightWall;  // right north
      cells[currentX][currentY].walls[1] = leftWall;  // left south 
      
      // NEIGHBOR
      cells[currentX][currentY - 1].walls[2] = frontWall; // front neighbor update east wall 
      cells[currentX - 1][currentY].walls[1] = rightWall;// righ neighbor update south wall 
      cells[currentX + 1][currentY].walls[0] = leftWall;// left neighbor update north wall 
      break;
  }
    if (leftWall+ rightWall+ frontWall > 0) {
      updateValues();
    }
    
    char newDir = getNextMove();
    if (currentDir != newDir) {
      turnMM(currentDir, newDir);
    }
    currentDir = newDir;
    
    moveMM();
  return true;
}


void updateValues() {
  //Stack<Cell> stack  = new Stack<Cell>();
  Cell stack[100];
  int counter = 0;
  // push current cell
  // stack.Push(cells[currentX][currentY]);
  counter = counter + 1;
  stack[counter] = cells[currentX][currentY];
  while (counter > 0) {
    // for top element of stack
    // Cell checkCell = stack.Pop();
    Cell checkCell = stack[counter];
    counter = counter - 1;

    // get expected value -> smallestneighbor+1
    Cell smallestCell = chooseSmallestNeighbor(checkCell.xCoord, checkCell.yCoord).TCell;
    int expectedValue = smallestCell.value + 1;

    // validate actualUpdateCell value
    if (checkCell.value != expectedValue && checkCell.value != 0) {
      checkCell.value = expectedValue;
      cells[checkCell.xCoord][checkCell.yCoord].value=expectedValue;
      int neighborsCount = getOpenNeighbors(checkCell.xCoord, checkCell.yCoord);
      for (int i = 0; i <= neighborsCount; i++) {
        counter = counter + 1;
        stack[counter] = neighbors[i];
        // stack.Push(neighbors[i]);
      }
    }
  }
}


char getNextMove() {
  // from neighbors (no walls between them) choose the smallest one
  char newDir = chooseSmallestNeighbor(currentX, currentY).smallest;
  return newDir;
}

void turnMM(char currentDir, char newDir) {
  switch (currentDir) {
    case 'n':
      switch (newDir) {
        case 'e':
          turnRight();
          break;
        case 'w':
          turnLeft();
          break;
        case 's':
          turnBack();
          break;
      }
      break;

    case 'e':
      switch (newDir) {
        case 'n':
          turnLeft();
          break;

        case 'w':
          turnBack();
          break;
        case 's':
          turnRight();
          break;
      }

      break;

    case 'w':
      switch (newDir) {
        case 'n':
          turnRight();
          break;
        case 'e':
          turnBack();
          break;

        case 's':
          turnLeft();
          break;
      }

      break;
    case 's':
      switch (newDir) {
        case 'n':
          turnBack();
          break;
        case 'e':
          turnRight();
          break;

        case 'w':
          turnLeft();
          break;
      }
      break;
  }
}

void moveMM(){
    switch(currentDir){
      case 'n':
        currentX = currentX - 1; 
        break;
      case 's':
        currentX = currentX + 1;
        break; 
      case 'e':
        currentY = currentY + 1;
        break;
      case 'w': 
        currentY = currentY - 1; 
        break; 
    }
    goForward();
}

Tuple chooseSmallestNeighbor(int currentX, int currentY) {
  char smallest = currentDir;
  Cell smallestCell = cells[currentX][currentY];
  int min = 1000;
  int walls[4];
  walls[0] = cells[currentX][currentY].walls[0];
  walls[1] = cells[currentX][currentY].walls[1];
  walls[2] = cells[currentX][currentY].walls[2];
  walls[3] = cells[currentX][currentY].walls[3];

  switch (currentDir) {
    case 'n':
      if (walls[0] == 0) {
        min = cells[currentX - 1][currentY].value;
        smallestCell = cells[currentX - 1][currentY];
      }
      break;
    case 's':
      if (walls[1] == 0) {
        min = cells[currentX + 1][currentY].value;
        smallestCell = cells[currentX + 1][currentY];
      }
      break;
    case 'e':
      if (walls[2] == 0) {
        min = cells[currentX][currentY + 1].value;
        smallestCell = cells[currentX][currentY + 1];
      }
      break;
    case 'w':
      if (walls[3] == 0) {
        min = cells[currentX][currentY - 1].value;
        smallestCell = cells[currentX][currentY - 1];
      }
      break;
  }

      // Compare otherneighbors values
      // compare north
      if (walls[0] == 0 && min > cells[currentX - 1][currentY].value ) {
        smallest = 'n';
        smallestCell = cells[currentX - 1][currentY];
        min = cells[currentX - 1][currentY].value;
      }
      // compare south
      if (walls[1] == 0 && min > cells[currentX + 1][currentY].value ) {
        smallest = 's';
        smallestCell = cells[currentX + 1][currentY];
        min = cells[currentX + 1][currentY].value;
      }
      // compare east
      if (walls[2] == 0 && min > cells[currentX][currentY + 1].value ) {
        smallest = 'e';
        smallestCell = cells[currentX][currentY + 1];
        min = cells[currentX][currentY + 1].value;
      }
      // compare west
      if (walls[3] == 0 && min > cells[currentX][currentY - 1].value) {
        smallest = 'w';
        smallestCell = cells[currentX][currentY - 1];
        min = cells[currentX][currentY - 1].value;
      }
      
  Tuple res;
  res.TCell = smallestCell;
  res.smallest = smallest;
  return res;
}

int getOpenNeighbors(int currentX, int currentY){
    int neighborsCount = 0;
    // North 
    if(currentX-1 > 0){
      neighbors[neighborsCount] = cells[currentX-1][currentY];
      neighborsCount = neighborsCount + 1;
    }
    // East 
    if(currentY+1 < MAZESIZE){
      neighbors[neighborsCount] = cells[currentX][currentY+1];
      neighborsCount = neighborsCount + 1;
    }
    // West
    if(currentY-1 >0){
      neighbors[neighborsCount] = cells[currentX][currentY-1];
      neighborsCount = neighborsCount + 1;
    }

    // South
    if(currentX+1 < MAZESIZE){
      neighbors[neighborsCount] = cells[currentX+1][currentY];
      neighborsCount = neighborsCount + 1;
    }
    return neighborsCount; 
}
