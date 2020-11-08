#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <ctime>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <string>
using namespace std;


static const int ROAD = 0;
static const int WALL = 1;
static const int BEGINING = 2;
static const int EXIT = 3;
static const int STEP = 4;
static const int godown = 101;
static const int goright = 102;
static const int goleft = 103;
static const int goup = 104;
static int weight = 10;
static int row = 0;
static int col = 0;

struct Space3x3
{
	int x, y, distance;
	Space3x3(int n, int m, int dir)
	{
		x = n;
		y = m;
		distance = dir;
	}
};

struct point
{
	int x;
	int y;
}start;



struct Node
{
	int x_pos, y_pos;
	int valuetobeginning; //The actual value from start to final
	int HinAstar;//The estimate of the starting to final point
	int FinAstar;//estimate
	Node* parent;
	Node(int x_pos, int y_pos)
	{
		this->x_pos = x_pos;
		this->y_pos = y_pos;
		this->valuetobeginning = 0;
		this->HinAstar = 0;
		this->FinAstar = 0;
		this->parent = NULL;
	}
	Node(int x_pos, int y_pos, Node* parent)
	{
		this->x_pos = x_pos;
		this->y_pos = y_pos;
		this->valuetobeginning = 0;
		this->HinAstar = 0;
		this->FinAstar = 0;
		this->parent = parent;
	}
};

class Astarpathfinding
{
public:
	Astarpathfinding();
	~Astarpathfinding();
	void SearchShortestPath(Node* start_pos, Node* end_pos);
	bool FindPath(Node* start_pos, Node* end_pos);
	void StepCheck(int x_pos, int y_pos, Node* parent, int real_value);
	void Nextstepgo(Node* cur_point);
	int CheckContains(vector<Node*>* node_list, int x_pos, int y_pos);
	void CountGHF(Node* s_node, Node* e_node, int real_value);
	static bool Compare(Node* n1, Node* n2);
	bool WallsetCheck(int x_pos, int y_pos);
	void FindPath(Node* current);
	vector<Node*> open_list;
	vector<Node*> close_list;
	Node *start_pos;
	Node *end_pos;
};

vector<Space3x3> squre;
int xpos = 1, y_pos = 1;
int** maze = nullptr;
int maze_w = 0;
int maze_h = 0;
int end_pos = 0;
vector<point> end_points;

void RestartMaze()
{
	for (int i = 0; i < maze_h; i++)
		for (int j = 0; j < maze_w; j++)
			maze[i][j] = WALL;
}

void SetSquare()
{
	end_points.clear();
	RestartMaze();
	maze[1][1] = ROAD;
}

void GetNextSqure()
{
	//down
	if (xpos + 1 < maze_h - 1 && maze[xpos + 1][y_pos] == WALL) 
	{
		squre.push_back(Space3x3(xpos + 1, y_pos, godown));
	}
	//right
	if (y_pos + 1 < maze_w - 1 && maze[xpos][y_pos + 1] == WALL) 
	{
		squre.push_back(Space3x3(xpos, y_pos + 1, goright));
	}
	//up
	if (xpos - 1 >= 1 && maze[xpos - 1][y_pos] == WALL)
	{
		squre.push_back(Space3x3(xpos - 1, y_pos, goup));
	}
	//left
	if (y_pos - 1 >= 1 && maze[xpos][y_pos - 1] == WALL) 
	{
		squre.push_back(Space3x3(xpos, y_pos - 1, goleft));
	}
}

//	Clear the input cache
void ClearQueue()
{
	while (getchar() != '\n')
		continue;
}

//	ensure input is intage
int InputInt(char const* msg)
{
	int result;
	cout << msg;
	while (scanf("%d", &result) != 1)
	{
		cout << "Invalid input!" << endl;
		ClearQueue();
		cout << msg;
	}
	ClearQueue();
	return result;
}

//	ensure input is odd
int InputOddInt(char const* msg)
{
	int result;
	cout << msg;
	while (scanf("%d", &result) != 1 || result % 2 != 1)
	{
		cout << "Invalid input!Please enter an odd number!" << endl;
		ClearQueue();
		cout << msg;
	}
	ClearQueue();
	return result;
}

//	input an intage in a range
int InputIntInRange(char const * msg, int l, int r)
{
	int j;
	while (1)
	{
		j = InputInt(msg);
		if (j<l || j>r)
		{
			cout << "Please enter an integer between "<< l <<" and "<< r <<"£¡\n";
			continue;
		}
		else
			break;
	}
	return j;
}

//	ensure input is a string
char* InputLine(char const * msg, char * str)
{
	int j = 0;
	int c;
	cout << msg;
	while (1)
	{
		c = getchar();
		if (c == '\n' || c == EOF)
			break;
		str[j++] = c;
	}
	str[j] = '\0';
	return str;
}

