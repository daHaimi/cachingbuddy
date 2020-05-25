/**
 * Initialize the compass
 * "Do a barrel roll"
 */
void initCompass() {
  compass.begin();
  for (uint8_t i = 0; i < LED_COUNT; i++) {
    compass.setPixelColor(activeLed, compass.Color(0, 0, 0));
    activeLed = i;
    compass.setPixelColor(activeLed, getColor((1.0/24) * i));
    compass.show();
    delay(50);
  }
}

/**
 * Calculate color of the active LED
 * * If further away than MAX_DISTANCE, the color is COLOR_FAR_AWAY
 * * Else lerp between the predefined steps
 */
uint32_t getColor(float distancePercentage) {
  if (distancePercentage >= 1.0) {
    return COLOR_FAR_AWAY;
  }
  if (distancePercentage == 0.0) {
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
    ((uint8_t)(lower >> 16)) * (1.0 - rangePercent) + ((uint8_t)(upper >> 16)) * rangePercent * LED_BRIGHTNESS,
    ((uint8_t)(lower >> 8)) * (1.0 - rangePercent) + ((uint8_t)(upper >> 8)) * rangePercent * LED_BRIGHTNESS,
    ((uint8_t)lower) * (1.0 - rangePercent) + ((uint8_t)upper) * rangePercent * LED_BRIGHTNESS);
  return color;
}

/**
 * Update the compass: Choose new LED according to the course and color to the distance
 */
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

/**
 * Called from the main loop: Update the compass to the current values
 */
void loopCompass() {
  double distPercentage = (double)current_search.distance / MAX_DISTANCE;
  updateCompass(current_search.course, distPercentage, gps.location.isValid());
}
