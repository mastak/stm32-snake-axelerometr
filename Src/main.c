/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "bmp280.h"
#include "MPU9250.h"
#include "MadgwickAHRS.h"
#include "snake.h"

#define abs(x)  (x < 0) ? -x : x
#define max(x, y)  x > y ? x : y
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
double temp, press, alt;
int8_t com_rslt;
float ax, ay, az;
float gx, gy, gz;
float mx, my, mz;

uint32_t lastTime, currentTime, drawTime;

volatile Direction current_direction = BOTTOM;
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void set_direction(float ax, float ay);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void I2C_ScanBus(void);
/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
    SnakePoint *SnakeHead = NULL;
    SnakePoint *SnakeTail = NULL;

    SnakePoint *tmp_snake = NULL;

    uint32_t next_render_tick = 0;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART3_UART_Init();

  /* USER CODE BEGIN 2 */
  LCD_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  I2Cdev_init(&hi2c1);

  I2C_ScanBus();
  LCD_Printf("Connecting to BMP280...\n");
  bmp280_t bmp280;
  com_rslt = BMP280_init(&bmp280);
  com_rslt += BMP280_set_power_mode(BMP280_NORMAL_MODE);
  if (com_rslt != SUCCESS) {
    LCD_Printf("1Check BMP280 connection!\nProgram terminated");
    return 0;
  }
  com_rslt += BMP280_set_work_mode(BMP280_STANDARD_RESOLUTION_MODE);
  if (com_rslt != SUCCESS) {
    LCD_Printf("2Check BMP280 connection!\nProgram terminated");
    return 0;
  }
  com_rslt += BMP280_set_standby_durn(BMP280_STANDBY_TIME_1_MS);
  if (com_rslt != SUCCESS) {
    LCD_Printf("3Check BMP280 connection!\nProgram terminated");
    return 0;
  }
  LCD_Printf("Connection successful!\n");
  
  LCD_Printf("Connecting to MPU9250...\n");
  while(!MPU9250_testConnection());
  MPU9250_initialize();
  MPU9250_setFullScaleGyroRange(MPU9250_GYRO_FS_1000); // 1000 град/сек
  MPU9250_setFullScaleAccelRange(MPU9250_ACCEL_FS_4); // 4g
  LCD_Printf("Connection successful!\n\n");
  
  Madgwick_init();
  
  lastTime = drawTime = HAL_GetTick();

  
  SNAKE_init(&SnakeHead, &SnakeTail, SNAKE_LEN, POINT_SIZE);


  tmp_snake = SnakeHead;
  while(tmp_snake != NULL) {
    drow_point(tmp_snake->position, SNAKE_COLOR);
    tmp_snake = tmp_snake->prev;
  }


  while (1)
  {
    BMP280_read_temperature_double(&temp);
    BMP280_read_pressure_double(&press);
    alt = BMP280_calculate_altitude(102900); // insert actual data here
    
    MPU9250_getMotion9Real(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
    
    currentTime = HAL_GetTick();
    Madgwick_update(gx, gy, gz, ax, ay, az, mx, my, mz, (currentTime - lastTime)/1000.0);
    lastTime = currentTime;
    
    if (HAL_GetTick() - drawTime > 20) {
      LCD_SetCursor(0, 64);
      LCD_Printf("T: %6.2f C  P: %6.0f Pa  A: %3.0f m\n", temp, press, alt);
      LCD_Printf("Accel:   %7.4f %7.4f %7.4f\n", ax, ay, az);
      // LCD_Printf("Gyro:    %7.4f %7.4f %7.4f\n", gx, gy, gz);
      // LCD_Printf("Compass: %7.1f %7.1f %7.1f\n\n", mx, my, mz);
      // LCD_Printf("Madgwick: P: %5.1f R: %5.1f Y: %5.1f\n", Madgwick_getPitch(), Madgwick_getRoll(), Madgwick_getYaw());
      drawTime = HAL_GetTick();
    }

    if (next_render_tick < currentTime) {
      set_direction(ax, ay);

      next_render_tick = currentTime + TPF;
      SNAKE_add_point(&SnakeHead, &SnakeTail, current_direction, POINT_SIZE);
      drow_point(SnakeTail->position, BG_COLOR);
      SNAKE_remove_last_point(&SnakeTail);

      drow_point(SnakeHead->position, SNAKE_COLOR);
    }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Activate the Over-Drive mode 
    */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
void I2C_ScanBus() {
  uint8_t i = 0;
  LCD_Printf("Scanning for I2C devices:\n");
  for (i = 0x07; i < 0x78; i++) {
    if (HAL_I2C_IsDeviceReady(&hi2c1, i << 1, 10, 100) == HAL_OK)
      LCD_Printf("Ready: 0x%02x\n", i);
  }
}


void set_direction(float ax, float ay) {
  float a_ax = abs(ax),
        a_ay = abs(ay), 
        maxAbs = max(a_ax, a_ay);
  Direction new_direction;  

  LCD_Printf("VALUES: %1.2f %1.2f %1.2f %1.2f %1.2f\n", ax, ay, a_ax, a_ay, maxAbs);
  if (maxAbs < 0.3) return;
  
  if (a_ax > a_ay) {
    if (ax > 0.0) {
      new_direction = RIGHT;
    }
    else {
      new_direction = LEFT;
    }
  }
  else {
    if (ay > 0.0) {
      new_direction = TOP;
    }
    else {
      new_direction = BOTTOM; 
    }
  }
  if ((current_direction == TOP && new_direction == BOTTOM) ||
        (current_direction == BOTTOM && new_direction == TOP) ||
        (current_direction == RIGHT && new_direction == LEFT) ||
        (current_direction == LEFT && new_direction == RIGHT)) {
      return;
  }
  current_direction = new_direction;

}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
