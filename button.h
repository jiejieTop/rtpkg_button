#ifndef BUTTON_H
#define BUTTON_H

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <string.h>

#define BTN_NAME_MAX  32     //名字最大为32字节

/* 
 The button debounce time is 40ms, the recommended calling period is 20ms.
 It is considered valid only if the 40ms state is continuously detected, including bounce and press.
*/

//#define CONTINUOS_TRIGGER               //Whether to support continuous triggering, do not detect single-click and long-press if you send even

/* Whether to support click and double-click while there is a trigger,
 if you choose to enable the macro definition, single-click double-clickback,
 but click will delay the response.Because it must be judged whether a 
 double click is triggered after clicking, the delay time is the double-click 
 interval BUTTON_DOUBLE_TIME.If you do not enable this macro definition,
 it is recommended that there is only one click/double click in the project. 
 Otherwise, a click will be triggered when the response is double-clicked.
 Because the double click must be generated after one press and release.
*/

//#define SINGLE_AND_DOUBLE_TRIGGER      

/* Whether long-press release is supported or not. If this macro definition is turned on, a long press is triggered after a long press release.
    Otherwise, long press is triggered for a long time, the trigger period is determined by BUTTON_LONG_CYCLE*/
//#define LONG_FREE_TRIGGER              

#ifndef  BUTTON_DEBOUNCE_TIME
#define BUTTON_DEBOUNCE_TIME   2   //Debounce time  (n-1)*call cycle
#endif

#ifndef  BUTTON_CONTINUOS_CYCLE
#define BUTTON_CONTINUOS_CYCLE  1  //Double-click the trigger cycle time  (n-1)*call cycle
#endif

#ifndef  BUTTON_LONG_CYCLE
#define BUTTON_LONG_CYCLE       1  //Long press trigger cycle time   (n-1)*call cycle
#endif

#ifndef  BUTTON_DOUBLE_TIME
#define BUTTON_DOUBLE_TIME      15 //Double click interval  (n-1)*call cycle  Recommended at 200-600ms
#endif

#ifndef BUTTON_LONG_TIME
#define BUTTON_LONG_TIME       50  //For n seconds ((n-1)*call cycle)ms, think long press event
#endif


#define TRIGGER_CB(event)   \
        if(btn->CallBack_Function[event]) \
          btn->CallBack_Function[event]((Button_t*)btn)

typedef void (*Button_CallBack)(void*);   //The button triggers the callback function and needs to be implemented by the user.



typedef enum {
  BUTTON_DOWM = 0,
  BUTTON_UP,
  BUTTON_DOUBLE,
  BUTTON_LONG,
  BUTTON_LONG_FREE,
  BUTTON_CONTINUOS,
  BUTTON_CONTINUOS_FREE,
  BUTTON_ALL_RIGGER,
  number_of_event,                         /* The event that triggered the callback */
  NONE_TRIGGER
}Button_Event;

/*
  Each button corresponds to a global structure variable.
  Its member variables are necessary to implement filtering and multiple button states.
*/
typedef struct button
{
  /* The following is a function pointer pointing to the function that determines whether the button is pressed or not. */
  rt_uint8_t (*Read_Button_Level)(void); /* Read the button level function, you need to implement */
  
  char Name[BTN_NAME_MAX];
  
  rt_uint8_t Button_State              :   4;    /* The current state of the button (pressed or bounced) */
  rt_uint8_t Button_Last_State         :   4;    /* The last button state used to determine the double click */
  rt_uint8_t Button_Trigger_Level      :   2;    /* Button trigger level */
  rt_uint8_t Button_Last_Level         :   2;    /* Button current level */
  
  rt_uint8_t Button_Trigger_Event;               /* Button trigger event, click, double click, long press, etc. */
  
  Button_CallBack CallBack_Function[number_of_event];
  
  rt_uint8_t Button_Cycle;                       /* Continuous button cycle */
  
  rt_uint8_t Timer_Count;                        /* Timing */
  rt_uint8_t Debounce_Time;                      /* Debounce time */
  
  rt_uint8_t Long_Time;                          /* Button press duration */
  
  struct button *Next;
  
}Button_t;




/* Function declaration for external calls */

void Button_Create(const char *name,
                  Button_t *btn, 
                  rt_uint8_t(*read_btn_level)(void),
                  rt_uint8_t btn_trigger_level);
                  
void Button_Attach(Button_t *btn,Button_Event btn_event,Button_CallBack btn_callback);   
                  
void Button_Cycle_Process(Button_t *btn);   
                  
void Button_Process(void);
                  
void Button_Delete(Button_t *btn);   
  
void Search_Button(void);     
                  
void Get_Button_EventInfo(Button_t *btn);
rt_uint8_t Get_Button_Event(Button_t *btn);
rt_uint8_t Get_Button_State(Button_t *btn);
void Button_Process_CallBack(void *btn);
                  
#endif
