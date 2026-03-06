CommandType currentCarCommand = CMD_NONE;
unsigned long lastRecvTime = 0;
CommandType p1CoopCmd = CMD_NONE;
CommandType p2CoopCmd = CMD_NONE;
unsigned long p1LastRecv = 0;
unsigned long p2LastRecv = 0;

void stopCar()
{
    digitalWrite(AIN1_PIN, LOW);
    digitalWrite(AIN2_PIN, LOW);
    analogWrite(PWMA_PIN, 0);
    digitalWrite(BIN1_PIN, LOW);
    digitalWrite(BIN2_PIN, LOW);
    analogWrite(PWMB_PIN, 0);
}

void controlCarDigital(CommandType cmd)
{
    switch (cmd)
    {
    case CMD_UP:
        digitalWrite(AIN1_PIN, HIGH);
        digitalWrite(AIN2_PIN, LOW);
        analogWrite(PWMA_PIN, MOTOR_SPEED);
        digitalWrite(BIN1_PIN, HIGH);
        digitalWrite(BIN2_PIN, LOW);
        analogWrite(PWMB_PIN, MOTOR_SPEED);
        break;
    case CMD_DOWN:
        digitalWrite(AIN1_PIN, LOW);
        digitalWrite(AIN2_PIN, HIGH);
        analogWrite(PWMA_PIN, MOTOR_SPEED);
        digitalWrite(BIN1_PIN, LOW);
        digitalWrite(BIN2_PIN, HIGH);
        analogWrite(PWMB_PIN, MOTOR_SPEED);
        break;
    case CMD_RIGHT:
        digitalWrite(AIN1_PIN, LOW);
        digitalWrite(AIN2_PIN, HIGH);
        analogWrite(PWMA_PIN, TURN_SPEED);
        digitalWrite(BIN1_PIN, HIGH);
        digitalWrite(BIN2_PIN, LOW);
        analogWrite(PWMB_PIN, TURN_SPEED);
        break;
    case CMD_LEFT:
        digitalWrite(AIN1_PIN, HIGH);
        digitalWrite(AIN2_PIN, LOW);
        analogWrite(PWMA_PIN, TURN_SPEED);
        digitalWrite(BIN1_PIN, LOW);
        digitalWrite(BIN2_PIN, HIGH);
        analogWrite(PWMB_PIN, TURN_SPEED);
        break;
    default:
        stopCar();
        break;
    }
}

void runCarGame()
{
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 10);
    display.println("SOLO MODE");
    display.setTextSize(1);
    display.setCursor(0, 35);
    display.print("CMD: ");
    display.println(currentCarCommand);
    if (millis() - lastRecvTime > 200)
    {
        stopCar();
        display.setCursor(0, 50);
        display.print("Status: STOP");
    }
    else
    {
        controlCarDigital(currentCarCommand);
        display.setCursor(0, 50);
        display.print("Status: DRIVING");
    }
}

void updateCoopMotors()
{
    if (p1CoopCmd == CMD_UP)
    {
        digitalWrite(AIN1_PIN, HIGH);
        digitalWrite(AIN2_PIN, LOW);
        analogWrite(PWMA_PIN, MOTOR_SPEED);
    }
    else if (p1CoopCmd == CMD_DOWN)
    {
        digitalWrite(AIN1_PIN, LOW);
        digitalWrite(AIN2_PIN, HIGH);
        analogWrite(PWMA_PIN, MOTOR_SPEED);
    }
    else
    {
        digitalWrite(AIN1_PIN, LOW);
        digitalWrite(AIN2_PIN, LOW);
        analogWrite(PWMA_PIN, 0);
    }
    if (p2CoopCmd == CMD_UP)
    {
        digitalWrite(BIN1_PIN, HIGH);
        digitalWrite(BIN2_PIN, LOW);
        analogWrite(PWMB_PIN, MOTOR_SPEED);
    }
    else if (p2CoopCmd == CMD_DOWN)
    {
        digitalWrite(BIN1_PIN, LOW);
        digitalWrite(BIN2_PIN, HIGH);
        analogWrite(PWMB_PIN, MOTOR_SPEED);
    }
    else
    {
        digitalWrite(BIN1_PIN, LOW);
        digitalWrite(BIN2_PIN, LOW);
        analogWrite(PWMB_PIN, 0);
    }
}

void runCoopGame()
{
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 0);
    display.println("CO-OP TANK");
    display.setTextSize(1);
    display.setCursor(0, 25);
    display.print("P1 (L): ");
    if (millis() - p1LastRecv > 200)
    {
        display.print("STOP");
        p1CoopCmd = CMD_NONE;
    }
    else
    {
        if (p1CoopCmd == CMD_UP)
            display.print("FWD");
        else if (p1CoopCmd == CMD_DOWN)
            display.print("BCK");
        else
            display.print("");
    }
    display.setCursor(0, 40);
    display.print("P2 (R): ");
    if (millis() - p2LastRecv > 200)
    {
        display.print("STOP");
        p2CoopCmd = CMD_NONE;
    }
    else
    {
        if (p2CoopCmd == CMD_UP)
            display.print("FWD");
        else if (p2CoopCmd == CMD_DOWN)
            display.print("BCK");
        else
            display.print("");
    }
    display.setCursor(0, 55);
    display.print("Btn to Exit");
    updateCoopMotors();
}