// Free maze array memory
void FreeMaze()
{
	for (int i = 0; i < maze_h; i++)
	{
		delete[] maze[i];
	}
	delete[]maze;
	maze = nullptr;
}


void PrintoutMaze()
{
	for (int i = 0; i < maze_h; i++)
	{
		for (int j = 0; j < maze_w; j++)
		{
			if (maze[i][j] == WALL)
				printf("X");
			else if (maze[i][j] == BEGINING)
				printf("S");
			else if (maze[i][j] == EXIT)
				printf("E");
			else if (maze[i][j] == STEP)
				printf("o");
			else{
				printf(" ");
			}
		}
		printf("\n");
	}
}

// Set the exit and better the maze
bool SetStartAndEnd()
{
	int center_posx = maze_h / 2;
	int center_posy = maze_w / 2;
	int i = 1, j = maze_h - 1, k = 1, l = maze_h - 1;
	// 3 x 3 in middle must be space
	for (i = center_posx - 1; i < center_posx + 2; i++)
	{
		for (j = center_posy - 1; j < center_posy + 2; j++)
		{
			maze[i][j] = ROAD;
		}
	}
	//The perimeter must be walled
	for (i = 0; i < maze_w; i++)
	{
		maze[0][i] = WALL;
		maze[maze_h - 1][i] = WALL;
	}
	for (i = 0; i < maze_h; i++)
	{
		maze[i][maze_w - 1] = WALL;
		maze[i][0] = WALL;
	}

	start.x = center_posx;
	start.y = center_posy;
	Astarpathfinding astar;
	Node *start_pos = new Node(start.x, start.y);
	row = maze_h - 1;
	col = maze_w - 1;
	maze[center_posx][center_posy] = BEGINING;
	int num_end = 0;
	i = 1;
	j = maze_h - 1;
	k = 1;
	l = maze_h - 1;
	//find exits 
	while (num_end < end_pos)
	{
		for (; i < maze_w; i++)
		{
			if (maze[1][i] == ROAD && maze[0][i] == WALL)
			{
				Node *end_pos = new Node(0, i);
				//if (astar.CheckPath(start_pos, end_pos))
				{
					point pt;
					pt.x = 0;
					pt.y = i;
					end_points.push_back(pt);
					maze[0][i] = EXIT;
					num_end++;
					break;
				}
			}
		}
		for (; j > 0 && num_end < end_pos; j--)
		{
			if (maze[j][1] == ROAD && maze[j][0] == WALL)
			{
				Node *end_pos = new Node(j, 0);
				//if (astar.CheckPath(start_pos, end_pos))
				{
					point pt;
					pt.x = j;
					pt.y = 0;
					end_points.push_back(pt);
					maze[j][0] = EXIT;
					num_end++;
					break;
				}
			}
		}
		for (; k < maze_w && num_end < end_pos; k++)
		{
			if (maze[maze_h - 2][k] == ROAD && maze[maze_h - 1][k] == WALL)
			{
				Node *end_pos = new Node(maze_h - 1, k);
				//if (astar.CheckPath(start_pos, end_pos))
				{
					point pt;
					pt.x = maze_h - 1;
					pt.y = k;
					end_points.push_back(pt);
					maze[maze_h - 1][k] = EXIT;
					num_end++;
					break;
				}
			}
		}
		for (; l > 0 && num_end < end_pos; l--)
		{
			if (maze[l][maze_w - 2] == ROAD && maze[l][maze_w - 1] == WALL)
			{
				Node *end_pos = new Node(l, maze_w - 1);
				//if (astar.CheckPath(start_pos, end_pos))
				{
					point pt;
					pt.x = l;
					pt.y = maze_w - 1;
					end_points.push_back(pt);
					maze[l][maze_w - 1] = EXIT;
					num_end++;
					break;
				}
			}
		}
		if (i == maze_w && j == 0 && k == maze_w && l == 0)
			break;
		if (num_end == end_pos)
			break;
	}
	if (num_end == end_pos)
		return true;
	else
		return false;
}

