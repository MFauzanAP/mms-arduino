//  Imports
#include <cppQueue.h>
#include <ArduinoQueue.h>
#include <Robojax_L298N_DC_motor.h>
#include <Dictionary.h>

//  Declare structs
typedef struct COORDINATE {
  int x;
  int y;
};

//  Declare variables
byte grid[12][12];
int floodGrid[12][12];
cppQueue prevCrossroads(sizeof(COORDINATE));
COORDINATE coords = { 0, 0 };
COORDINATE startCoords = { 0, 0 };
int dir = 0;
int undiscovered = 144;
bool explored = false;

//  Function used to move the robot forward
void moveForward(int n = 1) {
  
}

//  Function used to make the robot face a specific direction
void faceDir(int dir) {
  
}

//  Function used to check for a wall in front
bool wallFront() {
  
}

//  Function used to check for a wall on the left
bool wallLeft() {
  
}

//  Function used to check for a wall on the right
bool wallRight() {
  
}

//  Function used to pathfind to a specific target
void moveToLeastValue() {

  //  Keep looping until robot is at target
  while (floodGrid[coords.y][coords.x] != 0) {

    //  Get current cell
    byte cell = grid[coords.y][coords.x];
  
    //  Get adjacent coords
    COORDINATE coordN = { coords.x, coords.y - 1 };
    COORDINATE coordE = { coords.x + 1, coords.y };
    COORDINATE coordS = { coords.x, coords.y + 1 };
    COORDINATE coordW = { coords.x - 1, coords.y };
  
    //  Get the cell with the least value
    int min = 144;
    int dir = 0;
    if (withinBounds(coordN) && cell & B1000 == B0000 && floodGrid[coordN.y][coordN.x] < min) { min = floodGrid[coordN.y][coordN.x]; dir = 0; }
    if (withinBounds(coordE) && cell & B0100 == B0000 && floodGrid[coordE.y][coordE.x] < min) { min = floodGrid[coordE.y][coordE.x]; dir = 3; }
    if (withinBounds(coordS) && cell & B0010 == B0000 && floodGrid[coordS.y][coordS.x] < min) { min = floodGrid[coordS.y][coordS.x]; dir = 2; }
    if (withinBounds(coordW) && cell & B0001 == B0000 && floodGrid[coordW.y][coordW.x] < min) { min = floodGrid[coordW.y][coordW.x]; dir = 1; }
  
    //  Rotate and move robot
    faceDir(dir);
    moveForward();

  }

}

//  Helper function used to check if 2 coords point to the same position
bool isSameCoord(COORDINATE a, COORDINATE b) {

  //  Check coords
  if (a.x != b.x) {
    return false;
  }
  if (a.y != b.y) {
    return false;
  }
  return true;

}

//  Helper function used to get surrounding walls
byte senseWalls() {

  //  Declare starting byte
  byte b = B0000;

  //  Select current direction
  switch (dir) {

    //  If facing north
    case 0:
      if (wallFront()) {
        b |= B1000;
      }
      if (wallLeft()) {
        b |= B0001;
      }
      if (wallRight()) {
        b |= B0100;
      }
      break;

    //  If facing east
    case 1:
      if (wallFront()) {
        b |= B0100;
      }
      if (wallLeft()) {
        b |= B1000;
      }
      if (wallRight()) {
        b |= B0010;
      }
      break;

    //  If facing south
    case 2:
      if (wallFront()) {
        b |= B0010;
      }
      if (wallLeft()) {
        b |= B0100;
      }
      if (wallRight()) {
        b |= B0001;
      }
      break;

    //  If facing west
    case 3:
      if (wallFront()) {
        b |= B0001;
      }
      if (wallLeft()) {
        b |= B0010;
      }
      if (wallRight()) {
        b |= B1000;
      }
      break;

  }

  //  Return walls
  return b;

}

//  Helper function used to check if a specific coord is inside the grid
bool withinBounds(COORDINATE newCoords) {

  //  Check if too far north
  if (newCoords.y < 0) return false;

  //  Check if too far south
  if (newCoords.y >= 12) return false;

  //  Check if too far west
  if (newCoords.x < 0) return false;

  //  Check if too far east
  if (newCoords.x >= 12) return false;

  //  Else return true
  return true;

}

//  Helper function used to get the undiscovered adjacent cells at the current coords
ArduinoQueue<int> getUndiscoveredCells() {

  //  Declare variable to keep track of possible paths
  ArduinoQueue<int> paths(4);

  //  Get current cell
  byte cell = grid[coords.y][coords.x];

  //  Check if north cell is undiscovered
  if (cell & B1000 == B0000 && withinBounds({ coords.x, coords.y - 1 })) {

    //  Get north cell
    byte n = grid[coords.y - 1][coords.x];

    //  If undiscovered add to list of paths
    if (n == NULL) paths.enqueue(0);

  }

  //  Check if east cell is undiscovered
  if (cell & B0100 == B0000 && withinBounds({ coords.x + 1, coords.y })) {

    //  Get east cell
    byte e = grid[coords.y][coords.x + 1];

    //  If undiscovered add to list of paths
    if (e == NULL) paths.enqueue(1);

  }

  //  Check if south cell is undiscovered
  if (cell & B0010 == B0000 && withinBounds({ coords.x, coords.y + 1 })) {

    //  Get south cell
    byte s = grid[coords.y + 1][coords.x];

    //  If undiscovered add to list of paths
    if (s == NULL) paths.enqueue(2);

  }

  //  Check if west cell is undiscovered
  if (cell & B0001 == B0000 && withinBounds({ coords.x - 1, coords.y })) {

    //  Get west cell
    byte w = grid[coords.y][coords.x - 1];

    //  If undiscovered add to list of paths
    if (w == NULL) paths.enqueue(3);

  }

}

