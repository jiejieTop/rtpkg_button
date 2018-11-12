/************************************************************
  * @brief   button_drive
  * @param   RT_NULL
  * @return  RT_NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    button.c
  ***********************************************************/

#include "button.h"

#ifdef PKG_USING_BUTTON

/*******************************************************************
 *                          Variable Declaration                              
 *******************************************************************/

static struct button* Head_Button = RT_NULL;


/*******************************************************************
 *                         Function Declaration   
 *******************************************************************/
static char *StrnCopy(char *dst, const char *src, rt_uint32_t n);
static void Print_Btn_Info(Button_t* btn);
static void Add_Button(Button_t* btn);


/************************************************************
  * @brief   Create a Button 
  * @param   name:button name 
  * @param   btn:button structure
  * @param   read_btn_level:Button trigger level reading function,
  *                 Return the level of the rt_uint8_t type by yourself
  * @param   btn_trigger_level:Button trigger level
  * @return  RT_NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    RT_NULL
  ***********************************************************/
void Button_Create(const char *name,
                  Button_t *btn, 
                  rt_uint8_t(*read_btn_level)(void),
                  rt_uint8_t btn_trigger_level)
{
  if( btn == RT_NULL)
  {
    RT_DEBUG_LOG(RT_DEBUG_THREAD,("struct button is RT_NULL!"));
  }
  
  memset(btn, 0, sizeof(struct button));      //Clear structure information
 
  StrnCopy(btn->Name, name, BTN_NAME_MAX);    //button name 
  
  
  btn->Button_State = NONE_TRIGGER;                     //Button status
  btn->Button_Last_State = NONE_TRIGGER;                //Button last status
  btn->Button_Trigger_Event = NONE_TRIGGER;             //Button trigger event
  btn->Read_Button_Level = read_btn_level;              //Button trigger level reading function
  btn->Button_Trigger_Level = btn_trigger_level;        //Button trigger level
  btn->Button_Last_Level = btn->Read_Button_Level();    //Button current level
  btn->Debounce_Time = 0;
  
  RT_DEBUG_LOG(RT_DEBUG_THREAD,("button create success!"));
  
  Add_Button(btn);          //Added to the singly linked list when button created
  
  Print_Btn_Info(btn);      //printf info
 
}

/************************************************************
  * @brief   burron trigger events are attach to callback function
  * @param   btn:button structure
  * @param   btn_event:button events
  * @param   btn_callback : Callback handler after the button is triggered.Need user implementation
  * @return  RT_NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
void Button_Attach(Button_t *btn,Button_Event btn_event,Button_CallBack btn_callback)
{
  if( btn == RT_NULL)
  {
    RT_DEBUG_LOG(RT_DEBUG_THREAD,("struct button is RT_NULL!"));
  }
  
  if(BUTTON_ALL_RIGGER == btn_event)
  {
    for(rt_uint8_t i = 0 ; i < number_of_event-1 ; i++)
      btn->CallBack_Function[i] = btn_callback;   //A callback function triggered by a button event ,Used to handle button events 
  }
  else
  {
    btn->CallBack_Function[btn_event] = btn_callback; 
  }
}

/************************************************************
  * @brief   Delete an already created button
  * @param   RT_NULL
  * @return  RT_NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    RT_NULL
  ***********************************************************/
void Button_Delete(Button_t *btn)
{
  struct button** curr;
  for(curr = &Head_Button; *curr;) 
  {
    struct button* entry = *curr;
    if (entry == btn) 
    {
      *curr = entry->Next;
    } 
    else
    {
      curr = &entry->Next;
    }
  }
}

/************************************************************
  * @brief   Get Button Event Info
  * @param   RT_NULL
  * @return  RT_NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
void Get_Button_EventInfo(Button_t *btn)
{
  for(rt_uint8_t i = 0 ; i < number_of_event-1 ; i++)
  {
    if(btn->CallBack_Function[i] != 0)
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Button_Event:%d",i));//print
    }      
  } 
}

/************************************************************
  * @brief   Get Button Event
  * @param   RT_NULL
  * @return  RT_NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
rt_uint8_t Get_Button_Event(Button_t *btn)
{
  return (rt_uint8_t)(btn->Button_Trigger_Event);
}

/************************************************************
  * @brief   Get Button State
  * @param   RT_NULL
  * @return  RT_NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  ***********************************************************/
rt_uint8_t Get_Button_State(Button_t *btn)
{
  return (rt_uint8_t)(btn->Button_State);
}

/************************************************************
  * @brief   button cycle processing function
  * @param   btn:处理的按键
  * @return  RT_NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    This function must be called in a certain period. The recommended period is 20~50ms.
  ***********************************************************/
