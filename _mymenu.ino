void draw_menu(void)
{ 
  //put values to display in arrays for access in easy iteration  
  int numoptions = 4; //how many menu uptions (length of arrays)
  const char *labels[numoptions] = {" SAVE AND EXIT  ", " Ground Speed (fps) ", " Blade Speed (fps) ", " Cut Height (in) "};
  float vals[numoptions]={0.0,(float)tx_msg.target_wheel, (float)tx_msg.target_blade, (float)tx_msg.target_height/10};//target_wheel, target_blade, target_height
  int precision[numoptions] = {0,0,0,1};//decimals to print for values
  
  //define placeholders for colors of highlighted, regular, and hidden features
  int labeltxt, labelbg, value, arrow; // colors formaking values visible or hidden 

  // Draw title
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("SETTINGS MENU", 120,20,4); 
  tft.drawLine(10, 48, 230, 48, TFT_BLUE); 

  //Draw out the 4 strings at the 4 locations defined in grid.y[xxxxxx]
  int yloc=grid.y[1]; //set vertical location of first menu option
  
  for (int i=0; i<numoptions; i++)
  {
    //Set colors for active or not
    if (i == current.row) //highlighted label, white value and nav arrows
    {
      labeltxt = TFT_BLACK;
      labelbg = TFT_WHITE;
      value = TFT_WHITE;
      arrow = TFT_WHITE;
    }
    else //all the other rows have white labels, hidden value and nav arrows
    {
      labeltxt = TFT_WHITE;
      labelbg = TFT_BLACK;
      value = TFT_BLACK;
      arrow = TFT_BLACK;
    }

    //Draw label
    tft.setTextDatum(TL_DATUM);                    //set datum
    tft.setTextColor(labeltxt, labelbg);        //normal text
    tft.drawString(labels[i], grid.x[1],yloc,2);  //draw label
    
    yloc += 1.5*grid.ROWHEIGHT;
    
    //Draw nav arrows
    int centeroffset = 45;
    int center=120;
    int w=8;
    int h=8;
    int y=yloc+7;

    //every row gets the arrow drawn left...
    tft.fillTriangle(center-centeroffset-w, y, center-centeroffset, y+h/2, center-centeroffset, y-h/2, arrow);
    tft.setTextDatum(TC_DATUM);                    //set datum
    tft.setTextColor(value, TFT_BLACK); 
    
    //...the first element doesn't get a right arrow or numeric value shown
    if (i == 0)
      {
        tft.drawString("Return    ", 120,yloc,2); //draw this in special 
      }

    if (i>0)
    {
      tft.fillTriangle(center+centeroffset+w, y, center+centeroffset, y+h/2, center+centeroffset, y-h/2, arrow);
      String mystring = String(vals[i],precision[i]);
      tft.drawString(String(" "+ mystring + " "),120,yloc,2);
    }
    
    yloc += 1.5*grid.ROWHEIGHT; //move down for next label
    }     
};
