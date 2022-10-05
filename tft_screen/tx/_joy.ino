  void navjoyread_menu(void)
  {
    //put values and attributes into arrays for easy iteration
    int numoptions = 4;
    int tmp;
    //vals is an array of pointers to the variables, must dereference to get the value of the variable
    int *vals[numoptions] = {&current.page, &tx_msg.target_wheel, &tx_msg.target_blade, &tx_msg.target_height};
    int increment[numoptions]={1,10,100,5};
    int minval[numoptions]={0,50,8000,15}; //lowest allowable value
    int maxval[numoptions]={1,250,15000,55};//highest allowable value
    
    //read stick
    navjoy.xval = analogRead(navjoy.XPIN)-navjoy.X0 ;
    navjoy.xval *= -1;
    navjoy.yval = analogRead(navjoy.YPIN)-navjoy.Y0 ;
    
    //if y moves, increment/decrement current.row
    if (abs(navjoy.yval) > navjoy.THRESH_MOVE)
    {
     Serial.printf("before = %i \n",current.row);
      int up1ordown1 = navjoy.yval / abs(navjoy.yval);//magnitude=1, sign for up or down
      Serial.printf("up1ordown1 = %i \n",up1ordown1);
      tmp=current.row;
      current.row += up1ordown1;
      if (current.row < 0 || current.row >= numoptions)
      {current.row = tmp;}
      delay(300);
      
    }
    //if x moves, increment/decrement value in current.row
    if (abs(navjoy.xval) > navjoy.THRESH_MOVE)
    {
      // increment values
      int up1ordown1 = navjoy.xval / abs(navjoy.xval);//magnitude=1, sign for up or down
      tmp = *(vals[current.row]); //store current val of var pointed to by current.row
      *vals[current.row] += up1ordown1 * increment[current.row];
      
      //correct values if they fall outside of accepted range
      if (*vals[current.row] < minval[current.row] || *vals[current.row] > maxval[current.row])
        {*vals[current.row]=tmp;}
      delay(300);
    }

    //if x moves left when current.row==0, reset view to dashboard
    if (current.row == 0 && navjoy.xval < -navjoy.THRESH_MOVE)
    {
      changepage(1);
    }
  };

   void navjoyread_dash(void)
  {
    //read stick
    navjoy.xval = analogRead(navjoy.XPIN)-navjoy.X0 ;
    navjoy.xval *= -1;
    navjoy.yval = analogRead(navjoy.YPIN)-navjoy.Y0 ;
    
    //store drive mode in current.flexvar1 (0=idle,1=drive,2=cut)
    current.flexvar1=0;          //zero as default
    if (navjoy.xval > navjoy.THRESH_MOVE) 
      {current.flexvar1 = 1;}    //allow drive if joy to the right
    if(current.flexvar1 == 1 && navjoy.yval > navjoy.THRESH_MOVE) 
      {current.flexvar1 = 2;}    //allow cut if joy right and low
    
    //if idle and x moves low,return to settings menu
    if (current.flexvar1 == 0 && navjoy.xval < -navjoy.THRESH_MOVE)
    {
      //tapping left means stop showing dashboard and start showing main menu
      changepage(0);
    }
  };

  void changepage(int newpage)
  {
    //erase screen and set new page to be rendered
    current.page = newpage;

    //when changing current screen, reset all other vars to zero
    current.row = 0;
    current.col = 0;
    current.flexvar1 = 0;
    current.flexvar2 = -1;//to ensure full redraw of dash labels

    tft.fillScreen(TFT_BLACK);
    // tft.drawRect(0, 0, 240, 320, TFT_BLACK);

    delay(500);
  };
