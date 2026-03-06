void handleMenu()
{
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 0);
  display.println(" MAIN MENU ");
  display.setCursor(10, 12);
  if (menuSelection == 0)
    display.print(">");
  else
    display.print(" ");
  display.print(" Catchy Blocks");
  display.setCursor(10, 21);
  if (menuSelection == 1)
    display.print(">");
  else
    display.print(" ");
  display.print(" F1 Reaction");
  display.setCursor(10, 30);
  if (menuSelection == 2)
    display.print(">");
  else
    display.print(" ");
  display.print(" RC Classic");
  display.setCursor(10, 39);
  if (menuSelection == 3)
    display.print(">");
  else
    display.print(" ");
  display.print(" RC CO-OP");
  display.setCursor(10, 48);
  if (menuSelection == 4)
    display.print(">");
  else
    display.print(" ");
  display.print(" Echo Game");
  display.setCursor(10, 57);
  if (menuSelection == 5)
    display.print(">");
  else
    display.print(" ");
  display.print(" Memory Game");

  if (wirelessMenuMove == -1)
  {
    menuSelection--;
    if (menuSelection < 0)
      menuSelection = 5;
    wirelessMenuMove = 0;
  }
  else if (wirelessMenuMove == 1)
  {
    menuSelection++;
    if (menuSelection > 5)
      menuSelection = 0;
    wirelessMenuMove = 0;
  }

  if (wirelessConfirm)
  {
    wirelessConfirm = false;
    if (millis() < ignoreInputUntil)
      return;

    if (menuSelection == 0)
    {
      resetRhythmGame();
      currentState = PLAYING_RHYTHM;
    }
    else if (menuSelection == 1)
    {
      resetF1Game();
      f1Timer = millis();
      currentState = PLAYING_F1;
    }
    else if (menuSelection == 2)
    {
      currentState = PLAYING_CAR;
      lastRecvTime = millis();
      ignoreInputUntil = millis() + 500;
    }
    else if (menuSelection == 3)
    {
      currentState = PLAYING_COOP;
      p1LastRecv = millis();
      p2LastRecv = millis();
      ignoreInputUntil = millis() + 500;
    }
    else if (menuSelection == 4)
    {
      currentState = PLAYING_SIMON;
      simonState = 0;
      simonPlayIndex = 0;
      simonStartTime = millis() + 800;
      ignoreInputUntil = millis() + 1000;
    }
    else if (menuSelection == 5)
    {
      currentState = PLAYING_MEMORY;
      memScoreP1 = 0;
      memScoreP2 = 0;
      memRound = 1;
      memCurrentLen = 3;
      memState = 0;
      ignoreInputUntil = millis() + 1000;
    }
  }
}