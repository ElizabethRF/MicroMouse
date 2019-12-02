#include <iostream>
#include <stdbool.h>
#define MAZESIZE 8
#include <stdlib.h>

double leftRead, rightRead, centerRead;
double leftLimit, rightLimit, centerLimit;
int leftWall, rightWall, frontWall;

int initialX = 7;
int initialY = 0;

int currentX = initialX;
int currentY = initialY;

bool continueCode = true;

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

void moveMM() {
	switch (currentDir) {
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

	switch (currentDir) {
	case 'n':
		if (cells[currentX][currentY].walls[0] == 0) {
			min = cells[currentX - 1][currentY].value;
			smallestCell = cells[currentX - 1][currentY];
		}
		break;
	case 's':
		if (cells[currentX][currentY].walls[1] == 0) {
			min = cells[currentX + 1][currentY].value;
			smallestCell = cells[currentX + 1][currentY];
		}
		break;
	case 'e':
		if (cells[currentX][currentY].walls[2] == 0) {
			min = cells[currentX][currentY + 1].value;
			smallestCell = cells[currentX][currentY + 1];
		}
		break;
	case 'w':
		if (cells[currentX][currentY].walls[3] == 0) {
			min = cells[currentX][currentY - 1].value;
			smallestCell = cells[currentX][currentY - 1];
		}
		break;
	}
	// Compare otherneighbors values
	// compare north
	if (cells[currentX][currentY].walls[0] == 0 && min > cells[currentX - 1][currentY].value) {
		smallest = 'n';
		smallestCell = cells[currentX - 1][currentY];
		min = cells[currentX - 1][currentY].value;
	}
	// compare south
	if (cells[currentX][currentY].walls[1] == 0 && min > cells[currentX + 1][currentY].value) {
		smallest = 's';
		smallestCell = cells[currentX + 1][currentY];
		min = cells[currentX + 1][currentY].value;
	}
	// compare east
	if (cells[currentX][currentY].walls[2] == 0 && min > cells[currentX][currentY + 1].value) {
		smallest = 'e';
		smallestCell = cells[currentX][currentY + 1];
		min = cells[currentX][currentY + 1].value;
	}
	// compare west
	if (cells[currentX][currentY].walls[3] == 0 && min > cells[currentX][currentY - 1].value) {
		smallest = 'w';
		smallestCell = cells[currentX][currentY - 1];
		min = cells[currentX][currentY - 1].value;
	}

	Tuple res;
	res.TCell = smallestCell;
	res.smallest = smallest;
	std::cout << "Smallest: " << smallest << "\n";
	std::cout << "Min: " << min << "\n";
	return res;
}

char getNextMove() {
	// from neighbors (no walls between them) choose the smallest one
	char newDir = chooseSmallestNeighbor(currentX, currentY).smallest;
	return newDir;
}


int getOpenNeighbors(int currentX, int currentY) {
	int neighborsCount = 0;
	// North 
	if (currentX - 1 > 0) {
		neighbors[neighborsCount] = cells[currentX - 1][currentY];
		neighborsCount = neighborsCount + 1;
	}
	// East 
	if (currentY + 1 < MAZESIZE) {
		neighbors[neighborsCount] = cells[currentX][currentY + 1];
		neighborsCount = neighborsCount + 1;
	}
	// West
	if (currentY - 1 > 0) {
		neighbors[neighborsCount] = cells[currentX][currentY - 1];
		neighborsCount = neighborsCount + 1;
	}

	// South
	if (currentX + 1 < MAZESIZE) {
		neighbors[neighborsCount] = cells[currentX + 1][currentY];
		neighborsCount = neighborsCount + 1;
	}
	return neighborsCount;
}

void updateValues() {
	//Stack<Cell> stack  = new Stack<Cell>();
	Cell stack[100];
	int counter = 0;
	// push current cell
	// stack.Push(cells[currentX][currentY]);
	counter = counter + 1;
	std::cout << "count " << counter << "\n";
	stack[counter] = cells[currentX][currentY];
	while (counter > 0) {
		// for top element of stack
		// Cell checkCell = stack.Pop();
		Cell checkCell = stack[counter];
		counter = counter - 1;
		std::cout << "count " << counter << "\n";
		// get expected value -> smallestneighbor+1
		Cell smallestCell = chooseSmallestNeighbor(checkCell.xCoord, checkCell.yCoord).TCell;
		int expectedValue = smallestCell.value + 1;
		std::cout << "CheckCellValue " << checkCell.value << "\n";
		std::cout << "Expected Value " << expectedValue << "\n";
		// validate actualUpdateCell value
		if (checkCell.value != expectedValue && checkCell.value != 0) {
			checkCell.value = expectedValue;
			int neighborsCount = getOpenNeighbors(checkCell.xCoord, checkCell.yCoord);
			std::cout << "Ncount " << neighborsCount << "\n";
			for (int i = 0; i < neighborsCount; i++) {
				counter = counter + 1;
				stack[counter] = neighbors[i];
				// stack.Push(neighbors[i]);
			}
		}
	}
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
	if (leftWall + rightWall + frontWall > 0) {
		updateValues();
	}

	char newDir = getNextMove();
	if (currentDir != newDir) {
		turnMM(currentDir, newDir);
	}
	currentDir = newDir;

	std::cout << " Before move Current Dir: " << currentDir << "\n";
	std::cout << "Before move Current X: " << currentX << "\n";
	std::cout << "Before move Current Y: " << currentY << "\n";

	moveMM();

	std::cout << " After move Current Dir: " << currentDir << "\n";
	std::cout << "After move Current X: " << currentX << "\n";
	std::cout << "After move Current Y: " << currentY << "\n";
	return true;
}

void startMaze() {
	int i;
	int j;
	for (i = 0; i < MAZESIZE; i++) {
		for (j = 0; j < MAZESIZE; j++) {
			cells[i][j].value = startValue(i, j);
			//int * resWalls = initialWalls(i, j); 
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


void printMazeWithValues(int mmXPos, int mmYPos) {
	for (int i = 0; i < MAZESIZE; i++) {
		for (int j = 0; j < MAZESIZE; j++) {
			// North Wall 
			if (cells[i][j].walls[0]) {
				std::cout << "+---";
			}
			else { // No north wall 
				std::cout << "   ";
			}
		}
		// End of line 
		std::cout << "+\n";
		for (int j = 0; j < MAZESIZE; j++) { // west wall 
			if (cells[i][j].walls[3]) {
				std::cout << "|";
			}
			else { // no wall 
				std::cout << " ";
			}
			// print cell value 
			if (i == mmXPos && j == mmYPos)
			{
				std::cout << "*" << cells[i][j].value;
			}
			else {
				std::cout << " " << cells[i][j].value;
			}

		}
		// print last east wall 
		std::cout << "|\n";

	}
	// print last south wall 
	for (int j = 0; j < MAZESIZE; j++) {
		std::cout << "+---";
	}
	std::cout << "+\n";
}

void printMaze() {
	for (int i = 0; i < MAZESIZE; i++) {
		for (int j = 0; j < MAZESIZE; j++) {
			// North Wall 
			if (cells[i][j].walls[0]) {
				std::cout << "+---";
			}
			else { // No north wall 
				std::cout << "   ";
			}
		}
		// End of line 
		std::cout << "+";
		for (int j = 0; j < MAZESIZE; j++) { // west wall 
			if (cells[i][j].walls[3]) {
				std::cout << "|";
			}
			else { // no wall 
				std::cout << " ";
			}
			// print spaces 
			std::cout << "   ";

		}
		// print last east wall
		std::cout << "|\n";

	}
	// print last south wall 
	for (int j = 0; j < MAZESIZE; j++) {
		std::cout << "+---";
	}
	std::cout << "+\n";
	// temporal 
	/*for(int i = 0 ; i < mazeSize; i++){
	 for(int j = 0; j < mazeSize; j++){
	   Console.WriteLine("("+i+","+j+") N:"+ maze[i,j].getNorthWall()+" S:"+maze[i,j].getSouthWall()+" E:"+ maze[i,j].getEastWall()+" W:"+maze[i,j].getWestWall());
	 }
	}*/
}

int main() {
	startMaze();
	//SETUP LISTO. Se puede iniciar el movimiento del micromouse 
	leftWall = rightWall = frontWall = 0;
	int cont = 0;
	while (cells[currentX][currentY].value != 0 && continueCode) {
		cont++;
		rightWall = 1;
		printMazeWithValues(currentX, currentY);
		std::cout << "Iteracion: " << cont << "\n";
		if (cont == 4) {
			rightWall = 0;
		}
		if (cont == 5) {
			leftWall = 1;
		}
		if (cont == 7) {
			rightWall = 0;
		}
		continueCode = false;
		continueCode = explore(cells, leftWall, rightWall, frontWall);
	}
	printMazeWithValues(currentX, currentY);
	std::cout << "Fin\n";
	return 0;
}