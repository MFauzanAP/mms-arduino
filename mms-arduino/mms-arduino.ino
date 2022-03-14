//  Imports
#include <Robojax_L298N_DC_motor.h>
#include <Dictionary.h>
#include <ArduinoQueue.h>
#include <RobotIRremote.h>
#include <RobotIRremoteInt.h>
#include <RobotIRremoteTools.h>

//  Declare structs
struct COORDINATE = {
  int x;
  int y;  
};

//  Declare variables
byte grid[12][12];
ArduinoQueue<COORDINATE> prevCrossroads();
COORDINATE coords = { 0, 0 };
COORDINATE start = [ 0, 0 ];
int dir = 0;
int undiscovered = 144;
bool explored = false;

//  Helper function used to check if 2 coords point to the same position
bool isSameCoord(COORDINATE a], COORDINATE b) {

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
        b |= B1000
      }
      if (wallLeft()) {
        b |= B0001
      }
      if (wallRight()) {
        b |= B0100
      }
      break;

    //  If facing east
    case 1:
      if (wallFront()) {
        b |= B0100
      }
      if (wallLeft()) {
        b |= B1000
      }
      if (wallRight()) {
        b |= B0010
      }
      break;

    //  If facing south
    case 2:
      if (wallFront()) {
        b |= B0010
      }
      if (wallLeft()) {
        b |= B0100
      }
      if (wallRight()) {
        b |= B0001
      }
      break;

    //  If facing west
    case 3:
      if (wallFront()) {
        b |= B0001
      }
      if (wallLeft()) {
        b |= B0010
      }
      if (wallRight()) {
        b |= B1000
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

//  Function called on start
void setup() {
  Serial.begin(19200);
}

//  Main loop
void loop() {
  log("Running...");
  setColor(0, 0, 'G');
  setText(0, 0, "abc");

  //  Main loop where we try to discover the whole maze
  while (undiscovered > 0 || (undiscovered == 0 && isSameCoord(coords, start))) {

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
    ArduinoQueue<int> paths(4) = getUndiscoveredCells();

    //  If there are no more possible paths
    if (paths.itemCount() == = 0) {

      //  Move to the previous crossroad
      pathfind(prevCrossroads[prevCrossroads.length - 1].split(' '));

      //  Delete crossroad
      prevCrossroads.splice(prevCrossroads.length - 1, 1);

    }
    else if (paths.itemCount() > 1) {

      //  Pick random path
      const chosen = paths[Math.floor(Math.random() * paths.length)];

      //  Add this to list of crossroads
      API.setColor(coords.x, API.mazeHeight() - coords.y - 1, 'R');
      prevCrossroads.push(coords.join(' '));

      //  Rotate and move
      faceDir(chosen);
      moveForward();

    }
    else {

      //  Choose path
      const chosen = paths[0];

      //  Add path to latest crossroad
      prevCrossroads.push(coords.join(' '));

      //  Rotate and move
      faceDir(chosen);
      moveForward();

    }

  }
}
