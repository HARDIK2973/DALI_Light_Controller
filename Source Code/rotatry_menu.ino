
// Include the required libraries
#include <LiquidCrystal.h>
#include <Dali.h>


// Define the LCD pins
LiquidCrystal lcd(5,6,7,8,9,10);


int menuCounter = 0; //counts the clicks of the rotary encoder between menu items (0-3 in this case)


int menu3_Value = 0; //value within menu 3

bool menu1_selected = false; //enable/disable to change the value of menu item
bool menu2_selected = false;
bool menu3_selected = false;


//Defining pins
//Arduino interrupt pins: 2, 3.
const int RotaryCLK = 2; //CLK pin on the rotary encoder
const int RotaryDT = 3; //DT pin on the rotary encoder
const int PushButton = 4; //Button to enter/exit menu


//Statuses for the rotary encoder
int CLKNow;
int CLKPrevious;

int DTNow;
int DTPrevious;

bool refreshLCD = true; //refreshes values
bool refreshSelection = false; //refreshes selection (> / X)


// Define the variable and its limits
int Brightness_Level = 0;
const int minBrightness = 0;
const int maxBrightness = 255;

void setup() {

  
  pinMode(2, INPUT_PULLUP); //RotaryCLK
  pinMode(3, INPUT_PULLUP); //RotaryDT
  pinMode(4, INPUT_PULLUP); //Button

  // Set up the LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Welcome!");

  Dali.begin(2, 3);

  lcd.clear();

  //------------------------------------------------------
  //Store states of the rotary encoder
  CLKPrevious = digitalRead(RotaryCLK);
  DTPrevious = digitalRead(RotaryDT);
      
  attachInterrupt(digitalPinToInterrupt(RotaryCLK), rotate, CHANGE); //CLK pin is an interrupt pin
  attachInterrupt(digitalPinToInterrupt(PushButton), pushButton, FALLING); //PushButton pin is an interrupt pin

 
}

void loop() {

  if(refreshLCD == true) //If we are allowed to update the LCD ...
  {
    updateLCD(); // ... we update the LCD ...

    //... also, if one of the menus are already selected...
    if(menu3_selected == true)
    {
     // do nothing
    }
    else
    {
      updateCursorPosition(); //update the position
    }
    
    refreshLCD = false; //reset the variable - wait for a new trigger
  }

  if(refreshSelection == true) //if the selection is changed
  {
    updateSelection(); //update the selection on the LCD
    refreshSelection = false; // reset the variable - wait for a new trigger
  }
  
 }

 
void rotate()
{  
  //---MENU3---------------------------------------------------------------
 if(menu3_selected == true)
  {
    CLKNow = digitalRead(RotaryCLK); //Read the state of the CLK pin
  // If last and current state of CLK are different, then a pulse occurred  
  if (CLKNow != CLKPrevious  && CLKNow == 1)
  {
    // If the DT state is different than the CLK state then
    // the encoder is rotating in A direction, so we increase
    if (digitalRead(RotaryDT) != CLKNow) 
    {
      if(menu3_Value < 255) //we do not go above 100
      {
        menu3_Value++;
        Dali.sendArc(3, menu3_Value);  
      }
      else
      {
        menu3_Value = 255;  
      }      
    } 
    else 
    {
      if(menu3_Value < 0) //we do not go below 0
      {
          menu3_Value = 0;
      }
      else
      {
      // Encoder is rotating B direction so decrease
      menu3_Value--;      
      Dali.sendArc(3, menu3_Value);  

      }      
    }    
  }
  CLKPrevious = CLKNow;  // Store last CLK state
  }
  else //MENU COUNTER----------------------------------------------------------------------------
  {
  CLKNow = digitalRead(RotaryCLK); //Read the state of the CLK pin
  // If last and current state of CLK are different, then a pulse occurred  
  if (CLKNow != CLKPrevious  && CLKNow == 1)
  {
    // If the DT state is different than the CLK state then
    // the encoder is rotating in A direction, so we increase
    if (digitalRead(RotaryDT) != CLKNow) 
    {
      if(menuCounter < 2) //we do not go above 3
      {
        menuCounter++;  
      }
      else
      {
        menuCounter = 0;  
      }      
    } 
    else 
    {
      if(menuCounter < 1) //we do not go below 0
      {
          menuCounter = 2;
      }
      else
      {
      // Encoder is rotating CW so decrease
      menuCounter--;      
      }      
    }    
  }
  CLKPrevious = CLKNow;  // Store last CLK state
  }

  //Refresh LCD after changing the counter's value
  refreshLCD = true;
}

void pushButton()
{
  switch(menuCounter)
  {
     case 0:
     menu1_selected = !menu1_selected;  //we change the status of the variable to the opposite
     Dali.sendArc(3,254);
     break;

     case 1:
     menu2_selected = !menu2_selected;
     Dali.sendCmd(3, Dali.CMD_OFF);
     break;

     case 2:
     menu3_selected = !menu3_selected;
     break;

    
  } 
  
  refreshLCD = true; //Refresh LCD after changing the value of the menu
  refreshSelection = true; //refresh the selection ("X")
}

void printLCD()
{
  //These are the values which are not changing the operation
  
  lcd.setCursor(1,0); //1st line, 2nd block
  lcd.print("ON"); //text
  //----------------------
  lcd.setCursor(1,1); //2nd line, 2nd block
  lcd.print("OFF"); //text
  //----------------------
  lcd.setCursor(9,0); //1st line, 10th block
  lcd.print("DIMMING"); //text

}

void updateLCD()
{  
  lcd.setCursor(11,1);
  lcd.print("   ");
  lcd.setCursor(11,1);
  lcd.print(menu3_Value); 

}

void updateCursorPosition()
{
  //Clear display's ">" parts 
  lcd.setCursor(0,0); //1st line, 1st block
  lcd.print(" "); //erase by printing a space
  lcd.setCursor(0,1);
  lcd.print(" "); 
  lcd.setCursor(0,2);
  lcd.print(" "); 
  lcd.setCursor(0,3);
  lcd.print(" "); 
     
  //Place cursor to the new position
  switch(menuCounter) //this checks the value of the counter (0, 1 or 2)
  {
    case 0:
    lcd.setCursor(0,0); //1st line, 1st block
    lcd.print(">"); 
    break;
    //-------------------------------
    case 1:
    lcd.setCursor(0,1); //2nd line, 1st block
    lcd.print(">"); 
    break;
    //-------------------------------    
    case 2:
    lcd.setCursor(8,0); //3rd line, 1st block
    lcd.print(">"); 
    break;
    
  }
}

void updateSelection()
{
  //When a menu is selected ">" becomes "X"

  if(menu1_selected == true)
  {
    lcd.setCursor(0,0); //1st line, 1st block
    lcd.print("X"); 
  }
  //-------------------
   if(menu2_selected == true)
  {
    lcd.setCursor(0,1); //2nd line, 1st block
    lcd.print("X"); 
  }
  //-------------------
  if(menu3_selected == true)
  {
    lcd.setCursor(8,0); //3rd line, 1st block
    lcd.print("X"); 
  }
  
}


