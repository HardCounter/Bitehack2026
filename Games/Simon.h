const int PATTERN_LENGTH = 10;
long beatPattern[PATTERN_LENGTH] = {0, 500, 1000, 1250, 1500, 2000, 2250, 2500, 3000, 3500};
int beatType[PATTERN_LENGTH] = {1, 2, 1, 1, 2, 1, 1, 2, 1, 2};
int simonState = 0, simonPlayIndex = 0;
unsigned long simonStartTime = 0;
long p1Inputs[PATTERN_LENGTH], p2Inputs[PATTERN_LENGTH];
int p1InputCount = 0, p2InputCount = 0;
bool p1Finished = false, p2Finished = false;

void runSimonGame()
{
    unsigned long currentMillis = millis();
    if (simonState == 0)
    {
        display.setTextSize(2);
        display.setCursor(30, 20);
        display.println("LISTEN!");
        if (currentMillis >= simonStartTime)
        {
            if (simonPlayIndex < PATTERN_LENGTH)
            {
                if (currentMillis - simonStartTime >= beatPattern[simonPlayIndex])
                {
                    if (beatType[simonPlayIndex] == 1)
                    {
                        display.fillCircle(64, 50, 10, SSD1306_WHITE);
                        playSoundKick();
                    }
                    else
                    {
                        display.fillRect(54, 45, 20, 10, SSD1306_WHITE);
                        playSoundSnare();
                    }
                    simonPlayIndex++;
                }
            }
            else if (currentMillis - simonStartTime > beatPattern[PATTERN_LENGTH - 1] + 500)
            {
                simonState = 1;
                p1InputCount = 0;
                p2InputCount = 0;
                p1Finished = false;
                p2Finished = false;
                for (int i = 0; i < PATTERN_LENGTH; i++)
                {
                    p1Inputs[i] = 0;
                    p2Inputs[i] = 0;
                }
                simonStartTime = 0;
                p1GameClick = false;
                p2GameClick = false;
            }
        }
    }
    else if (simonState == 1)
    {
        display.setTextSize(2);
        display.setCursor(20, 5);
        display.println("REPEAT!");
        display.setTextSize(1);
        display.setCursor(5, 30);
        display.print("P1: ");
        if (p1Finished)
            display.print("DONE");
        else
        {
            display.print(p1InputCount);
            display.print("/");
            display.print(PATTERN_LENGTH);
        }
        display.setCursor(70, 30);
        display.print("P2: ");
        if (p2Finished)
            display.print("DONE");
        else
        {
            display.print(p2InputCount);
            display.print("/");
            display.print(PATTERN_LENGTH);
        }

        if (simonStartTime == 0 && (p1GameClick || p2GameClick))
            simonStartTime = currentMillis;

        if (p1GameClick && !p1Finished)
        {
            p1Inputs[p1InputCount] = currentMillis - simonStartTime;
            p1InputCount++;
            if (p1InputCount >= PATTERN_LENGTH)
                p1Finished = true;
            p1GameClick = false;
        }
        if (p2GameClick && !p2Finished)
        {
            p2Inputs[p2InputCount] = currentMillis - simonStartTime;
            p2InputCount++;
            if (p2InputCount >= PATTERN_LENGTH)
                p2Finished = true;
            p2GameClick = false;
        }

        if (simonStartTime > 0 && (currentMillis - simonStartTime > 5000))
            simonState = 2;
        if (p1Finished && p2Finished)
        {
            simonState = 2;
            delay(500);
        }
    }
    else if (simonState == 2)
    {
        int p1DiffTotal = 0, p2DiffTotal = 0;
        for (int i = 0; i < PATTERN_LENGTH; i++)
        {
            if (i < p1InputCount)
                p1DiffTotal += abs(p1Inputs[i] - beatPattern[i]);
            else
                p1DiffTotal += 500;
            if (i < p2InputCount)
                p2DiffTotal += abs(p2Inputs[i] - beatPattern[i]);
            else
                p2DiffTotal += 500;
        }
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.println("TOTAL ERROR (ms):");
        display.setCursor(10, 20);
        display.print("P1: ");
        display.print(p1DiffTotal);
        display.setCursor(10, 35);
        display.print("P2: ");
        display.print(p2DiffTotal);
        display.setCursor(10, 50);
        if (p1DiffTotal < p2DiffTotal)
            display.print(">> P1 WINS! <<");
        else if (p2DiffTotal < p1DiffTotal)
            display.print(">> P2 WINS! <<");
        else
            display.print(">> DRAW! <<");

        if (p1GameClick || p2GameClick)
        {
            p1GameClick = false;
            p2GameClick = false;
            currentState = MAIN_MENU;
            delay(300);
        }
    }
}