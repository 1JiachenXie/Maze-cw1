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


static const int BLANK = 0;
static const int WALL = 1;
static const int START = 2;
static const int END = 3;
static const int STEP = 4;
static const int down_move = 101;
static const int right_move = 102;
static const int left_move = 103;
static const int up_move = 104;
static int weight_w = 10;
static int row = 0;
static int col = 0;

struct Squre
{
	int x, y, direction;
	Squre(int n, int m, int dir)
	{
		x = n;
		y = m;
		direction = dir;
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
	int real_value; //起始点到当前点实际代价
	int h_value;//当前节点到目标节点最佳路径的估计代价
	int f_value;//估计值
	Node* parent;
	Node(int x_pos, int y_pos)
	{
		this->x_pos = x_pos;
		this->y_pos = y_pos;
		this->real_value = 0;
		this->h_value = 0;
		this->f_value = 0;
		this->parent = NULL;
	}
	Node(int x_pos, int y_pos, Node* parent)
	{
		this->x_pos = x_pos;
		this->y_pos = y_pos;
		this->real_value = 0;
		this->h_value = 0;
		this->f_value = 0;
		this->parent = parent;
	}
};

class AstarAlgorithm
{
public:
	AstarAlgorithm();
	~AstarAlgorithm();
	void SearchShortestPath(Node* start_pos, Node* end_pos);
	bool CheckPath(Node* start_pos, Node* end_pos);
	void CheckStep(int x_pos, int y_pos, Node* parent, int real_value);
	void GetNextStep(Node* cur_point);
	int CheckContains(vector<Node*>* node_list, int x_pos, int y_pos);
	void CountGHF(Node* s_node, Node* e_node, int real_value);
	static bool Compare(Node* n1, Node* n2);
	bool CheckBeWall(int x_pos, int y_pos);
	void GetPath(Node* current);
	vector<Node*> open_list;
	vector<Node*> close_list;
	Node *start_pos;
	Node *end_pos;
};

vector<Squre> squre;
int xpos = 1, y_pos = 1;
int** maze = nullptr;
int maze_w = 0;
int maze_h = 0;
int end_pos = 0;
vector<point> end_points;

void ResetMaze()
{
	for (int i = 0; i < maze_h; i++)
		for (int j = 0; j < maze_w; j++)
			maze[i][j] = WALL;
}

void InitSqure()
{
	end_points.clear();
	ResetMaze();
	maze[1][1] = BLANK;
}

void GetNextSqure()
{
	//down
	if (xpos + 1 < maze_h - 1 && maze[xpos + 1][y_pos] == WALL) 
	{
		squre.push_back(Squre(xpos + 1, y_pos, down_move));
	}
	//right
	if (y_pos + 1 < maze_w - 1 && maze[xpos][y_pos + 1] == WALL) 
	{
		squre.push_back(Squre(xpos, y_pos + 1, right_move));
	}
	//up
	if (xpos - 1 >= 1 && maze[xpos - 1][y_pos] == WALL)
	{
		squre.push_back(Squre(xpos - 1, y_pos, up_move));
	}
	//left
	if (y_pos - 1 >= 1 && maze[xpos][y_pos - 1] == WALL) 
	{
		squre.push_back(Squre(xpos, y_pos - 1, left_move));
	}
}

//	清空输入缓存
void ClearQueue()
{
	while (getchar() != '\n')
		continue;
}

//	确认输入的是整数
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

//	确认输入的是奇数
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

//	输入一个范围内的整数
int InputIntInRange(char const * msg, int l, int r)
{
	int j;
	while (1)
	{
		j = InputInt(msg);
		if (j<l || j>r)
		{
			cout << "Please enter an integer between "<< l <<" and "<< r <<"！\n";
			continue;
		}
		else
			break;
	}
	return j;
}

//	确认输入字符串
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

// 释放迷宫数组内存
void FreeMaze()
{
	for (int i = 0; i < maze_h; i++)
	{
		delete[] maze[i];
	}
	delete[]maze;
	maze = nullptr;
}


void PrintMaze()
{
	for (int i = 0; i < maze_h; i++)
	{
		for (int j = 0; j < maze_w; j++)
		{
			if (maze[i][j] == WALL)
				printf("X");
			else if (maze[i][j] == START)
				printf("S");
			else if (maze[i][j] == END)
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

// 设置终点和优化迷宫
bool SetStartAndEnd()
{
	int center_posx = maze_h / 2;
	int center_posy = maze_w / 2;
	int i = 1, j = maze_h - 1, k = 1, l = maze_h - 1;
	// 中间3*3的地方必须是空白的
	for (i = center_posx - 1; i < center_posx + 2; i++)
	{
		for (j = center_posy - 1; j < center_posy + 2; j++)
		{
			maze[i][j] = BLANK;
		}
	}
	//外围必须都是围墙
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
	AstarAlgorithm astar;
	Node *start_pos = new Node(start.x, start.y);
	row = maze_h - 1;
	col = maze_w - 1;
	maze[center_posx][center_posy] = START;
	int num_end = 0;
	i = 1;
	j = maze_h - 1;
	k = 1;
	l = maze_h - 1;
	//找终点 
	while (num_end < end_pos)
	{
		for (; i < maze_w; i++)
		{
			if (maze[1][i] == BLANK && maze[0][i] == WALL)
			{
				Node *end_pos = new Node(0, i);
				//if (astar.CheckPath(start_pos, end_pos))
				{
					point pt;
					pt.x = 0;
					pt.y = i;
					end_points.push_back(pt);
					maze[0][i] = END;
					num_end++;
					break;
				}
			}
		}
		for (; j > 0 && num_end < end_pos; j--)
		{
			if (maze[j][1] == BLANK && maze[j][0] == WALL)
			{
				Node *end_pos = new Node(j, 0);
				//if (astar.CheckPath(start_pos, end_pos))
				{
					point pt;
					pt.x = j;
					pt.y = 0;
					end_points.push_back(pt);
					maze[j][0] = END;
					num_end++;
					break;
				}
			}
		}
		for (; k < maze_w && num_end < end_pos; k++)
		{
			if (maze[maze_h - 2][k] == BLANK && maze[maze_h - 1][k] == WALL)
			{
				Node *end_pos = new Node(maze_h - 1, k);
				//if (astar.CheckPath(start_pos, end_pos))
				{
					point pt;
					pt.x = maze_h - 1;
					pt.y = k;
					end_points.push_back(pt);
					maze[maze_h - 1][k] = END;
					num_end++;
					break;
				}
			}
		}
		for (; l > 0 && num_end < end_pos; l--)
		{
			if (maze[l][maze_w - 2] == BLANK && maze[l][maze_w - 1] == WALL)
			{
				Node *end_pos = new Node(l, maze_w - 1);
				//if (astar.CheckPath(start_pos, end_pos))
				{
					point pt;
					pt.x = l;
					pt.y = maze_w - 1;
					end_points.push_back(pt);
					maze[l][maze_w - 1] = END;
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
	InitSqure();
	GetNextSqure();

	while (squre.size()) 
	{
		int squre_size = squre.size();
		int randnum = rand() % squre_size;
		Squre select_squre = squre[randnum];
		xpos = select_squre.x;

		y_pos = select_squre.y;

		switch (select_squre.direction) 
		{
		case down_move:
			xpos++;
			break;

		case right_move:
			y_pos++;
			break;

		case left_move:
			y_pos--;
			break;

		case up_move:
			xpos--;
			break;
		}
		//目标块如果是墙
		if (maze[xpos][y_pos] == WALL) 
		{
			//打通墙和目标块
			maze[select_squre.x][select_squre.y] = maze[xpos][y_pos] = BLANK;
			//再次找出与矿工当前位置相邻的墙
			GetNextSqure();
		}

		//删除这堵墙(把用不了的墙删了，对于那些已经施工过了不必再施工了，同时也是确保我们能跳出循环)
		squre.erase(squre.begin() + randnum);
	}
	return SetStartAndEnd();
}


AstarAlgorithm::AstarAlgorithm()
{
}
AstarAlgorithm::~AstarAlgorithm()
{
}

void AstarAlgorithm::SearchShortestPath(Node* start_pos, Node* end_pos)
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
			GetPath(current);
			open_list.clear();
			close_list.clear();
			break;
		}
		GetNextStep(current);
		close_list.push_back(current);
		open_list.erase(open_list.begin());
		sort(open_list.begin(), open_list.end(), Compare);
	}
}

bool AstarAlgorithm::CheckPath(Node* start_pos, Node* end_pos)
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
		GetNextStep(current);
		close_list.push_back(current);
		open_list.erase(open_list.begin());
		sort(open_list.begin(), open_list.end(), Compare);
	}
	return false;
}

void AstarAlgorithm::CheckStep(int x_pos, int y_pos, Node* parent, int real_value)
{
	if (x_pos < 0 || x_pos > row || y_pos < 0 || y_pos > col)
		return;
	if (this->CheckBeWall(x_pos, y_pos))
		return;
	if (CheckContains(&close_list, x_pos, y_pos) != -1)
		return;
	int index;
	if ((index = CheckContains(&open_list, x_pos, y_pos)) != -1)
	{
		Node *point = open_list[index];
		if (point->real_value > parent->real_value + real_value)
		{
			point->parent = parent;
			point->real_value = parent->real_value + real_value;
			point->f_value = point->real_value + point->h_value;
		}
	}
	else
	{
		Node * point = new Node(x_pos, y_pos, parent);
		CountGHF(point, end_pos, real_value);
		open_list.push_back(point);
	}
}

void AstarAlgorithm::GetNextStep(Node* current)
{
	CheckStep(current->x_pos - 1, current->y_pos, current, weight_w);//左
	CheckStep(current->x_pos + 1, current->y_pos, current, weight_w);//右
	CheckStep(current->x_pos, current->y_pos + 1, current, weight_w);//上
	CheckStep(current->x_pos, current->y_pos - 1, current, weight_w);//下
}

int AstarAlgorithm::CheckContains(vector<Node*>* node_list, int x_pos, int y_pos)
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

void AstarAlgorithm::CountGHF(Node* s_node, Node* e_node, int real_value)
{
	int h_value = abs(s_node->x_pos - e_node->x_pos) * weight_w + abs(s_node->y_pos - e_node->y_pos) * weight_w;
	int currentg = s_node->parent->real_value + real_value;
	int f_value = currentg + h_value;
	s_node->f_value = f_value;
	s_node->h_value = h_value;
	s_node->real_value = currentg;
}


bool AstarAlgorithm::Compare(Node* n1, Node* n2)
{
	return n1->f_value < n2->f_value;
}

bool AstarAlgorithm::CheckBeWall(int x_pos, int y_pos)
{
	if (maze[x_pos][y_pos] == WALL)
		return true;
	return false;
}

void AstarAlgorithm::GetPath(Node* current)
{
	if (current->parent != NULL)
		GetPath(current->parent);
	if (maze[current->x_pos][current->y_pos] == BLANK)
		maze[current->x_pos][current->y_pos] = STEP;
}


void FindShortestPath()
{
	AstarAlgorithm astar;
	Node *start_pos = new Node(start.x, start.y);
	for (int i = 0; i < end_points.size(); i++)
	{
		Node *end_pos = new Node(end_points[i].x, end_points[i].y);
		astar.SearchShortestPath(start_pos, end_pos);
		cout << endl;
		PrintMaze();
	}
}

// 生成迷宫
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
	ResetMaze();
	end_pos = InputInt("Number of exits:");
	while (CreateMaze() == false)
	{
		ResetMaze();
	}
	PrintMaze();
}

// 保存迷宫
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
			else if (maze[i][j] == START)
				outfile << "S";
			else if (maze[i][j] == END)
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

// 加载迷宫
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
					maze[i][j] = START;
				}
				else if (ch == 'E')
				{
					maze[i][j] = END;
				}
				else if (ch == ' ' || ch == 'o')
				{
					maze[i][j] = BLANK;
				}

				if (maze[i][j] == START)
				{
					start.x = i;
					start.y = j;
				}
				else if (maze[i][j] == END)
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
	PrintMaze();
}

void FindPath()
{
	row = maze_h - 1;
	col = maze_w - 1;
	FindShortestPath();
}

// 程序主界面
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
