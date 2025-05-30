/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include <string.h>
#include <stdio.h>
#include "adc.h"
#include "dac.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_BUF_LEN  256
#define ADC_BUF_HALF_LEN 128
#define TRAIT_BUF_LEN 1024
#define TRAIT_BUF_HALF_LEN 512
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern float volume1;
extern float volume2;
extern double veff;
uint16_t position_rect1=100,position_rect2=100,position_rect1_old=100,position_rect2_old=100;
uint32_t entree1=0, entree2=0;
extern uint16_t sortie;
extern uint16_t adc1_buffer[ADC_BUF_LEN];
extern uint16_t adc3_buffer[ADC_BUF_LEN];
extern uint16_t dac_buf[ADC_BUF_LEN];
extern uint16_t traitement_buffer[TRAIT_BUF_LEN];
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId AffichageHandle;
osThreadId Aquisition_ADCHandle;
osThreadId ToucheHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
double calculer_valeur_efficace(const uint16_t* tableau, int taille);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void Tache_Affiche(void const * argument);
void Tache_Acquisition(void const * argument);
void Tache_touche(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of Affichage */
  osThreadDef(Affichage, Tache_Affiche, osPriorityIdle, 0, 2048);
  AffichageHandle = osThreadCreate(osThread(Affichage), NULL);

  /* definition and creation of Aquisition_ADC */
  osThreadDef(Aquisition_ADC, Tache_Acquisition, osPriorityHigh, 0, 256);
  Aquisition_ADCHandle = osThreadCreate(osThread(Aquisition_ADC), NULL);

  /* definition and creation of Touche */
  osThreadDef(Touche, Tache_touche, osPriorityIdle, 0, 128);
  ToucheHandle = osThreadCreate(osThread(Touche), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Tache_Affiche */
/**
* @brief Function implementing the Affichage thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Tache_Affiche */
void Tache_Affiche(void const * argument)
{
  /* USER CODE BEGIN Tache_Affiche */
	char text[60];
  /* Infinite loop */
  for(;;)
  {
	  BSP_LCD_SelectLayer(1);
	  if(position_rect2!=position_rect2_old){
		  BSP_LCD_SetTextColor(0);
		  BSP_LCD_FillRect(114,position_rect2_old,20,40);
		  BSP_LCD_SetTextColor(LCD_COLOR_LIGHTMAGENTA);
		  BSP_LCD_FillRect(114,position_rect2,20,40);
	  }

	  if(position_rect1!=position_rect1_old){
		  BSP_LCD_SetTextColor(0);
		  BSP_LCD_FillRect(340,position_rect1_old,20,40);
		  BSP_LCD_SetTextColor(LCD_COLOR_LIGHTMAGENTA);
		  BSP_LCD_FillRect(340,position_rect1,20,40);
	  }

//	  BSP_LCD_SetTextColor(0);
//	  BSP_LCD_FillRect(300,20,80,20);
//	  if (veff>1200){
//		  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
//		  BSP_LCD_FillRect(340,20,40,20);
//		  if (veff>1200){
//
//		  		  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
//		  		  BSP_LCD_FillRect(340,20,40,20);
//		  }
//		  if (veff>1500){
//			  BSP_LCD_SetTextColor(LCD_COLOR_RED);
//			  BSP_LCD_FillRect(300,20,40,20);
//		  }
//	  }




    osDelay(2);
  }
  /* USER CODE END Tache_Affiche */
}

/* USER CODE BEGIN Header_Tache_Acquisition */
/**
* @brief Function implementing the Aquisition_ADC thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Tache_Acquisition */
void Tache_Acquisition(void const * argument)
{
  /* USER CODE BEGIN Tache_Acquisition */
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	//uint32_t entree1, entree2;
	uint16_t i;
  /* Infinite loop */
  for(;;)
  {
	  HAL_GPIO_TogglePin(LED13_GPIO_Port,LED13_Pin);
	  veff = calculer_valeur_efficace(traitement_buffer, TRAIT_BUF_LEN);
    osDelay(2);
  }
  /* USER CODE END Tache_Acquisition */
}

/* USER CODE BEGIN Header_Tache_touche */
/**
* @brief Function implementing the Touche thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Tache_touche */
void Tache_touche(void const * argument)
{
  /* USER CODE BEGIN Tache_touche */
	static TS_StateTypeDef  TS_State;
	uint16_t hmax=230,vmax=230,hmin=75;
  /* Infinite loop */
  for(;;)
  {

		BSP_TS_GetState(&TS_State);
		if(TS_State.touchDetected){

			position_rect1_old = position_rect1;
			position_rect2_old = position_rect2;

			if (TS_State.touchX[0]>230){


				position_rect1= TS_State.touchY[0];

				if (position_rect1 > hmax){position_rect1=hmax;}
				if (position_rect1 < hmin){position_rect1=hmin;}

				volume1 = hmax - position_rect1;
				volume1 = volume1/(hmax-hmin);


			}
			else{
				position_rect2= TS_State.touchY[0];

				if (position_rect2 > hmax){position_rect2=hmax;}
				if (position_rect2 < hmin){position_rect2=hmin;}

				volume2 = hmax - position_rect2;
				volume2 = volume2/(hmax-hmin);

			}
		}
    osDelay(1);
  }

  /* USER CODE END Tache_touche */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
double calculer_valeur_efficace(const uint16_t* tableau, int taille) {
    double somme_carres = 0.0;
    for (int i = 0; i < taille; ++i) {
        somme_carres += (double)tableau[i] * (double)tableau[i];
    }
    return sqrt(somme_carres / taille);
}
/* USER CODE END Application */