bool CreateMaze()
{
	SetSquare();
	GetNextSqure();

	while (squre.size()) 
	{
		int squre_size = squre.size();
		int randnum = rand() % squre_size;
		Space3x3 select_squre = squre[randnum];
		xpos = select_squre.x;

		y_pos = select_squre.y;

		switch (select_squre.distance) 
		{
		case godown:
			xpos++;
			break;

		case goright:
			y_pos++;
			break;

		case goleft:
			y_pos--;
			break;

		case goup:
			xpos--;
			break;
		}
		//If the target block is a wall
		if (maze[xpos][y_pos] == WALL) 
		{
			//Break through the wall and the target block
			maze[select_squre.x][select_squre.y] = maze[xpos][y_pos] = ROAD;
			//Again, find the wall next to the miner's current position
			GetNextSqure();
		}

		//Remove the wall (remove the walls that don't work, don't have to work on those that have already been built, and also make sure we get out of the loop)
		squre.erase(squre.begin() + randnum);
	}
	return SetStartAndEnd();
}


Astarpathfinding::Astarpathfinding()
{
}
Astarpathfinding::~Astarpathfinding()
{
}

void Astarpathfinding::SearchShortestPath(Node* start_pos, Node* end_pos)
{
	if (start_pos->x_pos < 0 || start_pos->x_pos > row || start_pos->y_pos < 0 || start_pos->y_pos > col
		|| end_pos->x_pos < 0 || end_pos->x_pos > row || end_pos->y_pos < 0 || end_pos->y_pos > col)
		return;
	Node* current;
	this->start_pos = start_pos;
	this->end_pos = end_pos;
	open_list.push_back(start_pos);
	while (open_list.size() > 0)
	{
		current = open_list[0];
		if (current->x_pos == end_pos->x_pos && current->y_pos == end_pos->y_pos)
		{
			FindPath(current);
			open_list.clear();
			close_list.clear();
			break;
		}
		Nextstepgo(current);
		close_list.push_back(current);
		open_list.erase(open_list.begin());
		sort(open_list.begin(), open_list.end(), Compare);
	}
}

bool Astarpathfinding::FindPath(Node* start_pos, Node* end_pos)
{
	if (start_pos->x_pos < 0 || start_pos->x_pos > row || start_pos->y_pos < 0 || start_pos->y_pos > col
		|| end_pos->x_pos < 0 || end_pos->x_pos > row || end_pos->y_pos < 0 || end_pos->y_pos > col)
		return false;
	Node* current;
	this->start_pos = start_pos;
	this->end_pos = end_pos;
	open_list.push_back(start_pos);
	while (open_list.size() > 0)
	{
		current = open_list[0];
		if (current->x_pos == end_pos->x_pos && current->y_pos == end_pos->y_pos)
		{
			open_list.clear();
			close_list.clear();
			return true;
		}
		Nextstepgo(current);
		close_list.push_back(current);
		open_list.erase(open_list.begin());
		sort(open_list.begin(), open_list.end(), Compare);
	}
	return false;
}

void Astarpathfinding::StepCheck(int x_pos, int y_pos, Node* parent, int real_value)
{
	if (x_pos < 0 || x_pos > row || y_pos < 0 || y_pos > col)
		return;
	if (this->WallsetCheck(x_pos, y_pos))
		return;
	if (CheckContains(&close_list, x_pos, y_pos) != -1)
		return;
	int index;
	if ((index = CheckContains(&open_list, x_pos, y_pos)) != -1)
	{
		Node *point = open_list[index];
		if (point->valuetobeginning > parent->valuetobeginning + real_value)
		{
			point->parent = parent;
			point->valuetobeginning = parent->valuetobeginning + real_value;
			point->FinAstar = point->valuetobeginning + point->HinAstar;
		}
	}
	else
	{
		Node * point = new Node(x_pos, y_pos, parent);
		CountGHF(point, end_pos, real_value);
		open_list.push_back(point);
	}
}

void Astarpathfinding::Nextstepgo(Node* current)
{
	StepCheck(current->x_pos - 1, current->y_pos, current, weight);//×ó
	StepCheck(current->x_pos + 1, current->y_pos, current, weight);//ÓÒ
	StepCheck(current->x_pos, current->y_pos + 1, current, weight);//ÉÏ
	StepCheck(current->x_pos, current->y_pos - 1, current, weight);//ÏÂ
}

int Astarpathfinding::CheckContains(vector<Node*>* node_list, int x_pos, int y_pos)
{
	for (int i = 0; i < node_list->size(); i++)
	{
		if (node_list->at(i)->x_pos == x_pos && node_list->at(i)->y_pos == y_pos)
		{
			return i;
		}
	}
	return -1;
}

void Astarpathfinding::CountGHF(Node* s_node, Node* e_node, int real_value)
{
	int h_value = abs(s_node->x_pos - e_node->x_pos) * weight + abs(s_node->y_pos - e_node->y_pos) * weight;
	int currentg = s_node->parent->valuetobeginning + real_value;
	int f_value = currentg + h_value;
	s_node->FinAstar = f_value;
	s_node->HinAstar = h_value;
	s_node->valuetobeginning = currentg;
}


