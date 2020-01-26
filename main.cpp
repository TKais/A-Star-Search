#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
using std::cout;
using std::vector;
using std::ifstream;
using std::istringstream;
using std::string;
using std::abs;
using std::sort;

enum class State {kEmpty, kObstacle, kClosed, kPath, kStart, kFinish};

const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

vector<State> ParseLine(string line) {
    istringstream sline(line);
    int n;
    char c;
    vector<State> row;
    while (sline >> n >> c && c == ',') {
      if (n == 0) {
        row.push_back(State::kEmpty);
      } else {
        row.push_back(State::kObstacle);
      }
    }
    return row;
}

vector<vector<State>> ReadBoardFile(string path) {
    ifstream my_file(path);
    vector<vector<State>> board = {};

    if(my_file) {
        string line;
        while(getline(my_file, line)) {
            vector<State> row = ParseLine(line);
            board.push_back(row);
        }
    }
    return board;
}

bool Compare(vector<int> node1, vector<int> node2) {
  int fValue1 = node1[2] + node1[3];
  int fValue2 = node2[2] + node2[3];
  if(fValue1 > fValue2) {
    return true;
  }
  return false;
}

string CellString(State cell) {
  switch(cell) {
    case State::kObstacle: return "⛰️   ";
    case State::kPath: return "🚗   ";
    case State::kStart: return "🚦   ";
    case State::kFinish: return "🏁   ";
    default: return "0   "; 
  }
}

void CellSort(vector<vector<int>> *v) {
  // sorts the open list according to the sum of h + g
  sort(v->begin(), v->end(), Compare);
}

bool CheckValidCell(int x, int y, vector<vector<State>> &grid) {
    // ensures that the potential neighbor coordinates are on the grid and that the cell is open
  bool on_grid_x = (x >= 0 && x < grid.size());
  bool on_grid_y = (y >= 0 && y < grid[0].size());
  if (on_grid_x && on_grid_y) {
    return grid[x][y] == State::kEmpty;
  }
  return false;
}

int Heuristic(int coordX1, int coordY1, int coordX2, int coordY2) {
    // computes the distance to the goal
    return abs(coordX1 - coordX2) + abs(coordY1 - coordY2);
}

void AddToOpen(int x, int y, int g, int h, vector<vector<int>> &open, vector<vector<State>> &grid) {
    // adds the node to the open list and marks the grid cell as closed
    vector<int> node = {x, y, g, h};
    open.push_back(node);
    grid[x][y] = State::kClosed;
}

void ExpandNeighbors(const vector<int> &current, int goal[2], vector<vector<int>> &openlist, vector<vector<State>> &grid) {
  // Get current node's data.
  int x = current[0];
  int y = current[1];
  int g = current[2];

  // Loop through current node's potential neighbors.
  for (int i = 0; i < 4; i++) {
    int x2 = x + delta[i][0];
    int y2 = y + delta[i][1];

    // Check that the potential neighbor's x2 and y2 values are on the grid and not closed.
    if (CheckValidCell(x2, y2, grid)) {
      // Increment g value and add neighbor to open list.
      int g2 = g + 1;
      int h2 = Heuristic(x2, y2, goal[0], goal[1]);
      AddToOpen(x2, y2, g2, h2, openlist, grid);
    }
  }
}

vector<vector<State>> Search(vector<vector<State>> grid, int init[2], int goal[2]) {
  // Create the vector of open nodes.
  vector<vector<int>> open {};

  // Set state for init and goal coordinates
  grid[init[0]][init[1]] = State::kStart;
  grid[goal[0]][goal[1]] = State::kFinish;
  
  // Initialize the starting node variables. 
  int x = init[0];
  int y = init[1];
  int g = 0;
  int h = Heuristic(init[0], goal[0], init[1], goal[1]);
  // Use AddToOpen to add the starting node to the open vector.
  AddToOpen(x, y, g ,h, open, grid);

  while(open.size() > 0) {
      CellSort(&open);
      auto currentNode = open.back();
      open.pop_back();
      x = currentNode[0];
      y = currentNode[1];
      grid[x][y] = State::kPath;

      if(x == goal[0] && y == goal[1]) {
          return grid;
      }
      ExpandNeighbors(currentNode, goal, open, grid);
  }

  return std::vector<vector<State>>{};
}

void PrintBoard(const vector<vector<State>> board) {
  for (int i = 0; i < board.size(); i++) {
    for (int j = 0; j < board[i].size(); j++) {
      cout << CellString(board[i][j]);
    }
    cout << "\n";
  }
}

int main() {
    int init[2]{0, 0};
    int goal[2]{4, 5};
    vector<vector<State>> board = ReadBoardFile("1.board");
    vector<vector<State>> solution = Search(board, init, goal);
    PrintBoard(solution);
}