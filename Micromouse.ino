#include <stdbool.h>
#define MAZESIZE 8
#include <stdlib.h>

#define SENSOR_I_EN 2  // izquierda 
#define SENSOR_D_EN 3  // derecha 
#define SENSOR_C_EN 4  // centro 

//  read
#define SENSOR_I_RD A0  // izquierda 
#define SENSOR_D_RD A1 // derecha 
#define SENSOR_C_RD A2  // centro 

#define ENCODER_I_A 5  // izquierda a 
#define ENCODER_I_B 6  // izquierda b 
#define ENCODER_D_A 7  // derecha a 
#define ENCODER_D_B 8  // derecha b 

double leftRead, rightRead, centerRead;
double leftLimit, rightLimit, centerLimit;
int leftWall, rightWall, frontWall;

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
  startMaze();
  //SETUP LISTO. Se puede iniciar el movimiento del micromouse 
  leftWall = rightWall = frontWall = 0;
  Serial.begin(9600);  // baudios por segundo
  // determinar pines de entrada y salida
  //ENTRADA
  // ENCODER
  pinMode( ENCODER_I_A, INPUT);
  pinMode( ENCODER_I_B, INPUT);
  pinMode( ENCODER_D_A, INPUT);
  pinMode( ENCODER_D_B, INPUT);

  //SALIDA
  //PAREDES
  pinMode(SENSOR_I_EN, OUTPUT);
  pinMode(SENSOR_D_EN, OUTPUT);
  pinMode(SENSOR_C_EN, OUTPUT);
}

void loop() {
  // left
  leftRead = analogRead(SENSOR_I_RD);
  delay(5);

  digitalWrite(SENSOR_I_EN, HIGH);
  delay(53);

  leftRead = analogRead(SENSOR_I_RD) - leftRead;

  digitalWrite(SENSOR_I_EN, LOW);
  delay(50);

  // right
  rightRead = analogRead(SENSOR_D_RD);
  delay(5);

  digitalWrite(SENSOR_D_EN, HIGH);
  delay(53);

  rightRead = analogRead(SENSOR_D_RD) - rightRead;

  digitalWrite(SENSOR_I_EN, LOW);
  delay(50);

  // center
  centerRead = analogRead(SENSOR_C_RD);
  delay(5);

  digitalWrite(SENSOR_C_EN, HIGH);
  delay(53);

  leftRead = analogRead(SENSOR_C_RD) - centerRead;

  digitalWrite(SENSOR_C_EN, LOW);
  delay(50);

  if (leftRead < leftLimit) {
    leftWall = 1;
  }else{
    leftWall = 0;
  }

  if (rightRead < rightLimit) {
    rightWall = 1;
  }else{
    rightWall = 0;
  }

  if (centerRead < centerLimit) {
    frontWall = 1;
  }else{
    frontWall = 0;
  }
  
  if(cells[currentX][currentY].value != 0 && continueCode){
    continueCode = false; 
    continueCode = explore(cells, leftWall, rightWall, frontWall);
  }else if(cells[currentX][currentY].value == 0){
    // Done 
  }
  

}

void startMaze() {
  int i;
  int j;
  for (i = 0; i < MAZESIZE; i++) {
    for (j = 0; j < MAZESIZE; j++) {
      cells[i][j].value = startValue(i, j);
      int * resWalls = initialWalls(i, j); 
      cells[i][j].walls[0] = resWalls[0];
      cells[i][j].walls[1] = resWalls[1];
      cells[i][j].walls[2] = resWalls[2];
      cells[i][j].walls[3] = resWalls[3];
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

int * initialWalls(int x, int y) {
  static int init[4];
  if (x == 0) { // North
    init[0] = 1;
  }
  if (x == MAZESIZE - 1) { // South
    init[1] = 1;
  }
  if (y == MAZESIZE - 1) { // East
    init[2] = 1;
  }
  if (y == 0) { // West
    init[3] = 1;
  }

  return init;

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
          //turnRight();
          break;
        case 'w':
          //turnLeft();
          break;
        case 's':
          //turnBack();
          break;
      }
      break;

    case 'e':
      switch (newDir) {
        case 'n':
          //turnLeft();
          break;

        case 'w':
          //turnBack();
          break;
        case 's':
          //turnRight();
          break;
      }

      break;

    case 'w':
      switch (newDir) {
        case 'n':
          //turnRight();
          break;
        case 'e':
          //turnBack();
          break;

        case 's':
          //turnLeft();
          break;
      }

      break;
    case 's':
      switch (newDir) {
        case 'n':
          //turnBack();
          break;
        case 'e':
          //turnRight();
          break;

        case 'w':
          //turnLeft();
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