bool Astarpathfinding::Compare(Node* n1, Node* n2)
{
	return n1->FinAstar < n2->FinAstar;
}

bool Astarpathfinding::WallsetCheck(int x_pos, int y_pos)
{
	if (maze[x_pos][y_pos] == WALL)
		return true;
	return false;
}

void Astarpathfinding::FindPath(Node* current)
{
	if (current->parent != NULL)
		FindPath(current->parent);
	if (maze[current->x_pos][current->y_pos] == ROAD)
		maze[current->x_pos][current->y_pos] = STEP;
}


void FindShortestPath()
{
	Astarpathfinding astar;
	Node *start_pos = new Node(start.x, start.y);
	for (int i = 0; i < end_points.size(); i++)
	{
		Node *end_pos = new Node(end_points[i].x, end_points[i].y);
		astar.SearchShortestPath(start_pos, end_pos);
		cout << endl;
		PrintoutMaze();
	}
}

// set maze
void GenMaze()
{
	if (maze != nullptr)
		FreeMaze();
	maze_h = InputInt("Height:");
	maze_w = InputInt("Width:");
	maze = new int*[maze_h];
	for (int i = 0; i < maze_h; i++)
	{
		maze[i] = new int[maze_w];
	}
	RestartMaze();
	end_pos = InputInt("Number of exits:");
	while (CreateMaze() == false)
	{
		RestartMaze();
	}
	PrintoutMaze();
}

// save maze
void SaveMaze()
{
	char file_name[256];
	memset(file_name, 0, sizeof(file_name));
	InputLine("Enter the filename:", file_name);
	ofstream  outfile;
	outfile.open(file_name, ios::out);
	if (!outfile)
	{
		cerr << "Open File Fail." << endl;
		exit(1);
	}
	outfile << maze_h << " " << maze_w << endl;
	for (int i = 0; i < maze_h; ++i)
	{
		for (int j = 0; j < maze_w; j++)
		{
			if (maze[i][j] == WALL)
				outfile << "X";
			else if (maze[i][j] == BEGINING)
				outfile << "S";
			else if (maze[i][j] == EXIT)
				outfile << "E";
			else if (maze[i][j] == STEP)
				outfile << "o";
			else
				outfile << " ";
		}
		outfile << endl;
	}
	outfile.close();

}

// load maze
void LoadMaze()
{
	if (maze != nullptr)
		FreeMaze();
	char file_name[256];
	memset(file_name, 0, sizeof(file_name));
	InputLine("Please enter the filename:", file_name);
	ifstream  infile;
	infile.open(file_name, ios::in);
	if (!infile)
	{
		cerr << "Open file failed." << endl;
		return;
	}
	infile >> maze_h >> maze_w;
	maze = new int*[maze_h];
	for (int i = 0; i < maze_h; i++)
	{
		maze[i] = new int[maze_w];
	}
	end_points.clear();
	char ch;
	for (int i = 0; i < maze_h; ++i)
	{
		string str;
		while (getline(infile, str))
		{

			for (int j = 0; j < str.length(); j++)
			{
				ch = str[j];
				if (ch == 'X')
				{
					maze[i][j] = WALL;
				}
				else if (ch == 'S')
				{
					maze[i][j] = BEGINING;
				}
				else if (ch == 'E')
				{
					maze[i][j] = EXIT;
				}
				else if (ch == ' ' || ch == 'o')
				{
					maze[i][j] = ROAD;
				}

				if (maze[i][j] == BEGINING)
				{
					start.x = i;
					start.y = j;
				}
				else if (maze[i][j] == EXIT)
				{
					point pt;
					pt.x = i;
					pt.y = j;
					end_points.push_back(pt);
				}
			}
			if (str.length() > 0)
				break;
		}
	}
	infile.close();
	PrintoutMaze();
}

void FindPath()
{
	row = maze_h - 1;
	col = maze_w - 1;
	FindShortestPath();
}

// main menu of this cw
void MainMenu()
{
	int option = -1;
	do 
	{
		cout << "1.Make a new maze" << endl;
		cout << "2.Find the shortest road" << endl;
		cout << "3.Save maze to file" << endl;
		cout << "4.Load previously saved mazes" << endl;
		cout << "5.Exit" << endl;
		option = InputIntInRange("Opt:", 1, 5);
		switch (option)
		{
		case 1:
			GenMaze();
			break;
		case 2:
			FindPath();
			break;
		case 3:
			SaveMaze();
			break;
		case 4:
			LoadMaze();
			break;
		case 5:
			FreeMaze();
			break;
		}
	} while (option != 5);
}

int main()
{
	srand((unsigned)time(NULL));
	MainMenu();
	return 0;
}
