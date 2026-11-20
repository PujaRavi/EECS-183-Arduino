// displays Level
void print_level(int level) {

  matrix.fillScreen(BLACK.to_333());
  matrix.setTextSize(1);
  matrix.setTextColor(WHITE.to_333());
  matrix.setCursor(0,0);
  matrix.print("level");

  matrix.setCursor(0,8);
  matrix.print(level);

  delay(200);
  matrix.fillScreen(BLACK.to_333());
}

// displays number of lives
void print_lives(int lives) {
  matrix.fillScreen(BLACK.to_333());
  matrix.setTextSize(1);
  matrix.setTextColor(WHITE.to_333());
  matrix.setCursor(0,0);
  matrix.print("lives");

  matrix.setCursor(0,8);
  matrix.print(lives);

  delay(200);
  matrix.fillScreen(BLACK.to_333());
}

// displays "game over"
void game_over() {
  matrix.fillScreen(BLACK.to_333());
  matrix.setTextSize(1);
  matrix.setTextColor(WHITE.to_333());
  matrix.setCursor(0,0);
  matrix.print("game over");
}

