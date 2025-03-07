
#include <iostream> // for basic IO
#include <vector> // for storing 3d data
#include "TemplatedStack.h" // Data structures class
#include "randgen.h" //wall removal randomness will be provided from this lib. Acquired from CS201 codebase.
#include <fstream> // for file handling
#include <sstream> //for dissection of strings
#include <string> //for to_str and main str operations

using namespace std;

//#define DEBUG

void GenerateMaze(vector<vector<vector<Cell>>>& vec, int i, int rows, int columns);
bool tryRemoveBottomWall(vector<vector<Cell>>& maze, Cell& curr);
bool tryRemoveTopWall(vector<vector<Cell>>& maze, Cell& curr);
bool tryRemoveRightWall(vector<vector<Cell>>& maze, Cell& curr);
bool tryRemoveLeftWall(vector<vector<Cell>>& maze, Cell& curr);
bool KnockRandom(vector<vector<Cell>>& V, Cell& attempt, Stack<Cell>& m, bool& failed);
void WriteStack(Stack<Cell>& stack, ofstream& ofile, const int& row);

//-----------------------------------------------------------------------------------------------------------------

int main(){
    
    //Input taking process for mazegen
    int mazeAmount = 0;
    string rowsandcolumns = "";
    int rows = 0;
    int columns = 0;
    
    cout << "Enter the number of mazes: ";
    cin >> mazeAmount;
    
    cout << "Enter the number of rows and columns (M and N): ";
    
    cin.ignore(999,'\n');
    getline(cin,rowsandcolumns);
    
    stringstream ss(rowsandcolumns);
    
    ss >> rows >> columns;
   
    #ifdef DEBUG
        cout << "(DEBUG) rows: " << rows << ", columns: " << columns << endl;
    #endif
    
    vector<vector<vector<Cell>>> vec; //a vector of mazes, that stored in matrices, will be used later when iterating over the number of userrequested mazes
    
    for (int i = 1; i <= mazeAmount; i++){ //generate mazeAmount number of mazes
        GenerateMaze(vec, i, rows, columns);
    }
    
    cout << "All mazes are generated.\n";
    
    //Input process for path finding step
    int choice, x1, x2, y1, y2;
    
    if (mazeAmount != 1){
        cout << "Enter a maze ID between 1 and " << mazeAmount << " inclusive to find a path: ";
        cin >> choice;
    } else {
        choice = 1;
    }
    
    cout << "Enter x and y coordinates of the entry points (x,y) or (column,row): ";
    cin >> x1 >> y1;
    
    cout << "Enter x and y coordinates of the exit points (x,y) or (column,row): ";
    cin >> x2 >> y2;
    
    
    Stack<Cell>solve; //solving stack
    vector<vector<Cell>>maze; //maze that we will solve
    
    Cell temp; //temporary current cell
    Cell prev; //temporary cell to keep track of previous one 
  
  
    //Coord conversion, (0,0)
    int a,b;
    a = rows-1-y1;
    b = x1;
    maze = vec[choice-1];
    
    temp = maze[a][b]; //set temp as the starting point than push it to the stack 
    solve.push(temp); 
    
    a = rows-1-y2;
    b = x2;
    
    //Main Backtracking happens here
    while (!(temp.x == a && temp.y == b)) //until we reach the requested ending point
    {
        #ifdef DEBUG
            cout << "(DEBUG) Current Position: \t(" << temp.x << ", " << temp.y << ")" << endl;
        #endif
        bool failed = false; //to track if it fails trying all possible directions
        
        
        if (maze[temp.x][temp.y].top== 0 && !(prev.x == temp.x-1 && prev.y == temp.y) && !maze[temp.x-1][temp.y].visited_pf)
        {
            maze[temp.x-1][temp.y].visited_pf = true; //set it as visited
            prev = temp; //update the preview
            temp = maze[temp.x-1][temp.y]; //set it to upper cell
            solve.push(maze[temp.x][temp.y]); //push the new cell
        }
        //going down (same idea as above)
        else if (maze[temp.x][temp.y].bottom== 0 && !(prev.x == temp.x+1 && prev.y == temp.y) && !maze[temp.x+1][temp.y].visited_pf)
        {
            maze[temp.x+1][temp.y].visited_pf = true;
            prev = temp;
            temp = maze[temp.x+1][temp.y];
            solve.push(maze[temp.x][temp.y]);
        }
        //going right
        else if (maze[temp.x][temp.y].right== 0 && temp.y+1 <= columns && !(prev.x == temp.x && prev.y == temp.y+1) && !maze[temp.x][temp.y+1].visited_pf)
        {
            maze[temp.x][temp.y+1].visited_pf = true;
            prev = temp;
            temp = maze[temp.x][temp.y+1];
            solve.push(maze[temp.x][temp.y]);
        }
        //going left
        else if (maze[temp.x][temp.y].left== 0 && !(prev.x == temp.x && prev.y == temp.y-1) && !maze[temp.x][temp.y-1].visited_pf)
        {
            maze[temp.x][temp.y-1].visited_pf = true;
            prev = temp;
            temp = maze[temp.x][temp.y-1];
            solve.push(maze[temp.x][temp.y]);
        } else {
            failed = true; //if all directions have failed, set flag
        }
        
        if (failed == true){
            #ifdef DEBUG
                cout << "(DEBUG) Backtracking from: \t(" << temp.x << ", " << temp.y << ")" << endl;
            #endif
            try {
                
                solve.pop(temp); //remove the upper one
                solve.pop(temp); //pop the latter one as temp
                solve.push(temp); //than push it back
                
            } catch (const underflow_error& e){
                cout << "Stack Error:" << e.what() << endl;
            }
        }
    }
    
    ofstream ofile; 

    //Constructing the filename according to the number of mazes generated
    string filename = "maze_" + to_string(choice) + "_path_" + to_string(x1) + "_" + to_string(y1) + "_" + to_string(x2) + "_" + to_string(y2) + ".txt";
    
    //Handle file writing of paths
    ofile.open(filename.c_str());
    WriteStack(solve, ofile, rows); 
    ofile.close(); 
    
    return 0;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


void GenerateMaze(vector<vector<vector<Cell>>>& vec, int i, int rows, int columns) {
    
    //Generates a maze with full of walls intact, will be altered by "KnockRandom"
    vector<vector<Cell>> maze(rows, vector<Cell>(columns)); 
    
    // Initialize maze cells
    for (int x = 0; x < maze.size(); x++) { //initializes the coord values of the struct beforehand
        for (int y = 0; y < maze[0].size(); y++) {
            maze[x][y].x = x;
            maze[x][y].y = y;
        }
    }
    
    //generate stack, push (0,0) into it, set it to visited by default and than set the temp 
    Stack<Cell> mazeMaker; 
    mazeMaker.push(maze[0][0]); 
    maze[0][0].visited_gen = true;
    Cell temp = maze[0][0]; 

    // Generate maze, main processing goes here
    while (!mazeMaker.isEmpty()) { //until the mazeMaker stack is empty
    
        bool done = false; //flag for the cell to try its every possible path
        bool failed = false; //flag for tracking the state of current cell
        
        while (done == false) { //enforce trying in each direction
            done = KnockRandom(maze, temp, mazeMaker, failed); //keep trying to pop a wall randomly, function generates random direction each time
        }
        
        if (failed == true) { //if stuck in the cell:
            try{
                mazeMaker.pop(temp); //attempt to backtrack
            } catch (const underflow_error& e){
                cout << " Stack Error(pop action): " << e.what() << '\n';
            }
            
        }
    }
    vec.push_back(maze); //push the maze to the maze vector

    #ifdef DEBUG
        cout << "(DEBUG) Maze_row_size: " << maze.size() << endl;
        cout << "(DEBUG) Maze_column_size: " << maze[0].size() << endl;
    #endif
    
    //Write maze to file, the step of row&column info
    ofstream ofile1;
    string filename = "maze_" + to_string(i) + ".txt"; //Forming the file name with appending i
    ofile1.open(filename.c_str());
    ofile1 << maze.size() << " " << maze[0].size() << endl;
    
    //Output the rest in the requested format given in mazedrawer rar
    int k = rows - 1;
    for (int i = 0; i < maze.size(); i++) {
        for (int j = 0; j < maze[0].size(); j++) {
            ofile1 << "x=" << j << " y=" << i << " l=" << maze[k][j].left << " r=" << maze[k][j].right
            << " u=" << maze[k][j].top << " d=" << maze[k][j].bottom << endl;
        }
        k--;
    } 
    
    ofile1.close(); // close the output file
}

// --------------------------------------------------------------------------------------------       

//Attempts to remove the bottom wall of the current cell if it leads to an unvisited cell
bool tryRemoveBottomWall(vector<vector<Cell>>& maze, Cell& curr) {
    
    //Check if the bottom cell is within the maze bounds and unvisited
    if (curr.x + 1 < maze.size() && !maze[curr.x + 1][curr.y].visited_gen) {
        
        //Remove the bottom
        maze[curr.x][curr.y].bottom = 0;

        //Mark as visited
        maze[curr.x + 1][curr.y].visited_gen = true;

        // Remove the top wall of the bottom cell since it's the same wall as the current cell's bottom wall
        maze[curr.x + 1][curr.y].top = 0;

        //indicate that wall was successfully removed
        return true;
    }

    //Return false if the bottom wall cannot be removed 
    return false;
}

// --------------------------------------------------------------------------------------------

//Attempts to remove the top wall of the current cell if it leads to an unvisited cell
bool tryRemoveTopWall(vector<vector<Cell>>& maze, Cell& curr) {
    
    //Check if the top cell is within the maze bounds and unvisited
    if (curr.x - 1 >= 0 && !maze[curr.x - 1][curr.y].visited_gen) {
        
        //Remove the top
        maze[curr.x][curr.y].top = 0;

        //Mark as visited
        maze[curr.x - 1][curr.y].visited_gen = true;

        //Remove the bottom wall of the top cell since it's the same wall as the current cell's top wall
        maze[curr.x - 1][curr.y].bottom = 0;

        //indicate that wall was successfully removed
        return true;
    }

    //Return false if the top wall cannot be removed 
    return false;
}

// --------------------------------------------------------------------------------------------

// Attempts to remove the right wall of the current cell if it leads to an unvisited cell
bool tryRemoveRightWall(vector<vector<Cell>>& maze, Cell& curr) {
    
    //Check if the right cell is within the maze bounds and unvisited
    if (curr.y + 1 < maze[0].size() && !maze[curr.x][curr.y + 1].visited_gen) {
        
        // Remove the right wall of the current cell
        maze[curr.x][curr.y].right = 0;

        //mark as visited
        maze[curr.x][curr.y + 1].visited_gen = true;

        // Remove the left wall of the right cell since it's the same wall as the current cell's right wall
        maze[curr.x][curr.y + 1].left = 0;

        //indicate that wall was successfully removed
        return true;
    }

    //Return false if the right wall cannot be removed 
    return false;
}

// --------------------------------------------------------------------------------------------

// Attempts to remove the left wall of the current cell if it leads to an unvisited cell
bool tryRemoveLeftWall(vector<vector<Cell>>& maze, Cell& curr) {
    
    //check if the left cell is within the maze bounds and unvisited
    if (curr.y - 1 >= 0 && !maze[curr.x][curr.y - 1].visited_gen) {
        
        //Remove the left wall of the current cell
        maze[curr.x][curr.y].left = 0;

        //Mark as visited
        maze[curr.x][curr.y - 1].visited_gen = true;

        //Remove the right wall of the left cell since it's the same wall as the current cell's left wall
        maze[curr.x][curr.y - 1].right = 0;

        //indicate that wall was successfully removed
        return true;
    }

    //Return false if the left wall cannot be removed 
    return false;
}

// --------------------------------------------------------------------------------------------

//will attempt to knock down walls randomly
bool KnockRandom(vector<vector<Cell>>& maze, Cell& attempt, Stack<Cell>& result, bool& failed) {   
    
    //Random int Generation
    int dir;
    RandGen r;
    dir  = r.RandInt(1,4);
   
    //Main processing for each direction:
    if (dir == 1){ //feasability of bottom cell
    
        if (tryRemoveBottomWall(maze, attempt) == true){ //if feasible: 
            attempt = maze[(attempt.x+1)][attempt.y]; //if so, set the attempted cell to the than push it to the result stack
            result.push(maze[attempt.x][attempt.y]); 
            return true;
        }
        attempt.flag_down = true; //set the flag as tried
        
    } else if (dir == 2){ //feasability of left cell
    
        if (tryRemoveLeftWall(maze, attempt) == true){ //if feasable: 
            attempt = maze[attempt.x][attempt.y-1]; //if so, set the attempted cell to the than push it to the result stack
            result.push(maze[attempt.x][attempt.y]); 
            return true;
        }
        attempt.flag_left = true; //set the flag as tried
        
    } else if (dir == 3){ //feasability of upper cell

        if (tryRemoveTopWall(maze, attempt) == true) //if popping up works
        {
            attempt = maze[attempt.x-1][attempt.y]; //if so, set the attempted cell to the than push it to the result stack
            result.push(maze[attempt.x][attempt.y]); 
            return true;
        }
        attempt.flag_up = true; //set the flag as tried
    } else {
        
       if (tryRemoveRightWall(maze, attempt) == true){ //feasability of right cell
       
           attempt = maze[attempt.x][attempt.y+1]; //if so, set the attempted cell to the than push it to the result stack
           result.push(maze[attempt.x][attempt.y]); 
           return true;
       }
       attempt.flag_right = true; //set the flag as tried
        
    } if (attempt.flag_down && attempt.flag_left && attempt.flag_up && attempt.flag_right){
        
        //no expandable paths for this try
        failed = true; 
        
        //break
        return true; 
    }
    
    //all possibilities have been exhausted
    return false;
}

// --------------------------------------------------------------------------------------------

void WriteStack(Stack<Cell>& stack, ofstream& ofile, const int& row) {
    
    //temporary stack for stack reversal
    Stack<Cell> tempStack;

    //pop elements from the original stack and push them onto the temp one
    while (!stack.isEmpty()) {
        Cell t = stack.topAndPop();
        tempStack.push(t);
    }

    //Now that the temporary stack is in reverse order, write the elements to the file
    while (!tempStack.isEmpty()) {
        Cell temp = tempStack.topAndPop();
        
        // Convert coords and write one by one
        ofile << temp.y << " " << (row - temp.x - 1) << endl;
    }
}
