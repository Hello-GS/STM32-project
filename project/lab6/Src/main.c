/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include <time.h>
#include "snakeImage.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define SNAKE_Max_Long 50//
#define N 10
uint8_t eating = 0;
uint8_t size = 10;
uint8_t press = 0;
uint8_t f_delay;
uint8_t s_delay;
int colorCount = 1;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//蛇结构体
struct Snake
{
	int X[SNAKE_Max_Long];
	int Y[SNAKE_Max_Long];
	uint8_t Long;//蛇的长度
	uint8_t Direction;//蛇移动的方向
	uint16_t color;
}snake;

//食物结构�??
struct Food
{
	uint8_t X;//
	uint8_t Y;//食物右上
	uint8_t state;//判断 1是存�?? 0是不存在
	uint8_t time;
	uint16_t color;
}food;

struct Stone
{
	uint8_t X;//
	uint8_t Y;//
	uint8_t state;//判断 1是存�?? 0是不存在
	uint8_t time;
	uint16_t color;
}stone;

//游戏等级分数
struct Game
{
	int Score;//分数
	int level;//游戏等级
	uint8_t life;//game结束
}game;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
uint8_t rxBuffer[20];

void SystemClock_Config(void);


/* USER CODE BEGIN PFP */
void generate();//生成食物和石�??
uint8_t overlapping(int a);//判断是否重叠�??1是食物�??0是石�??
void gameover();//游戏结束
void eat();//判断是否吃什�??
void start();//游戏初始�??
void gameJudge();//判断游戏是否结束
void up();
//UI
void showScore();
void InitialPlane();
void showSnake();
void showBlock(int x, int y,uint16_t color);
void showScore();
void showEnd();
uint16_t randomColor();
void lcd_DrawPicture(uint8_t *pic);
void showlevel();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t i;

	//uint8_t move;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  LCD_Init();
  srand(time(NULL));

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
  HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
  start();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //lab1 : ;
  while (1)
  {
	  while(!food.state || !stone.state){
	 		  generate();
	 }

	  showBlock(snake.X[snake.Long-1],snake.Y[snake.Long-1],WHITE);

	  for(i=snake.Long-1;i>0;i--){
			  snake.X[i]=snake.X[i-1];
			  snake.Y[i]=snake.Y[i-1];
	}
	  switch(snake.Direction)
	  {
		case 1:snake.Y[0] -= 10;break;//向up运动
		case 2:snake.X[0] -= 10;break;//向right运动
		case 3:snake.Y[0] += 10;break;//向down运动
		case 4:snake.X[0] += 10;break;//向left运动
		default: break;
	  }
	  press = 1;
	  eat();
	  gameJudge();
	  showSnake();
	  if(!f_delay){
		  if(food.state)
		 		showBlock(food.X,food.Y,food.color);
	  }else
 		  f_delay -= 1;

	  if(!s_delay){
		  if(stone.state)
		  		  showBlock(stone.X,stone.Y,stone.color);
	  }
	  else
		  s_delay -= 1;


	  showScore();

	  switch(game.level)
	 {
		case 1: HAL_Delay(400);break;
		case 2: HAL_Delay(320);break;
		case 3: HAL_Delay(270);break;
		default: HAL_Delay(200);break;
	 }
	  if(eating){
		  HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
		  eating = 0;
	 }
	  if(game.life){
	  		food.time ++;
	  		stone.time++;
	  }else{
	  		gameover();
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	HAL_Delay(150);
	switch (GPIO_Pin) {
		case KEY0_Pin:
			if(press){
				switch (snake.Direction)
				{
					case 1: snake.Direction = 4;break;
					case 2: snake.Direction = 1;break;
					case 3: snake.Direction = 4;break;
					case 4: snake.Direction = 3;break;
					default: break;
				}
				press = 0;
			}
			break;
		case KEY1_Pin:
			if(press){
				switch (snake.Direction)
				{
					case 1: snake.Direction = 2;break;
					case 2: snake.Direction = 3;break;
					case 3: snake.Direction = 2;break;
					case 4: snake.Direction = 1;break;
					default: break;
				}
				press = 0;
			}
			break;
		case KEY_WK_Pin:
			HAL_Delay(2000);
			break;
		default:
			break;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */



void start(){
	//time = 0;
	lcd_DrawPicture(Isnake);
	snake.X[0] = 125;
	snake.Y[0] = 185;
	snake.X[1] = 125;
	snake.Y[1] = 175;
	snake.Direction = 1;
	snake.Long = 2;
	food.state = 0;
	stone.state = 0;
	snake.color = BLUE;
	food.color = GREEN;
	stone.color = BLACK;
	while (1){
		food.X = (rand()%20)*size + 15;
		food.Y = (rand()%25)*size + 45;
		if(!overlapping(1))
			break;
	}
	food.state = 1;
	while(1){
		stone.X = (rand()%20)*size + 15;
		stone.Y = (rand()%25)*size + 45;
		if(!overlapping(0))
			break;
	}
	stone.state = 1;
	food.time = 0;
	stone.time = 0;
	game.Score = 0;
	game.level = 0;
	game.life = 1;
	game.level = 1;
	HAL_Delay(1000);
	LCD_Clear(WHITE);
	InitialPlane();
	f_delay = 0;
	s_delay = 0;
}

void generate(){
	while(!food.state){
		food.X = (rand()%20) * size + 15;
		food.Y = (rand()%25) * size + 45;
		if(!overlapping(1)){
			food.color = randomColor();
			food.time = 0;
			food.state = 1;
			break;
		}
	}
	while(!stone.state){
		stone.X = (rand()%20)* size + 15;
		stone.Y = (rand()%25)* size + 45;
		if(!overlapping(0)){
			stone.time = 0;
			stone.state = 1;
			break;
		}
	}
}

uint8_t overlapping(int a){
	if (a == 1){
		for(uint8_t i=0;i<snake.Long;i++)
		{
			if(snake.X[i]==food.X &&snake.Y[i]==food.Y )//自身的任�??坐标值与蛇头坐标相等就认为是自身碰撞
				return 1;
		}
		if(stone.state){
			if (food.X == stone.X && food.Y == stone.Y)
				return 1;
		}

	}else
	{
		for(uint8_t i=0;i<snake.Long;i++)
		{
			if(snake.X[i]==stone.X &&snake.Y[i]==stone.Y)//自身的任�??坐标值与蛇头坐标相等就认为是自身碰撞
				return 1;
		}
		if(food.state){
			if (food.X == stone.X && food.Y == stone.Y)
				return 1;
		}
	}
	return 0;
}

void up(){
	showlevel();
	HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
	HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
	HAL_Delay(100);
	HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
	HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
	HAL_Delay(100);
	HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
	HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
	HAL_Delay(100);
	HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
	HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
	HAL_Delay(100);
	LCD_Clear(WHITE);
	InitialPlane();
}
//KEY1_Pin �? KEY2_Pin �? KEY_WK_Pin 暂停
//Direction 1�? 2�? 3�? 4�?


void eat(){
	if(snake.X[0]==food.X&&snake.Y[0]==food.Y)
	{
		if(snake.Long < SNAKE_Max_Long)
			snake.Long++;//蛇的身体长一�??
		game.Score += 10;
		eating = 1;
		HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
		snake.color = food.color;
		food.state = 0;
	}
	if(snake.X[0]==stone.X && snake.Y[0]==stone.Y){
		game.life = 0;
	}
	//判断是否出界
	if(snake.X[0]<= 5 || snake.X[0] >= 235 || snake.Y[0]<= 35 || snake.Y[0] >= 315)
		game.life = 0;

	//是否吃了自身
	for(uint8_t i=3;i<snake.Long;i++)
	{
		if(snake.X[i]==snake.X[0]&&snake.Y[i]==snake.Y[0])//自身的任�??坐标值与蛇头坐标相等就认为是自身碰撞
			game.life = 0;
	}

}

void gameover(){
	HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
	showEnd();
	HAL_Delay(1000);
	HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
}

void gameJudge(){
	if (game.level == 1 && game.Score > 20){
		game.level = 2;
		up();
	}
	if (game.level == 2 && game.Score > 40){
		game.level = 3;
		up();
	}
	if (food.time > 100){
		food.state = 0;
		f_delay = 3;
		showBlock(food.X, food.Y,WHITE);
	}
	if (stone.time > 100){
		stone.state = 0;
		s_delay = 3;
		showBlock(stone.X, stone.Y,WHITE);
	}
}

void showEnd(){
	LCD_Clear(BLACK);
	LCD_Fill(0,0,8,280,DARKBLUE);
	LCD_Fill(232,0,240,280,DARKBLUE);
	LCD_Fill(0,0,240,8,DARKBLUE);
	LCD_Fill(0,272,240,280,DARKBLUE);
	LCD_ShowString(60,100,240,24,24,"GAME OVER! ");
	LCD_ShowString(70,140,240,24,24,"score: ");
	LCD_ShowNum (145,140,game.Score,3,24);
	while(1){}
}

void InitialPlane()
{
	LCD_Set_Window(0, 0, 240, 320);
	LCD_DrawRectangle(4, 34, 236, 316); // Draw a rectangle
	LCD_DrawRectangle(2, 32, 238, 318);

	LCD_Fast_DrawPoint(2, 32, WHITE);
	LCD_Fast_DrawPoint(2, 318, WHITE);
	LCD_Fast_DrawPoint(238, 32, WHITE);
	LCD_Fast_DrawPoint(238, 318, WHITE);
}

void showScore()
{

	LCD_ShowString(70,3,240,24,24,"SCORE: ");
	LCD_ShowNum (145,3,game.Score,3,24);
}

// 画出蛇的形状，两个数组分别为左上角的坐标，lenx是数组的长度通过sizeof(a)/sizeof(a[0]获得
void showSnake()
{
	int lenx = snake.Long;
	showBlock(snake.X[0], snake.Y[0],BLUE);
	for (int i = 1; i < lenx; i++)
	{
		showBlock(snake.X[i], snake.Y[i],snake.color);
	}
}

// 画出�?/食物/石头的每个节点， color是每个节点的颜色
void showBlock(int x, int y, uint16_t color)
{
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			LCD_Fast_DrawPoint(x + i, y + j, color);
		}
	}
	//与背景颜色相同即可
	LCD_Fast_DrawPoint(x + 4, y + 4, WHITE);
	LCD_Fast_DrawPoint(x + 4, y + 5, WHITE);
	LCD_Fast_DrawPoint(x + 5, y + 4, WHITE);
	LCD_Fast_DrawPoint(x + 5, y + 5, WHITE);

	LCD_Fast_DrawPoint(x, y, WHITE);
	LCD_Fast_DrawPoint(x + 1, y, WHITE);
	LCD_Fast_DrawPoint(x, y + 1, WHITE);

	LCD_Fast_DrawPoint(x + 9, y, WHITE);
	LCD_Fast_DrawPoint(x + 9, y, WHITE);
	LCD_Fast_DrawPoint(x + 9, y + 1, WHITE);

	LCD_Fast_DrawPoint(x, y + 9, WHITE);
	LCD_Fast_DrawPoint(x, y + 8, WHITE);
	LCD_Fast_DrawPoint(x + 1, y + 9, WHITE);

	LCD_Fast_DrawPoint(x + 9, y + 9, WHITE);
	LCD_Fast_DrawPoint(x + 8, y + 9, WHITE);
	LCD_Fast_DrawPoint(x + 9, y + 8, WHITE);
}


uint16_t randomColor()
{
	colorCount = (colorCount+1)%10;
	switch (colorCount)
	{
	case 0:
		return GREEN;
	case 1:
		return YELLOW;
	case 2:
		return BROWN;
	case 3:
		return BRRED;
	case 4:
		return GRAY;
	case 5:
		return RED;
	case 6:
		return LIGHTGREEN;
	case 7:
		return DARKBLUE;
	case 8:
		return GRAYBLUE;
	case 9:
		return  LGRAYBLUE;
	}
}

void lcd_DrawPicture(uint8_t *pic)
{
	uint32_t i = 8;
	uint16_t temp;
	uint32_t index = 0;
	uint32_t totalpoint = 240;
	totalpoint *= 320;			 //得到总点数
	LCD_SetCursor(0x00, 0x0000); //设置光标位置
	LCD_WriteRAM_Prepare();		 //开始写入GRAM
	for (index = 0; index < totalpoint; index++)
	{
		temp = (uint16_t)(pic[i] << 8) + pic[i + 1]; //16位总线， 须要一次发送2个字节的数据
		LCD_WR_DATA(temp);							 //将取出的16位像素数据送入显示区
		i = i + 2;									 //取模位置加2，以为获取下一个像素数据
	}
}

void showlevel(){
    LCD_Clear(BLACK);
    LCD_Fill(0,0,8,280,DARKBLUE);
    LCD_Fill(232,0,240,280,DARKBLUE);
    LCD_Fill(0,0,240,8,DARKBLUE);
    LCD_Fill(0,272,240,280,DARKBLUE);
    LCD_ShowString(60,100,240,24,24,"Hello! ");
    LCD_ShowString(70,140,240,24,24,"Round: ");
    LCD_ShowNum (145,140,game.level,3,24);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
