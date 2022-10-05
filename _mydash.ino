void draw_dash()
{
  

  // // debug: draw pagenum and driving state
  // tft.setTextColor(TFT_WHITE, TFT_BLACK); 
  // tft.drawNumber(current.page,20,20,4);

  // tft.drawNumber(current.flexvar1,220,20,4);

  // // Draw Header
  // tft.setTextColor(TFT_GREEN, TFT_BLACK);
  // tft.setTextDatum(TC_DATUM);
  // tft.drawString("Dashboard", 120,20,4); 
  // tft.drawLine(10, 48, 230, 48, TFT_BLUE); 

  //redraw whole screen on status change
  if (current.flexvar1 != current.flexvar2)
  {
    tft.fillScreen(TFT_BLACK);
    drawSTATUS(true,true);
    drawBLADES(1,true,false);
    drawWHEELS(2,true,false);
    drawBATTERY(3,true,true);
    current.flexvar2 = current.flexvar1;
  };

  //draw blade data only when cutting (status 2)
  if (current.flexvar1 == 2)
  {drawBLADES(1,false,true);}
  
  //draw wheel data when driving or cutting (status 1 or 2)
  if(current.flexvar1 >=1)
  {drawWHEELS(2,false,true);}

  //draw battery data always (status 0, 1, or 2)
  drawBATTERY(3,false,true);

};

void drawSTATUS(bool drawlabels, bool drawdata)
{
  int sectionnum = 0;//where to locate vertically
  int whichdata=0;//where in struct to look
  int line1_color, line2_color;//color of directions changes based on int current.flexvar1
  int y[] = {grid.y[sectionnum], grid.y[sectionnum] + grid.ROWHEIGHT, grid.y[sectionnum] + 2*grid.ROWHEIGHT, grid.y[sectionnum] + grid.ROWHEIGHT*3};
  
  if (drawlabels)
  {
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_GREEN, TFT_BLACK); //"STATUS:<CURRENT STATUS>" title always green
    switch(current.flexvar1) 
    {
      case 1 :
        tft.drawString("STATUS: DRIVING", grid.x[1]-3, y[1]-10, 4);
        break;
      case 2 :
        tft.drawString("STATUS: CUTTING", grid.x[1]-3, y[1]-10, 4);
        break;
      default:
        tft.drawString("STATUS: IDLE", grid.x[1]-3, y[1]-10, 4);
        // draw bottom exit label
        tft.setTextDatum(TC_DATUM);
        tft.setTextColor(TFT_YELLOW, TFT_RED);
        tft.drawString("  Tap (LEFT) to change settings  ", 120, 300, 2);
        break;
    }
  }
  
   tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_GREEN, TFT_BLACK); //"STATUS:<CURRENT STATUS>" title always green
  switch(current.flexvar1) 
  {
    case 1 :
      {line1_color = TFT_DARKGREY;
      line2_color = TFT_YELLOW;
      break;}
    case 2 :
      {tft.setTextColor(TFT_YELLOW, TFT_RED);
      tft.drawString("RELEASE THE STICK to stop", grid.x[1], y[3]+6, 2);
      break;}
    default:
      {line1_color = TFT_YELLOW;
      line2_color = TFT_YELLOW;
      //draw exit instructions at bottom
      tft.setTextDatum(TC_DATUM);
      tft.setTextColor(TFT_YELLOW, TFT_RED);
      tft.drawString("  Tap (LEFT) to change settings  ", 120, 300, 2);
      break;}
  }
            
    int line_yoffset = 15;
    tft.drawLine(10, 48, 230, 48, TFT_BLUE); 


  tft.setTextDatum(TL_DATUM);
  if (current.flexvar1 ==0 || current.flexvar1 == 1)
    {
    tft.setTextColor(line1_color, TFT_BLACK);
    tft.drawString("Hold (RIGHT) to drive...", grid.x[1], y[2]+4, 2);
    tft.setTextColor(line2_color, TFT_BLACK);
    tft.drawString("then also hold (DOWN) to cut", grid.x[1], y[3]+6, 2);
    }    
}