//  Function used to generate values for each cell in the grid
void generateFloodGrid(cppQueue target) {

  //  Declare variables
  Dictionary &d = *(new Dictionary());
  int dist = 0;

  //  Keep repeating untill all cells are updated
  while (d.count() < 144) {

    //  Declare temporary list
    cppQueue tempList(sizeof(COORDINATE));

    //  Loop through the list of cells to update
    for (int i = 0; i < target.getCount(); i++) {

      //  Get coords
      COORDINATE coord = {0, 0};
      target.peekIdx(&coord, i);

      //  Get cell
      byte cell = grid[coord.y][coord.x];

      //  Get coordinates in string form
      String stringCoords = String(coord.x) + String(' ') + String(coord.y);

      //  Check if this cell is already mapped
      if (d[stringCoords] == NULL || d[stringCoords] == "") { 
        
        //  Add to memo
        d(stringCoords, (String) dist);

        //  Update cell in grid
        floodGrid[coord.y][coord.x] = dist;

        //  Get adjacent cells
        COORDINATE n = { coord.x, coord.y - 1 };
        COORDINATE e = { coord.x + 1, coord.y };
        COORDINATE s = { coord.x, coord.y + 1 };
        COORDINATE w = { coord.x - 1, coord.y };

        //  Add adjacent cells to temp list if not already in list or memo and if accessible
        if (withinBounds(n) && (d[String(n.x + " " + n.y)] == NULL || d[String(n.x + " " + n.y)] == "") && grid[n.y][n.x] & B0010 == B0000) tempList.push(&n);
        if (withinBounds(e) && (d[String(e.x + " " + e.y)] == NULL || d[String(e.x + " " + e.y)] == "") && grid[e.y][e.x] & B0001 == B0000) tempList.push(&e);
        if (withinBounds(s) && (d[String(s.x + " " + s.y)] == NULL || d[String(s.x + " " + s.y)] == "") && grid[s.y][s.x] & B1000 == B0000) tempList.push(&s);
        if (withinBounds(w) && (d[String(w.x + " " + w.y)] == NULL || d[String(w.x + " " + w.y)] == "") && grid[w.y][w.x] & B0100 == B0000) tempList.push(&w);

      }

    }

    //  Replace old list with new one
    target = tempList;

    //  Increment distance
    dist++;

  }

}

//  Function called on start
void setup() {
  Serial.begin(19200);
}

//  Main loop
void loop() {

  //  Main loop where we try to discover the whole maze
  while (undiscovered > 0 || (undiscovered == 0 && isSameCoord(coords, startCoords))) {

    //  Get current cell
    byte cell = grid[coords.y][coords.x];

    //  If this cell is undiscovered
    if (cell == NULL) {

      //  Check for walls
      byte walls = senseWalls();

      //  Update grid
      grid[coords.y][coords.x] = walls;
      cell = walls;

      //  Decrement undiscovered count
      undiscovered--;

    }

    //  Get possible paths
    ArduinoQueue<int> paths(4);
    paths = getUndiscoveredCells();

    //  If there are no more possible paths
    if (paths.itemCount() == 0) {

      //  Move to the previous crossroad
      cppQueue target(sizeof(COORDINATE));
      COORDINATE prevCrossroad = { 0, 0 };
      prevCrossroads.pop(&prevCrossroad);
      target.push(&prevCrossroad);
      generateFloodGrid(target);
      moveToLeastValue();

    }
    else if (paths.itemCount() > 1) {

      //  Pick random path
      int chosen = paths.dequeue();

      //  Add this to list of crossroads
      prevCrossroads.push(&coords);

      //  Rotate and move
      faceDir(chosen);
      moveForward(1);

    }
    else {

      //  Choose path
      int chosen = paths.dequeue();

      //  Add path to latest crossroad
      if (prevCrossroads.getCount() == 0) prevCrossroads.push(&coords);

      //  Rotate and move
      faceDir(chosen);
      moveForward(1);

    }

  }

  //  All discovered
  explored = true;
  cppQueue center(sizeof(COORDINATE));
  COORDINATE center0 = { 5, 6 };
  COORDINATE center1 = { 6, 6 };
  COORDINATE center2 = { 5, 5 };
  COORDINATE center3 = { 6, 5 };
  center.push(&center0);
  center.push(&center1);
  center.push(&center2);
  center.push(&center3);
  generateFloodGrid(center);
  moveToLeastValue();
  
}