void Button_Cycle_Process(Button_t *btn)
{
  rt_uint8_t current_level = (rt_uint8_t)btn->Read_Button_Level();  //Get the current button level
  
  if((current_level != btn->Button_Last_Level)&&(++(btn->Debounce_Time) >= BUTTON_DEBOUNCE_TIME))   //Button level changes, debounce
  {
      btn->Button_Last_Level = current_level; //Update current button level
      btn->Debounce_Time = 0;                 //button is pressed
      
      //If the button is not pressed, change the button state to press (first press / double trigger)
      if((btn->Button_State == NONE_TRIGGER)||(btn->Button_State == BUTTON_DOUBLE))
      {
        btn->Button_State = BUTTON_DOWM;
      }
      //free button
      else if(btn->Button_State == BUTTON_DOWM)
      {
        btn->Button_State = BUTTON_UP;
        RT_DEBUG_LOG(RT_DEBUG_THREAD,("button release"));
      }
  }
  
  switch(btn->Button_State)
  {
    case BUTTON_DOWM :                                        // button dowm 
    {
      if(btn->Button_Last_Level == btn->Button_Trigger_Level) 
      {
        #ifdef CONTINUOS_TRIGGER                              //Support continuous triggering

        if(++(btn->Button_Cycle) >= BUTTON_CONTINUOS_CYCLE)
        {
          btn->Button_Cycle = 0;
          btn->Button_Trigger_Event = BUTTON_CONTINUOS; 
          TRIGGER_CB(BUTTON_CONTINUOS);                      //continuous triggering
          RT_DEBUG_LOG(RT_DEBUG_THREAD,("continuous triggering"));
        }
        
        #else
        
        btn->Button_Trigger_Event = BUTTON_DOWM;
      
        if(++(btn->Long_Time) >= BUTTON_LONG_TIME)            //Update the trigger event before releasing the button as long press
        {
          #ifdef LONG_FREE_TRIGGER
          
          btn->Button_Trigger_Event = BUTTON_LONG; 
          
          #else
          
          if(++(btn->Button_Cycle) >= BUTTON_LONG_CYCLE)      //Continuous triggering of long press cycles
          {
            btn->Button_Cycle = 0;
            btn->Button_Trigger_Event = BUTTON_LONG; 
            TRIGGER_CB(BUTTON_LONG);    //long triggering
          }
          #endif
          
          if(btn->Long_Time == 0xFF)    //Update time overflow
          {
            btn->Long_Time = BUTTON_LONG_TIME;
          }
          RT_DEBUG_LOG(RT_DEBUG_THREAD,("Long press"));
        }
          
        #endif
      }

      break;
    } 
    
    case BUTTON_UP :                                        //button up
    {
      if(btn->Button_Trigger_Event == BUTTON_DOWM)          //Trigger click
      {
        if((btn->Timer_Count <= BUTTON_DOUBLE_TIME)&&(btn->Button_Last_State == BUTTON_DOUBLE)) // double click
        {
          btn->Button_Trigger_Event = BUTTON_DOUBLE;
          TRIGGER_CB(BUTTON_DOUBLE);    
          RT_DEBUG_LOG(RT_DEBUG_THREAD,("double click"));
          btn->Button_State = NONE_TRIGGER;
          btn->Button_Last_State = NONE_TRIGGER;
        }
        else
        {
            btn->Timer_Count=0;
            btn->Long_Time = 0;                   //Detection long press failed, clear 0
          
          #ifndef SINGLE_AND_DOUBLE_TRIGGER
            TRIGGER_CB(BUTTON_DOWM);              //click
          #endif
            btn->Button_State = BUTTON_DOUBLE;
            btn->Button_Last_State = BUTTON_DOUBLE;
          
        }
      }
      
      else if(btn->Button_Trigger_Event == BUTTON_LONG)
      {
        #ifdef LONG_FREE_TRIGGER
          TRIGGER_CB(BUTTON_LONG);                    //Long press
        #else
          TRIGGER_CB(BUTTON_LONG_FREE);               //Long press free
        #endif
        btn->Long_Time = 0;
        btn->Button_State = NONE_TRIGGER;
        btn->Button_Last_State = BUTTON_LONG;
      } 
      
      #ifdef CONTINUOS_TRIGGER
        else if(btn->Button_Trigger_Event == BUTTON_CONTINUOS)  //Press continuously
        {
          btn->Long_Time = 0;
          TRIGGER_CB(BUTTON_CONTINUOS_FREE);                    //Press continuously free
          btn->Button_State = NONE_TRIGGER;
          btn->Button_Last_State = BUTTON_CONTINUOS;
        } 
      #endif
      
      break;
    }
    
    case BUTTON_DOUBLE :
    {
      btn->Timer_Count++;                                       //Update time 
      if(btn->Timer_Count>=BUTTON_DOUBLE_TIME)
      {
        btn->Button_State = NONE_TRIGGER;
        btn->Button_Last_State = NONE_TRIGGER;
      }
      #ifdef SINGLE_AND_DOUBLE_TRIGGER
      
        if((btn->Timer_Count>=BUTTON_DOUBLE_TIME)&&(btn->Button_Last_State != BUTTON_DOWM))
        {
          btn->Timer_Count=0;
          TRIGGER_CB(BUTTON_DOWM);    
          btn->Button_State = NONE_TRIGGER;
          btn->Button_Last_State = BUTTON_DOWM;
        }
        
      #endif

      break;
    }

    default :
      break;
  }
  
}

