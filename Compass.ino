
void initCompass() {
  compass.begin();
  compass.setBrightness(50);
  for (uint8_t i = 0; i < LED_COUNT; i++) {
    compass.setPixelColor(activeLed, compass.Color(0, 0, 0));
    activeLed = i;
    compass.setPixelColor(activeLed, getColor((1.0/24) * i));
    compass.show();
    delay(50);
  }
}

uint32_t getColor(float distancePercentage) {
  float steps[] = {0.0, .5, 1.0};
  uint32_t stepColors[] = {0xff0000, 0xffff00, 0x00ff00};
  if (distancePercentage >= 1.0) {
    return 0x0000ff;
  }
  if (distancePercentage == 0.0) {
    // Extrawurst
    return stepColors[0];
  }
  uint8_t i, l, r, g, b;
  float range, rangePercent;
  uint32_t lower, upper;
  for (i = 0; i < STEP_NUM; i++) {
    if (distancePercentage < steps[i]) {
      break;
    }
  }
  lower = stepColors[i - 1];
  upper = stepColors[i];
  range = steps[i] - steps[i - 1];
  rangePercent = (distancePercentage - steps[i - 1]) / range;
  r = lower >> 16;
  g = lower >> 8;
  b = lower;
  uint32_t color = compass.Color(
    ((uint8_t)(lower >> 16)) * (1.0 - rangePercent) + ((uint8_t)(upper >> 16)) * rangePercent,
    ((uint8_t)(lower >> 8)) * (1.0 - rangePercent) + ((uint8_t)(upper >> 8)) * rangePercent,
    ((uint8_t)lower) * (1.0 - rangePercent) + ((uint8_t)upper) * rangePercent);
  return color;
}

void updateCompass(double course, double distancePercentage, bool valid) {
  compass.setPixelColor(activeLed, compass.Color(0, 0, 0));
  activeLed = floor(course / DEGREE_PER_LED);
  compass.setPixelColor(activeLed, getColor(1 - distancePercentage));
  compass.show();
  if (!valid) {
    compass.setPixelColor(activeLed, compass.Color(0, 0, 0));
    compass.show();
  }
}

void loopCompass() {
  double distPercentage = (double)current_search.distance / MAX_DISTANCE;
  updateCompass(current_search.course, distPercentage, gps.location.isValid());
}
