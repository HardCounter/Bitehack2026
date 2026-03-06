unsigned long toneTimeout = 0;

void updateAudio()
{
  if (toneTimeout > 0 && millis() >= toneTimeout)
  {
    ledcWrite(SOUND_CHANNEL, 0);
    toneTimeout = 0;
  }
}
void playToneAsync(int freq, int duration)
{
  ledcWriteTone(SOUND_CHANNEL, freq);
  ledcWrite(SOUND_CHANNEL, 100);
  toneTimeout = millis() + duration;
}
void playSoundKick()
{
  ledcWrite(SOUND_CHANNEL, 200);
  for (int f = 100; f > 40; f -= 4)
  {
    ledcWriteTone(SOUND_CHANNEL, f);
    delay(2);
  }
  ledcWrite(SOUND_CHANNEL, 0);
}
void playSoundSnare()
{
  ledcWriteTone(SOUND_CHANNEL, 1500);
  ledcWrite(SOUND_CHANNEL, 150);
  toneTimeout = millis() + 40;
}