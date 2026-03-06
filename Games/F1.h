enum F1State
{
    F1_RESET,
    F1_SEQUENCE,
    F1_WAIT_BLACKOUT,
    F1_REACTION,
    F1_ROUND_RESULT
};
F1State f1State = F1_RESET;
int f1ScoreP1 = 0, f1ScoreP2 = 0;
const int POINTS_TO_WIN = 3;
int redColor = 255, greenColor = 100, blueColor = 0;
unsigned long f1Timer = 0, f1RandomDelay = 0, f1ReactionStart = 0;
long p1ReactionTime = -1, p2ReactionTime = -1;
String roundWinnerMsg = "";

void resetF1Game()
{
    f1ScoreP1 = 0;
    f1ScoreP2 = 0;
    f1State = F1_RESET;
    ring.clear();
    ring.show();
}

void runF1Game()
{
    unsigned long currentMillis = millis();
    switch (f1State)
    {
    case F1_RESET:
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(30, 20);
        display.println("GET READY!");
        display.setCursor(10, 40);
        display.print("P1: ");
        display.print(f1ScoreP1);
        display.print("  P2: ");
        display.print(f1ScoreP2);
        p1GameClick = false;
        p2GameClick = false;
        p1ReactionTime = -1;
        p2ReactionTime = -1;
        ring.clear();
        ring.show();
        if (currentMillis - f1Timer > 2000)
        {
            f1State = F1_SEQUENCE;
            ledIndex = 0;
            f1Timer = currentMillis;
        }
        break;
    case F1_SEQUENCE:
        if (p1GameClick)
        {
            roundWinnerMsg = "P1 FALSE START!";
            f1ScoreP2++;
            f1State = F1_ROUND_RESULT;
            f1Timer = currentMillis;
            return;
        }
        if (p2GameClick)
        {
            roundWinnerMsg = "P2 FALSE START!";
            f1ScoreP1++;
            f1State = F1_ROUND_RESULT;
            f1Timer = currentMillis;
            return;
        }
        if (currentMillis - f1Timer > 500)
        {
            if (ledIndex < NUM_LEDS)
            {
                ring.setPixelColor(ledIndex, ring.Color(redColor, greenColor, blueColor));
                ring.show();
                ledIndex++;
                f1Timer = currentMillis;
            }
            else
            {
                f1State = F1_WAIT_BLACKOUT;
                f1RandomDelay = random(1000, 4000);
                f1Timer = currentMillis;
            }
        }
        display.setTextSize(2);
        display.setCursor(10, 25);
        display.print("HOLD...");
        break;
    case F1_WAIT_BLACKOUT:
        if (p1GameClick)
        {
            roundWinnerMsg = "P1 FALSE START!";
            f1ScoreP2++;
            f1State = F1_ROUND_RESULT;
            f1Timer = currentMillis;
            return;
        }
        if (p2GameClick)
        {
            roundWinnerMsg = "P2 FALSE START!";
            f1ScoreP1++;
            f1State = F1_ROUND_RESULT;
            f1Timer = currentMillis;
            return;
        }
        if (currentMillis - f1Timer > f1RandomDelay)
        {
            ring.clear();
            ring.show();
            f1ReactionStart = currentMillis;
            f1State = F1_REACTION;
        }
        display.setTextSize(2);
        display.setCursor(10, 25);
        display.print("HOLD...");
        break;
    case F1_REACTION:
        if (p1GameClick && p1ReactionTime == -1)
            p1ReactionTime = currentMillis - f1ReactionStart;
        if (p2GameClick && p2ReactionTime == -1)
            p2ReactionTime = currentMillis - f1ReactionStart;
        if (p1ReactionTime != -1 && p2ReactionTime != -1)
        {
            if (p1ReactionTime < p2ReactionTime)
            {
                f1ScoreP1++;
                roundWinnerMsg = "P1 WINS ROUND!";
            }
            else
            {
                f1ScoreP2++;
                roundWinnerMsg = "P2 WINS ROUND!";
            }
            f1State = F1_ROUND_RESULT;
            f1Timer = currentMillis;
        }
        display.setTextSize(2);
        display.setCursor(30, 10);
        display.println("GO!!");
        display.setTextSize(1);
        display.setCursor(0, 40);
        if (p1ReactionTime != -1)
        {
            display.print("P1: ");
            display.print(p1ReactionTime);
            display.print("ms");
        }
        else
            display.print("P1: ...");
        display.setCursor(0, 50);
        if (p2ReactionTime != -1)
        {
            display.print("P2: ");
            display.print(p2ReactionTime);
            display.print("ms");
        }
        else
            display.print("P2: ...");
        break;
    case F1_ROUND_RESULT:
        display.setTextSize(1);
        display.setCursor(10, 10);
        display.println(roundWinnerMsg);
        display.setCursor(10, 30);
        display.print("P1 Time: ");
        display.print(p1ReactionTime);
        display.setCursor(10, 40);
        display.print("P2 Time: ");
        display.print(p2ReactionTime);
        display.setTextSize(2);
        display.setCursor(10, 55);
        display.print(f1ScoreP1);
        display.print(" - ");
        display.print(f1ScoreP2);
        if (currentMillis - f1Timer > 3000)
        {
            if (f1ScoreP1 >= POINTS_TO_WIN)
            {
                winnerId = 1;
                currentState = GAME_OVER;
            }
            else if (f1ScoreP2 >= POINTS_TO_WIN)
            {
                winnerId = 2;
                currentState = GAME_OVER;
            }
            else
            {
                f1State = F1_RESET;
                f1Timer = currentMillis;
            }
        }
        break;
    }
}