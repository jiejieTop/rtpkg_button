
#include "button.h"

/**
  ******************************************************************
                              Variable declaration
  ******************************************************************
  */ 

Button_t Button1;
Button_t Button2; 


/**
  ******************************************************************
                             Function declaration
  ******************************************************************
  */ 
  
static void BSP_Init(void);

void Btn1_Dowm_CallBack(void *btn)
{
  PRINT_INFO("Button1 Click!");
}

void Btn1_Double_CallBack(void *btn)
{
  PRINT_INFO("Button1 Double click!");
}

void Btn1_Long_CallBack(void *btn)
{
  PRINT_INFO("Button1 Long press!");
}

void Btn1_Continuos_CallBack(void *btn)
{
  PRINT_INFO("Button1 Press continuously!");
}
void Btn1_ContinuosFree_CallBack(void *btn)
{
  PRINT_INFO("Button1 Press continuously free!");
}


void Btn2_Dowm_CallBack(void *btn)
{
  PRINT_INFO("Button2 Click!");
}

void Btn2_Double_CallBack(void *btn)
{
  PRINT_INFO("Button2 Double click!");
}

void Btn2_Long_CallBack(void *btn)
{
  PRINT_INFO("Button2 Long press!");
}

void Btn2_Continuos_CallBack(void *btn)
{
  PRINT_INFO("Button2 Press continuously!");
}
void Btn2_ContinuosFree_CallBack(void *btn)
{
  PRINT_INFO("Button2 Press continuously free!");
}

/**
  ******************************************************************
  * @brief   main
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  ******************************************************************
  */ 
int main(void)
{
  uint32_t i = 100000;
  
  BSP_Init();
  
  Button_Create("Button1",
              &Button1, 
              Read_KEY1_Level, 
              KEY_ON);
  Button_Attach(&Button1,BUTTON_DOWM,Btn1_Dowm_CallBack);                       //Click
  Button_Attach(&Button1,BUTTON_DOUBLE,Btn1_Double_CallBack);                   //Double click
  Button_Attach(&Button1,BUTTON_CONTINUOS,Btn1_Continuos_CallBack);             //Press continuously
  Button_Attach(&Button1,BUTTON_CONTINUOS_FREE,Btn1_ContinuosFree_CallBack);    //Press continuously free
  Button_Attach(&Button1,BUTTON_LONG,Btn1_Long_CallBack);                       //Long press


  Button_Create("Button2",
              &Button2, 
              Read_KEY2_Level, 
              KEY_ON);
  Button_Attach(&Button2,BUTTON_DOWM,Btn2_Dowm_CallBack);                     //Click
  Button_Attach(&Button2,BUTTON_DOUBLE,Btn2_Double_CallBack);                 //Double click
  Button_Attach(&Button2,BUTTON_CONTINUOS,Btn2_Continuos_CallBack);           //Press continuously
  Button_Attach(&Button2,BUTTON_CONTINUOS_FREE,Btn2_ContinuosFree_CallBack);  //Press continuously free
  Button_Attach(&Button2,BUTTON_LONG,Btn2_Long_CallBack);                     //Long press

  Get_Button_Event(&Button1);
  Get_Button_Event(&Button2);

  while(1)                            
  {

    Button_Process();     //Need to call the button handler function periodically

    Delay_ms(20);
    
    
  }
}

/**
  ******************************************************************
  * @brief   BSP_Init
  * @author  jiejie
  * @version V1.0
  * @date    2018-xx-xx
  ******************************************************************
  */ 
static void BSP_Init(void)
{
  /* LED Config */
  LED_GPIO_Config();
  
  /* USART Config */
  USART_Config();
  
  /* button config */
  Key_GPIO_Config();
  
  
 /* print info */
  PRINT_INFO("welcome to learn jiejie stm32 library!\n");
  
}


/********************************END OF FILE***************************************/
