#include <iostream>
#include <windows.h>
#include<conio.h>
#include<vector>

using namespace std;

#pragma region STRUCTs
struct SnakeInfo
{
	int length;
	int x;
	int y;
	char head;
	char movingDirection;
	int score;
};

struct FoodInfo
{
	int foodLocationX;
	int foodLocationY;
	char foodSymbol;
};
#pragma	endregion

#pragma region VARs
bool gameOver = false; 
const int HEIGHT = 20;
const int WIDTH = 40;
char lastTouch = 'w';
int prevBodyX = 0; //Initialize the body position of the first frame
int prevBodyY = 0;
int prevHeadX = 0; //Initialize the head position of the first frame
int prevHeadY = 0;
vector<int> tempX(1);//Define a set of one-dimensional arrays, initialize at least one value, here is to maintain the change of the head position of each frame (move one pixel per frame, so the head also needs to move one pixel per frame)
vector<int> tempY(1);

FoodInfo foodActual; // 实例化结构体FoodInfo，使用food来调用 Instance structure FoodInfo, use food to call
SnakeInfo snake; //实例化结构体SnakeInfo，使用snake来调用 Instance structure SnakeInfo, use snake to call

#pragma	endregion

void SetObjectLocation(int x, int y)
{
	//获取屏幕坐标 Get the screen coordinates
	COORD coord;
	coord.X = x; //形式参数值传递到实际参数 Formal parameter x value is passed to actual parameter
	coord.Y = y; //形式参数值传递到实际参数 Formal parameter x value is passed to actual parameter
	//设置到实际连接的显示器上
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

}

void SpawnFood(FoodInfo& food) //&代表引用传递而非值传递 & means reference transfer rather than value transfer
{
	srand(static_cast<unsigned int>(time(0))); //影响rand()值 

	//随机数结果除以边长-2取余数获取范围内随机值
	food.foodLocationX = rand() % (WIDTH - 2) + 1;
	food.foodLocationY = rand() % (HEIGHT - 2) + 1;
	//在屏幕上显示这两个数值
	SetObjectLocation(food.foodLocationX, food.foodLocationY);
	cout << food.foodSymbol;
}

void Initialization() 
{
	//定义玩家初始属性，初始位置
	snake.length = 1;
	snake.score = 0;
	snake.x = WIDTH / 2;
	snake.y = HEIGHT / 2;
	snake.head = '0';
	/*snake.movingDirection 从键盘直接读取，此时不初始化*/
	foodActual.foodSymbol = 'f';

	
	for (int i = 0; i < WIDTH; i++)
	{
		cout << "*"; 
	}
	cout << endl;

	for (int i = 0; i < HEIGHT; i++)
	{
		
		for (int j = 0; j < WIDTH; j++) 
		{
			if (j == 0 or j == WIDTH - 1)
			{
				cout << "*"; 
			}
			else
			{
				cout << " "; 
			}
		}
		cout << endl; 
	};

	for (int i = 0; i < WIDTH; i++)
	{
		cout << "*";
	}
	cout << endl;

	//初始时默认游戏区域内随机位置生成一次食物
	SpawnFood(foodActual);//传入实际参数

}

void GameMode()
{	
	if ((snake.x == 0 or snake.x == WIDTH - 1) or (snake.y == 0 or snake.y == HEIGHT + 1))
	{
		gameOver = true;

		SetObjectLocation(30, 22);
		cout << "游戏结束";
	}
	else if (snake.x == foodActual.foodLocationX and snake.y == foodActual.foodLocationY)
	{
		snake.length += 1;
		snake.score += 10;
		SpawnFood(foodActual);
		tempX.push_back(int());//push_back从末位添加元素
		tempY.push_back(int());
	}
}

void PlayerController()
{
	//玩家控制器
	// keyboard hit键盘按键事件
	if (_kbhit())
	{
		//定义局部变量touch，字符类型(因为键盘输入的就是字符类型变量)，负责接收键盘输入响应
		char touch = _getch();

		//通过读取键盘输入并赋值给snake.movingDirection这个结构体属性来定义角色移动（需要搭配后续改变蛇坐标的代码块）
		//分情况讨论，角色向上移动
		if (touch == 'w' and snake.movingDirection != 's')
		{
			snake.movingDirection = 'w';
			lastTouch = 'w';
		}
		//分情况讨论，角色向下移动
		else if (touch == 's' and snake.movingDirection != 'w')
		{
			snake.movingDirection = 's';
			lastTouch = 's';
		}
		//分情况讨论，角色向左移动
		else if (touch == 'a' and snake.movingDirection != 'd')
		{
			snake.movingDirection = 'a';
			lastTouch = 'a';
		}
		//分情况讨论，角色向右移动
		else if (touch == 'd' and snake.movingDirection != 'a')
		{
			snake.movingDirection = 'd';
			lastTouch = 'd';
		}
		//其他情况保持原先的方向（读取原输入值）
		else
		{
			snake.movingDirection = lastTouch;
		}
	}

	//定义蛇本身如何移动
	//向上
	if (snake.movingDirection == 'w') snake.y--;
	//向下
	else if (snake.movingDirection == 's') snake.y++;
	//向左
	else if (snake.movingDirection == 'a') snake.x--;
	//向右
	else if (snake.movingDirection == 'd') snake.x++;
	else return;
	
}

void Tick()
{
	Sleep(200); //休眠100ms

	//由于蛇尾位置一直在变，需要按照当前蛇的长度循环搜索蛇全身的位置信息直到最后一位（也就是后方为空格的一位）
	for (int i = 0; i < snake.length; i++)
	{
		//遍历蛇的整个身体找到尾
		prevBodyX = tempX[i];
		prevBodyY = tempY[i];

		tempX[i] = prevHeadX;
		tempY[i] = prevHeadY;
		//值交换，把欲清楚的prevBody位置信息传回prevHead
		//假如跳出for循环，prevHead的值会被后面的逻辑重新赋值prevHeadX=tempX[0]
		//假如继续for循环，执行下面的逻辑把prevHead的信息继续交换给后面的身体部分
		//判断是否跳出for循环的标准是snake.length也就是说直到遍历整个蛇的长度以前都不会跳出循环
		prevHeadX = prevBodyX;
		prevHeadY = prevBodyY;
	}

	//清除上一帧的符号，注意在刷新这一帧之前进行
	//找到且仅清楚蛇尾最后一位
	SetObjectLocation(prevBodyX, prevBodyY);
	cout << " "; //刷新为空格

	SetObjectLocation(snake.x, snake.y);
	cout << snake.head; //挪动光标位置到新坐标，还没有清除尾部坐标
	//值交换，刷新后立刻把头部位置存入prevHead变量
	prevHeadX = tempX[0];
	prevHeadY = tempY[0];
	//把结构体数值存入变量
	tempX[0] = snake.x;
	tempY[0] = snake.y;
}


void UserInterface()
{
	SetObjectLocation(0, HEIGHT + 2);
	cout << "得分：" << snake.score << endl;
	cout << "长度：" << snake.length << endl;
}

int main()
{
	
	Initialization();

	
	while (!gameOver)
	{
		GameMode();
		PlayerController();
		Tick();
		UserInterface();
	}

	return 0;
}