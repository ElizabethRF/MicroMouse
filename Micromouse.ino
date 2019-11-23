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

double leftRead, rightRead,centerRead; 
double leftLimit, rightLimit, centerLimit; 
boolean leftWall, rightWall, frontWall; 

int initialX = 7; 
int initialY = 0; 

struct Cell{
  int walls[4]; 
  int value; 
  int visited; 
  int xCoord; 
  int yCoord; 
};

Cell cells[8][8];


void setup() {
  startMaze(); 
  
  leftWall = rightWall = frontWall = false; 
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
    
    digitalWrite(SENSOR_I_EN,HIGH);
    delay(53);
    
    leftRead = analogRead(SENSOR_I_RD) - leftRead;
    
    digitalWrite(SENSOR_I_EN,LOW);
    delay(50);
    
  // right
    rightRead = analogRead(SENSOR_D_RD);
    delay(5);
    
    digitalWrite(SENSOR_D_EN,HIGH);
    delay(53);
    
    rightRead = analogRead(SENSOR_D_RD) - rightRead;
    
    digitalWrite(SENSOR_I_EN,LOW);
    delay(50);
    
  // center 
    centerRead = analogRead(SENSOR_C_RD);
    delay(5);
    
    digitalWrite(SENSOR_C_EN,HIGH);
    delay(53);
    
    leftRead = analogRead(SENSOR_C_RD) - centerRead;
    
    digitalWrite(SENSOR_C_EN,LOW);
    delay(50);
    
    if(leftRead < leftLimit){
      leftWall = true; 
    }
    
    if(rightRead < rightLimit){
      rightWall = true; 
    }
    
    if(centerRead < centerLimit){
      frontWall = true; 
    }
    
    explore(cells,leftWall, rightWall, frontWall);
    
  }

  void startMaze(){
    int i; 
    int j; 
    for(i = 0; i < MAZESIZE; i++){
      for(j = 0; i < MAZESIZE; j++){
        cells[i][j].value = startValue(i,j); 
        cells[i][j].walls = initialWalls(i,j);
        cells[i][j].xCoord = i;
        cells[i][j].yCoord = j; 
      }
    }
  }

  int startValue(int x, int y){
    int centerX = 4; 
    int centerY = 4; 
    if(x <= centerX-1){
      centerX -= 1; 
    }
    if(y <= centerY-1){
      centerY -= 1; 
    }
    int xDiff = abs(centerX - x); 
    int yDiff = abs(centerY - y); 
    int res = xDiff + yDiff; 
    return res; 
  }
  
  int initialWalls(int x, int y){
    int init[4];
    if(x == 0){ // North
        init[0] = 1; 
      } 
    if(x == MAZESIZE-1){ // South 
        init[1] = 1; 
      } 
      if(y == MAZESIZE-1){ // East 
        init[2] = 1; 
      } 
      if(y == 0){ // West 
        init[3] = 1; 
      } 
    
    return init; 

  }

  bool explore(Cell cells, bool leftWall, bool rightWall, bool frontWall){
    while(getCell(currentX,currentY) != 0 ){
      switch(currentDir){
        case 'n':
            
         break; 
        case 's':
        
         break; 
        case 'e':
        
         break; 
        case 'w':
        
         break; 
      }
      if(leftWall && rightWall && frontWall){
        updateValues(); 
      }
      char newDir = getNextMove(); 
      if(currentDir != newDir){
        turnMM(currentDir,newDir);
      }
      move(); 
    }
    return true;
  }
  
  
  void addFrontWall(){
    // add current and neighbor cell if not exist 
    switch(currentDir){
      case 'n':
        addWallNorthNeighbor();
        break; 
      case 's':
        addWallSouthNeighbor();
        break; 
      case 'e':
        addWallEastNeighbor(); 
        break; 
      case 'w':
        addWallWestNeighbor(); 
        break; 
    }
  }

  void addLeftWall(){
    switch(currentDir){
      case 'n':
        addWallWestNeighbor(); 
        break; 
      case 's':
        addWallEastNeighbor(); 
        break; 
      case 'e':
        addWallNorthNeighbor();
        break; 
      case 'w':
        addWallSouthNeighbor();
        break; 
    }

  }

  void addRightWall(){
    switch(currentDir){
      case 'n':
        addWallEastNeighbor(); 
        break; 
      case 's':
        addWallWestNeighbor(); 
        break; 
      case 'e':
        addWallSouthNeighbor();
        break; 
      case 'w':
        addWallNorthNeighbor();
        break; 
    }

  }

  void addWallNorthNeighbor(){
    if(!mmMaze.getCell(currentX,currentY).getNorthWall()){
        mmMaze.getCell(currentX,currentY).setNorthWall(true); 
        mmMaze.getCell(currentX-1,currentY).setSouthWall(true);
    }

  }
  void addWallSouthNeighbor(){
    if(!mmMaze.getCell(currentX,currentY).getSouthWall()){
      mmMaze.getCell(currentX,currentY).setSouthWall(true); 
      mmMaze.getCell(currentX+1,currentY).setNorthWall(true);
    }

  }
  void addWallEastNeighbor(){
    if(!mmMaze.getCell(currentX,currentY).getEastWall()){
      mmMaze.getCell(currentX,currentY).setEastWall(true); 
      mmMaze.getCell(currentX,currentY+1).setWestWall(true);
    }
  }
  
  void addWallWestNeighbor(){
    if(!mmMaze.getCell(currentX,currentY).getWestWall()){
      mmMaze.getCell(currentX,currentY).setWestWall(true); 
      mmMaze.getCell(currentX,currentY-1).setEastWall(true);
    }
  }

  void updateValues(){
    Stack<Cell> stack  = new Stack<Cell>(); 
    // push current cell 
    stack.Push(getCell(currentX,currentY));
    while(stack.Count > 0){
      // for top element of stack 
      Cell checkCell = stack.Pop();

      // get expected value -> smallestneighbor+1
      Cell smallestCell = chooseSmallestNeighbor(checkCell.getXCoord(),checkCell.getYCoord()).Item2; 
      int expectedValue = smallestCell.getValue() +1; 

      // validate actualUpdateCell value 
      if(checkCell.getValue() != expectedValue && checkCell.getValue() != 0){
        checkCell.setValue(expectedValue);
        List<Cell> neighbors = getOpenNeighbors(checkCell.getXCoord(),checkCell.getYCoord());
        for(int i = 0; i < neighbors.Count; i++){
          stack.Push(neighbors[i]);
        }
      }
    }
  }


  char getNextMove(){
    // from neighbors (no walls between them) choose the smallest one 
    char newDir = chooseSmallestNeighbor(currentX, currentY).Item1;
    
    return newDir; 
  }
  
  void turnMM(char currentDir,char newDir){
    switch(currentDir){
      case 'n':
        switch(newDir){
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
      switch(newDir){
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
      switch(newDir){
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
      switch(newDir){
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
  
  Tuple<char,Cell> chooseSmallestNeighbor(int currentX, int currentY){
    char smallest = currentDir;  
    Cell smallestCell = getCell(currentX,currentY);
    int min = 1000; 
    bool[] walls = getCell(currentX,currentY).walls;
    
    switch(currentDir){
      case 'n':
        if(!walls[0]){
          min = getCell(currentX-1,currentY);
          smallestCell = getCell(currentX-1,currentY).value; 
        }
        break;
      case 's':
        if(!walls[1]){
          min = getCell(currentX+1,currentY).value;
          smallestCell = mmMaze.getCell(currentX+1,currentY); 
        }
        break; 
      case 'e':
        if(!walls[2]){
          min = getCell(currentX,currentY+1).value;
          smallestCell = getCell(currentX,currentY+1); 
        }
        break;
      case 'w': 
        if(!walls[3]){
         min = getCell(currentX,currentY-1).value;
         smallestCell = getCell(currentX,currentY-1); 
        }
        break; 
        
      
    // Compare otherneighbors values 
    // compare north
    if(!walls[0] && min > getCell(currentX-1,currentY).value ){
      smallest = 'n'; 
      smallestCell = getCell(currentX-1,currentY); 
      min = getCell(currentX-1,currentY).getValue(); 
    }
    // compare south 
    if(!walls[1] && min > getCell(currentX+1,currentY).value ){
      smallest = 's'; 
      smallestCell = getCell(currentX+1,currentY); 
      min = getCell(currentX+1,currentY).value; 
    } 
    // compare east 
    if(!walls[2] && min > getCell(currentX,currentY+1).value ){
      smallest = 'e'; 
      smallestCell = getCell(currentX,currentY+1); 
      min = getCell(currentX,currentY+1).value;
    }
    // compare west
    if(!walls[3] && min > getCell(currentX,currentY-1).value){
      smallest = 'w'; 
      smallestCell = getCell(currentX,currentY-1); 
      min = getCell(currentX,currentY-1).value; 
    }

    Tuple<char,Cell> res = new Tuple<char,Cell>(smallest,smallestCell);
    return res; 
    }
    
    Cell getCell(int x, int y){
    return cells[x][y]; 
  }
}
