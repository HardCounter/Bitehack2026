#define MAX_MEM_SEQ 10
CommandType memSequence[MAX_MEM_SEQ];
int memRound = 1, memCurrentLen = 3, memState = 0;
int p1MemIdx = 0, p2MemIdx = 0;
int memScoreP1 = 0, memScoreP2 = 0;
bool p1MemActive = true, p2MemActive = true;
volatile CommandType p1MemInput = CMD_NONE, p2MemInput = CMD_NONE;
unsigned long memTimer = 0;
int ledIndex = 0;

void drawArrow(CommandType dir, int x, int y)
{
    if (dir == CMD_UP)
        display.fillTriangle(x, y, x - 6, y + 10, x + 6, y + 10, SSD1306_WHITE);
    else if (dir == CMD_DOWN)
        display.fillTriangle(x, y + 10, x - 6, y, x + 6, y, SSD1306_WHITE);
    else if (dir == CMD_LEFT)
        display.fillTriangle(x - 6, y + 5, x + 4, y, x + 4, y + 10, SSD1306_WHITE);
    else if (dir == CMD_RIGHT)
        display.fillTriangle(x + 6, y + 5, x - 4, y, x - 4, y + 10, SSD1306_WHITE);
}

void runMemoryGame()
{
    unsigned long currentMillis = millis();
    if (memState == 0)
    {
        randomSeed(currentMillis);
        for (int i = 0; i < MAX_MEM_SEQ; i++)
            memSequence[i] = (CommandType)random(2, 6);
        p1MemIdx = 0;
        p2MemIdx = 0;
        p1MemActive = true;
        p2MemActive = true;
        memState = 1;
        memTimer = currentMillis;
        ledIndex = 0;
    }
    else if (memState == 1)
    {
        display.setTextSize(2);
        display.setCursor(30, 0);
        display.println("WATCH!");
        display.setTextSize(1);
        display.setCursor(10, 55);
        display.print("Round: ");
        display.print(memRound);
        if (ledIndex < memCurrentLen)
        {
            if (currentMillis - memTimer < 800)
                drawArrow(memSequence[ledIndex], 64, 30);
            if (currentMillis - memTimer > 1000)
            {
                memTimer = currentMillis;
                ledIndex++;
                playToneAsync(400, 50);
            }
        }
        else
        {
            if (currentMillis - memTimer > 500)
            {
                playToneAsync(800, 200);
                p1MemInput = CMD_NONE;
                p2MemInput = CMD_NONE;
                memState = 2;
            }
        }
    }
    else if (memState == 2)
    {
        display.setTextSize(2);
        display.setCursor(25, 0);
        display.println("REPEAT!");
        display.setTextSize(1);
        display.setCursor(5, 25);
        display.print("P1: ");
        if (!p1MemActive)
            display.print("FAIL");
        else
        {
            display.print(p1MemIdx);
            display.print("/");
            display.print(memCurrentLen);
        }
        display.setCursor(70, 25);
        display.print("P2: ");
        if (!p2MemActive)
            display.print("FAIL");
        else
        {
            display.print(p2MemIdx);
            display.print("/");
            display.print(memCurrentLen);
        }

        if (p1MemActive && p1MemInput != CMD_NONE)
        {
            if (p1MemInput == memSequence[p1MemIdx])
            {
                memScoreP1++;
                p1MemIdx++;
                playToneAsync(NOTE_E3, 30);
                if (p1MemIdx >= memCurrentLen)
                    p1MemActive = false;
            }
            else
            {
                p1MemActive = false;
                playToneAsync(100, 100);
            }
            p1MemInput = CMD_NONE;
        }
        if (p2MemActive && p2MemInput != CMD_NONE)
        {
            if (p2MemInput == memSequence[p2MemIdx])
            {
                memScoreP2++;
                p2MemIdx++;
                if (p2MemIdx >= memCurrentLen)
                    p2MemActive = false;
            }
            else
            {
                p2MemActive = false;
            }
            p2MemInput = CMD_NONE;
        }
        bool p1Finished = (p1MemIdx >= memCurrentLen) || (p1MemActive == false && p1MemIdx < memCurrentLen);
        bool p2Finished = (p2MemIdx >= memCurrentLen) || (p2MemActive == false && p2MemIdx < memCurrentLen);
        if (p1Finished && p2Finished)
        {
            memState = 3;
            memTimer = currentMillis;
        }
    }
    else if (memState == 3)
    {
        display.setTextSize(1);
        display.setCursor(10, 10);
        display.println("ROUND OVER");
        display.setCursor(10, 30);
        display.print("P1 Score: ");
        display.print(memScoreP1);
        display.setCursor(10, 40);
        display.print("P2 Score: ");
        display.print(memScoreP2);
        if (currentMillis - memTimer > 2000)
        {
            memCurrentLen++;
            memRound++;
            if (memCurrentLen > 7)
            {
                if (memScoreP1 > memScoreP2)
                    winnerId = 1;
                else if (memScoreP2 > memScoreP1)
                    winnerId = 2;
                else
                    winnerId = 0;
                currentState = GAME_OVER;
            }
            else
            {
                memState = 0;
            }
        }
    }
}