/************************************************************
  * @brief   Traversing the way to scan the button without losing each button
  * @param   RT_NULL
  * @return  RT_NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    This function is called periodically, it is recommended to call 20-50ms once.
  ***********************************************************/
void Button_Process(void)
{
  struct button* pass_btn;
  for(pass_btn = Head_Button; pass_btn != RT_NULL; pass_btn = pass_btn->Next)
  {
      Button_Cycle_Process(pass_btn);
  }
}

/************************************************************
  * @brief   Search Button
  * @param   RT_NULL
  * @return  RT_NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    RT_NULL
  ***********************************************************/
void Search_Button(void)
{
  struct button* pass_btn;
  for(pass_btn = Head_Button; pass_btn != RT_NULL; pass_btn = pass_btn->Next)
  {
    RT_DEBUG_LOG(RT_DEBUG_THREAD,("button node have %s",pass_btn->Name));
  }
}

/************************************************************
  * @brief   Handle all button callback functions
  * @param   RT_NULL
  * @return  RT_NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    Not implemented yet
  ***********************************************************/
void Button_Process_CallBack(void *btn)
{
  rt_uint8_t btn_event = Get_Button_Event(btn);

  switch(btn_event)
  {
    case BUTTON_DOWM:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your press trigger"));
      break;
    }
    
    case BUTTON_UP:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your trigger release"));
      break;
    }
    
    case BUTTON_DOUBLE:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your double-click trigger"));
      break;
    }
    
    case BUTTON_LONG:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your long press trigger"));
      break;
    }
    
    case BUTTON_LONG_FREE:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your long press trigger free"));
      break;
    }
    
    case BUTTON_CONTINUOS:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add your continuous trigger processing logic"));
      break;
    }
    
    case BUTTON_CONTINUOS_FREE:
    {
      RT_DEBUG_LOG(RT_DEBUG_THREAD,("Add processing logic for your continuous trigger release"));
      break;
    }
      
  } 
}


/**************************** The following is the internal call function ********************/

/************************************************************
  * @brief   Copy the specified length string
  * @param   RT_NULL
  * @return  RT_NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    RT_NULL
  ***********************************************************/
static char *StrnCopy(char *dst, const char *src, rt_uint32_t n)
{
  if (n != 0)
  {
    char *d = dst;
    const char *s = src;
    do
    {
        if ((*d++ = *s++) == 0)
        {
            while (--n != 0)
                *d++ = 0;
            break;
        }
    } while (--n != 0);
  }
  return (dst);
}

/************************************************************
  * @brief   Print button related information
  * @param   RT_NULL
  * @return  RT_NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    RT_NULL
  ***********************************************************/
static void Print_Btn_Info(Button_t* btn)
{
  
  RT_DEBUG_LOG(RT_DEBUG_THREAD,("button struct information:\n\
              btn->Name:%s \n\
              btn->Button_State:%d \n\
              btn->Button_Trigger_Event:%d \n\
              btn->Button_Trigger_Level:%d \n\
              btn->Button_Last_Level:%d \n\
              ",
              btn->Name,
              btn->Button_State,
              btn->Button_Trigger_Event,
              btn->Button_Trigger_Level,
              btn->Button_Last_Level));
  Search_Button();
}
/************************************************************
  * @brief   Connect buttons with a single linked list
  * @param   RT_NULL
  * @return  RT_NULL
  * @author  jiejie
  * @github  https://github.com/jiejieTop
  * @date    2018-xx-xx
  * @version v1.0
  * @note    RT_NULL
  ***********************************************************/
static void Add_Button(Button_t* btn)
{
  struct button *pass_btn = Head_Button;
  
  while(pass_btn)
  {
    pass_btn = pass_btn->Next;
  }
  
  btn->Next = Head_Button;
  Head_Button = btn;
}

#endif



