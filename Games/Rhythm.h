#define TARGET_LINE_Y 55
#define HIT_WINDOW 10
#define NOTE_SPEED 2
#define LANE_1_X 32
#define LANE_2_X 96
#define RHYTHM_WIN_SCORE 25
#define MAX_MISSES 3

int rScoreP1 = 0, rScoreP2 = 0, rMissesP1 = 0, rMissesP2 = 0;
struct Note
{
    float y;
    int lane;
    bool active;
};
#define MAX_NOTES 12
Note notes[MAX_NOTES];
unsigned long lastSpawnTime = 0;
int spawnInterval = 1000;

void resetRhythmGame()
{
    rScoreP1 = 0;
    rScoreP2 = 0;
    rMissesP1 = 0;
    rMissesP2 = 0;
    for (int i = 0; i < MAX_NOTES; i++)
        notes[i].active = false;
    p1GameClick = false;
    p2GameClick = false;
}
void addMiss(int lane)
{
    if (lane == 1 && rMissesP1 < MAX_MISSES)
        rMissesP1++;
    if (lane == 2 && rMissesP2 < MAX_MISSES)
        rMissesP2++;
}
void spawnNote()
{
    int targetLane = random(1, 3);
    if (targetLane == 1 && rMissesP1 >= MAX_MISSES)
        targetLane = 2;
    if (targetLane == 2 && rMissesP2 >= MAX_MISSES)
        targetLane = 1;
    if ((targetLane == 1 && rMissesP1 >= MAX_MISSES) || (targetLane == 2 && rMissesP2 >= MAX_MISSES))
        return;
    for (int i = 0; i < MAX_NOTES; i++)
    {
        if (!notes[i].active)
        {
            notes[i].active = true;
            notes[i].y = 0;
            notes[i].lane = targetLane;
            break;
        }
    }
}

void drawRhythmGame()
{
    display.drawLine(0, TARGET_LINE_Y, SCREEN_WIDTH, TARGET_LINE_Y, SSD1306_WHITE);
    display.drawFastVLine(SCREEN_WIDTH / 2, 0, SCREEN_HEIGHT, SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(2, 0);
    display.print("P1: ");
    display.print(rScoreP1);
    display.setCursor(66, 0);
    display.print("P2: ");
    display.print(rScoreP2);
    display.setCursor(2, 10);
    for (int i = 0; i < rMissesP1; i++)
        display.print("x");
    display.setCursor(66, 10);
    for (int i = 0; i < rMissesP2; i++)
        display.print("x");
    for (int i = 0; i < MAX_NOTES; i++)
    {
        if (notes[i].active)
        {
            int drawX = (notes[i].lane == 1) ? LANE_1_X : LANE_2_X;
            if (abs((int)notes[i].y - TARGET_LINE_Y) <= HIT_WINDOW)
                display.fillRect(drawX, (int)notes[i].y, 10, 6, SSD1306_WHITE);
            else
                display.drawRect(drawX, (int)notes[i].y, 10, 6, SSD1306_WHITE);
        }
    }
    if (rMissesP1 >= MAX_MISSES)
    {
        display.drawLine(0, 0, 64, 64, SSD1306_WHITE);
        display.drawLine(0, 64, 64, 0, SSD1306_WHITE);
    }
    if (rMissesP2 >= MAX_MISSES)
    {
        display.drawLine(64, 0, 128, 64, SSD1306_WHITE);
        display.drawLine(64, 64, 128, 0, SSD1306_WHITE);
    }
}

void runRhythmGame()
{
    if (rScoreP1 >= RHYTHM_WIN_SCORE)
    {
        winnerId = 1;
        currentState = GAME_OVER;
        return;
    }
    if (rScoreP2 >= RHYTHM_WIN_SCORE)
    {
        winnerId = 2;
        currentState = GAME_OVER;
        return;
    }
    if (rMissesP1 >= MAX_MISSES && rMissesP2 >= MAX_MISSES)
    {
        winnerId = 0;
        currentState = GAME_OVER;
        return;
    }
    if (millis() - lastSpawnTime > spawnInterval)
    {
        spawnNote();
        lastSpawnTime = millis();
        spawnInterval = random(300, 1100);
    }
    for (int i = 0; i < MAX_NOTES; i++)
    {
        if (notes[i].active)
        {
            notes[i].y += NOTE_SPEED;
            if (notes[i].y > SCREEN_HEIGHT)
            {
                notes[i].active = false;
            }
        }
    }

    if (p1GameClick)
    {
        bool hit = false;
        if (rMissesP1 < MAX_MISSES)
        {
            for (int i = 0; i < MAX_NOTES; i++)
            {
                if (notes[i].active && notes[i].lane == 1 && abs((int)notes[i].y - TARGET_LINE_Y) <= HIT_WINDOW)
                {
                    rScoreP1++;
                    notes[i].active = false;
                    hit = true;
                    break;
                }
            }
            if (!hit)
                addMiss(1);
        }
        p1GameClick = false;
    }
    if (p2GameClick)
    {
        bool hit = false;
        if (rMissesP2 < MAX_MISSES)
        {
            for (int i = 0; i < MAX_NOTES; i++)
            {
                if (notes[i].active && notes[i].lane == 2 && abs((int)notes[i].y - TARGET_LINE_Y) <= HIT_WINDOW)
                {
                    rScoreP2++;
                    notes[i].active = false;
                    hit = true;
                    break;
                }
            }
            if (!hit)
                addMiss(2);
        }
        p2GameClick = false;
    }
    drawRhythmGame();
}