void drawBLADES(int sectionnum, bool drawlabels, bool drawdata)
{
  int whichdata=1;
  //y values
  int y[] = {grid.y[sectionnum], grid.y[sectionnum] + grid.ROWHEIGHT, grid.y[sectionnum] + 2*grid.ROWHEIGHT, grid.y[sectionnum] + grid.ROWHEIGHT*3};
  
  if (drawlabels)
  {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    
    tft.setTextDatum(TL_DATUM);
    tft.drawString(dashsection[whichdata].title, grid.x[1], y[1], 2);
    tft.drawString(dashsection[whichdata].label_row1, grid.x[1], y[2], 2);
    tft.drawString(dashsection[whichdata].label_row2, grid.x[1], y[3], 2);
    
    tft.setTextDatum(TC_DATUM);
    tft.drawString(dashsection[whichdata].label_col1, grid.x[2], y[1], 2);
    tft.drawString(dashsection[whichdata].label_col2, grid.x[3], y[1], 2);
    
    int line_yoffset = 15;
    tft.drawLine(10, y[1]+line_yoffset, 230, y[1]+line_yoffset, TFT_BLUE); 
  }
  if (drawdata)
  {
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(data_color, TFT_BLACK);

    tft.drawFloat(dashsection[whichdata].data_r1c1, 1, grid.x[2], y[2], 2);
    tft.drawFloat(dashsection[whichdata].data_r1c2, 0, grid.x[3], y[2], 2);
    tft.drawFloat(dashsection[whichdata].data_r2c1, 1, grid.x[2], y[3], 2);
    tft.drawFloat(dashsection[whichdata].data_r2c2, 0, grid.x[3], y[3], 2);
  }
}
void drawWHEELS(int sectionnum, bool drawlabels, bool drawdata)
{
  //y values
  int whichdata=2;
  int y[] = {grid.y[sectionnum], grid.y[sectionnum] + grid.ROWHEIGHT, grid.y[sectionnum] + 2*grid.ROWHEIGHT, grid.y[sectionnum] + grid.ROWHEIGHT*3};
  
  if (drawlabels)
  {
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.drawString(dashsection[whichdata].title, grid.x[1], y[1], 2);
    tft.drawString(dashsection[whichdata].label_row1, grid.x[1], y[2], 2);
    tft.drawString(dashsection[whichdata].label_row2, grid.x[1], y[3], 2);
    tft.setTextDatum(TC_DATUM);
    tft.drawString(dashsection[whichdata].label_col1, grid.x[2], y[1], 2);
    
    int line_yoffset = 15;
    tft.drawLine(10, y[1]+line_yoffset, 230, y[1]+line_yoffset, TFT_BLUE); 
  }
  if (drawdata)
  {
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(data_color, TFT_BLACK); 
    tft.drawFloat(dashsection[whichdata].data_r1c1, 0, grid.x[2], y[2], 2);
    tft.drawFloat(dashsection[whichdata].data_r2c1, 0, grid.x[2], y[3], 2); 
  }
}
void drawBATTERY(int sectionnum, bool drawlabels, bool drawdata)
{
  int whichdata = 3;
  //y values
  int y[] = {grid.y[sectionnum], grid.y[sectionnum] + grid.ROWHEIGHT, grid.y[sectionnum] + 2*grid.ROWHEIGHT, grid.y[sectionnum] + grid.ROWHEIGHT*3};
  
 if (drawlabels)
  {
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    
    tft.setTextDatum(TL_DATUM);
    tft.drawString(dashsection[whichdata].title, grid.x[1], y[1], 2);
    tft.drawString(dashsection[whichdata].label_row1, grid.x[1], y[2], 2);
    tft.drawString(dashsection[whichdata].label_row2, grid.x[1], y[3], 2);
    
    tft.setTextDatum(TC_DATUM);
    tft.drawString(dashsection[whichdata].label_col1, grid.x[2], y[1], 2);
    tft.drawString(dashsection[whichdata].label_col2, grid.x[3], y[1], 2);
    
    int line_yoffset = 15;
    tft.drawLine(10, y[1]+line_yoffset, 230, y[1]+line_yoffset, TFT_BLUE); 
  }

  if (drawdata)
    {
    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(data_color, TFT_BLACK);

    tft.drawFloat(dashsection[whichdata].data_r1c1, 1, grid.x[2], y[2], 2);
    tft.drawFloat(dashsection[whichdata].data_r1c2, 0, grid.x[3], y[2], 2);
    tft.drawFloat(dashsection[whichdata].data_r2c1, 1, grid.x[2], y[3], 2);
    tft.drawFloat(dashsection[whichdata].data_r2c2, 0, grid.x[3], y[3], 2);
  }

}