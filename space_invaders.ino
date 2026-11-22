#include <gamma.h>
#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

// define the wiring of the LED screen
const uint8_t CLK  = 8;
const uint8_t LAT = A3;
const uint8_t OE = 9;
const uint8_t A = A0;
const uint8_t B = A1;
const uint8_t C = A2;

// define the wiring of the inputs
const int POTENTIOMETER_PIN_NUMBER = 5;
const int BUTTON_PIN_NUMBER = 10;

// global constant for the number of Invaders in the game
const int NUM_ENEMIES = 16;
const int ROW_SIZE = 8;
const int GAME_TICK = 1000;
const int BALL_TICK = 200;

// a global variable that represents the LED screen
RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);

// the following functions are for printing messages
void print_level(int level);
void print_lives(int lives);
void game_over();

class Color {
  public:
    int red;
    int green;
    int blue;
    Color() {
      red = 0;
      green = 0;
      blue = 0;
    }
    Color(int r, int g, int b) {
      red = r;
      green = g;
      blue = b;
    }
    uint16_t to_333() const {
      return matrix.Color333(red, green, blue);
    }
};

const Color BLACK(0, 0, 0);
const Color RED(4, 0, 0);
const Color ORANGE(6, 1, 0);
const Color YELLOW(4, 4, 0);
const Color GREEN(0, 4, 0);
const Color BLUE(0, 0, 4);
const Color PURPLE(1, 0, 2);
const Color WHITE(4, 4, 4);
const Color LIME(2, 4, 0);
const Color AQUA(0, 4, 4);

class Invader {
  public:
    // Constructors
    Invader() {
      x = 0;
      y = 0;
      strength = 0;
    }
    // sets values for private date members x and y
    Invader(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    // sets values for private data members
    Invader(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }
    // sets values for private data members
    void initialize(int x_arg, int y_arg, int strength_arg) {
      x = x_arg;
      y = y_arg;
      strength = strength_arg;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_strength() const {
      return strength;
    }

    // Moves the Invader down the screen by one row
    // Modifies: y
    void move() {
      y++;
    }
    
    // draws the Invader if its strength is greater than 0
    // calls: draw_with_rgb
    void draw() {
      if (strength == 1){
        draw_with_rgb(RED, BLUE);
      }
      else if (strength == 2){
        draw_with_rgb(ORANGE, BLUE);
      }
      else if (strength == 3){
        draw_with_rgb(YELLOW, BLUE);
      }
      else if (strength == 4){
        draw_with_rgb(GREEN, BLUE);
      }
      else if (strength == 5){
        draw_with_rgb(BLUE, BLUE);
      }
      else if (strength == 6){
        draw_with_rgb(PURPLE, BLUE);
      }
      else if (strength == 7){
        draw_with_rgb(WHITE, BLUE);
      }
    }
    
    // draws black where the Invader used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK, BLACK);
    }    
    
    // Invader is hit by a Cannonball.
    // Modifies: strength
    // calls: draw, erase
    void hit() {
      strength--;
      if(strength > 0){
        draw();
      }
      else{
        erase();
      }

    }

  private:
    int x;
    int y;
    int strength;
    
    // draws the Invader
    void draw_with_rgb(Color body_color, Color eye_color) {
      int start_x = get_x();
      int start_y = get_y();
                matrix.drawPixel(start_x, start_y, BLACK.to_333());
                matrix.drawPixel(start_x+1, start_y, body_color.to_333());
                matrix.drawPixel(start_x+2, start_y, body_color.to_333());
                matrix.drawPixel(start_x+3, start_y, BLACK.to_333());
                matrix.drawPixel(start_x, start_y+1,  body_color.to_333());
                matrix.drawPixel(start_x+1, start_y+1, eye_color.to_333());
                matrix.drawPixel(start_x+2, start_y+1, eye_color.to_333());
                matrix.drawPixel(start_x+3, start_y+1, body_color.to_333());
                matrix.drawPixel(start_x, start_y+2,  body_color.to_333());
                matrix.drawPixel(start_x+1, start_y+2, body_color.to_333());
                matrix.drawPixel(start_x+2, start_y+2, body_color.to_333());
                matrix.drawPixel(start_x+3, start_y+2, body_color.to_333());
                matrix.drawPixel(start_x, start_y+3,  body_color.to_333());
                matrix.drawPixel(start_x+1, start_y+3, BLACK.to_333());
                matrix.drawPixel(start_x+2, start_y+3, BLACK.to_333());
                matrix.drawPixel(start_x+3, start_y+3, body_color.to_333());
          }
};

class Cannonball {
  public:
    // Ball starts at (0, 0) and is not flying
    Cannonball() {
      x = 0;
      y = 0;
      fired = false;
    }
    
    // resets private data members to initial values
    // Puts ball back to how it started (not flying, 0,0)
    void reset() {
      x = 0;
      y = 0;
      fired = false;
    }
    
    // getters
    // Returns ball's column (left/ right position)
    int get_x() const {
      return x;
    }
    // Returns ball's row (up/down position)
    int get_y() const {
      return y;
    }
    // Returns true if ball is currently flying
    // Denies second ball mid shot
    bool has_been_fired() const {
      return fired;
    }
    
    // sets private data members
    // Launches ball from a starting spot
    // Game calls this when button pressed, passing in spot
    // above cannon's barrel
    void fire(int x_arg, int y_arg) {
      if (!fired) {
        x = x_arg;
        y = y_arg;
        fired = true;
      }
    }
    
    // moves the Cannonball and detects if it goes off the screen
    // Modifies: y, fired
    // Moves ball up one row each time called
    // Checks if ball flew off top of screen
    void move() {
      if (!fired) {
        return;
      }

      y = y - 1;

      // ball is 2 LED tall this is lower one
      int bottomLED = y + 1; 
      if (bottomLED < 0) {
        fired = false;
      }
    }
    
    // resets private data members to initial values
    // Called when ball hits invader
    // Ball is gone, so reset it
    void hit() {
      reset();
    }
    
    // draws the Cannonball, if it is fired
    // Lights up ball on screen
    void draw() {
      if (fired) {
        matrix.drawPixel(x, y, ORANGE.to_333());
        matrix.drawPixel(x, y + 1, ORANGE.to_333());
      }
    }
    
    // draws black where the Cannonball used to be
    // Turns off ball's LED
    // Must be called before move(), or ball will leave
    // orange trail
    void erase() {
      matrix.drawPixel(x, y, BLACK.to_333());
      matrix.drawPixel(x, y + 1, BLACK.to_333());
    }

  private:
    // y is the coordinate of the top of the cannonball
    int x;
    int y;
    bool fired;
};

class Player {
  public:
    Player() {
      x = 0;
      y = 14;
      lives = 3;
    }
    
    // getters
    int get_x() const {
      return x;
    }
    int get_y() const {
      return y;
    }
    int get_lives() const {
      return lives;
    }
    
    // setter
    void set_x(int x_arg) {
      if(x_arg < 1) {
        x = 1;
      }
      else if (x_arg > 30) {
        x = 30;
      }
      else {
        x = x_arg;
      }
    }

    void set_lives(int lives_arg) {
        if (lives_arg >= 0) {
          lives = lives_arg;
        }
    
    }
    
    // Modifies: lives
    void die() {
      lives--;
      if (lives < 1) {
        game_over();
      }
    }
    
    // draws the Player
    // calls: draw_with_rgb
    void draw() {
      draw_with_rgb(AQUA);
    }
    
    // draws black where the Player used to be
    // calls: draw_with_rgb
    void erase() {
      draw_with_rgb(BLACK);
    }

  private:
    // The x,y coordinate is the top center piece of the player
    int x;
    int y;
    int lives;

    // sets private data members x and y to initial values
    void initialize(int x_arg, int y_arg) {
      x = x_arg;
      y = y_arg;
    }
    
    // draws the player
    void draw_with_rgb(Color color) {
      matrix.drawPixel(x, y, color.to_333());
      matrix.drawPixel(x, y+1, color.to_333());
      matrix.drawPixel(x-1,y+1, color.to_333());
      matrix.drawPixel(x+1,y+1, color.to_333());

    }
};

class Game {
  public:
    Game() {
      level = 0;
    }
    
    // sets up a new game of Space Invaders
    // Modifies: global variable matrix
    void setup_game() {
      level = 0;
      player.set_lives(3);

      enemy_time = millis();
      ball_time = millis();

      reset_level();
    }
    
    // advances the game simulation one step and renders the graphics
    // see spec for details of game
    // Modifies: global variable matrix
    void update(int potentiometer_value, bool button_pressed) {

      // Update player position
      int player_x = map(potentiometer_value, 0, 1023, 1, 30);

      if (player_x != player.get_x()) {
        player.erase();
        player.set_x(player_x);
      }

      // Fire cannonball
      if (!ball.has_been_fired() && button_pressed) {
        ball.fire(player_x, 13);
        ball_time = millis();
      }

      // Cannonball tick
      if (ball.has_been_fired() &&
          millis() - ball_time >= BALL_TICK) {

          ball.erase();
          ball.move();

          ball_time = millis();
      }

      // Check invader collisions
      for (int i = 0; i < NUM_ENEMIES; i++) {

        if (enemies[i].get_strength() > 0) {

          int enemy_y = enemies[i].get_y();
          int enemy_x = enemies[i].get_x();

          // Check for invader reaches bottom row
          if (enemy_y >= 12) {
            level--;
            player.die();
            reset_level();
            return;
          }
          
          // Check for invader-player collision
          if (enemy_y >= 11 && (enemy_x == player_x || enemy_x + 3 == player_x)) {

            level--;
            player.die();
            reset_level();
            return;
          }
          
          // Check for invader-ball collision
          if (ball.has_been_fired()) {
            int x_difference = ball.get_x() - enemy_x;
          
            if ((x_difference == 0 || x_difference == 3) && ball.get_y() <= enemy_y + 3) {

              enemies[i].hit();
              ball.hit();

            } else if ((x_difference == 1 || x_difference == 2) && ball.get_y() <= enemy_y + 2) {

              enemies[i].hit();
              ball.hit();
            }
          }
        }
      }
      
      // Invader tick
      if (millis() - enemy_time >= GAME_TICK) {

        bool bottom_row_alive = false;

        // Determine whether bottom row still has invaders
        for (int i = ROW_SIZE; i < NUM_ENEMIES; i++) {
          if (enemies[i].get_strength() > 0) {
            bottom_row_alive = true;
            break;
          }
        }

        // Move the appropriate row
        for (int i = 0; i < NUM_ENEMIES; i++) {

          if (bottom_row_alive && i >= ROW_SIZE) {

            if (enemies[i].get_strength() > 0) {
              enemies[i].erase();
              enemies[i].move();
            }

          } else if (!bottom_row_alive && i < ROW_SIZE) {

            if (enemies[i].get_strength() > 0) {
            enemies[i].erase();
            enemies[i].move();
            }
          }
        }

        enemy_time = millis();
      }

      // Check whether level is cleared
      if (level_cleared()) {
        reset_level();
      }
      
      // Draw everything
      for (int i = 0; i < NUM_ENEMIES; i++) {
        enemies[i].draw();
      }

      player.draw();
      ball.draw();
  }
  private:
    int level;
    unsigned long enemy_time;
    unsigned long ball_time;
    Player player;
    Cannonball ball;
    Invader enemies[NUM_ENEMIES];

    // check if Player defeated all Invaders in current level
    bool level_cleared() {
      for (int i = 0; i < NUM_ENEMIES; i++) {
        if (enemies[i].get_strength() > 0) {
          return false;
        }
      }
      return true;
    }

    // set up a level
    void reset_level() {
      level++;
      print_level(level);
      print_lives(player.get_lives());

      // Initialize enemies based on level
      for (int i = 0; i < NUM_ENEMIES; i++) {
        // Invaders are 4 wide and 4 tall
        int x = (i % ROW_SIZE) * 4;
        int y = 0;
        if (i >= ROW_SIZE) {
          y = 4;
        }
        int strength = 0;

        if (level == 1 && i < ROW_SIZE) {
          // Level 1: 8 invaders with strength 1
          strength = 1;

        } else if (level == 2) {
          // Level 2: 16 invaders with strength alternating 1 and 2
          strength = (i % 2) + 1;

        } else if (level == 3) {
          // Level 3: 16 invaders with strength alternating 1 through 5
          strength = (i % 5) + 1;

        } else if (level == 4) {
          // Level 4: 16 invaders with strength alternating 5,4 on top row and 2,3 on bottom row
          if (i < ROW_SIZE) {
            strength = 5 - (i % 2);
          } else {
            strength = (i % 2) + 2;
          }

        } else if (level > 4) {
          // Level 5+: 16 invaders with random strength
          strength = random(1, 7);

        }
        enemies[i].initialize(x, y, strength);
      }
    }
};

// a global variable that represents the game Space Invaders
Game game;

// see https://www.arduino.cc/reference/en/language/structure/sketch/setup/
void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN_NUMBER, INPUT);
  matrix.begin();
  randomSeed(analogRead(4));
  game.setup_game();
}

// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH);
  game.update(potentiometer_value, button_pressed);
  Serial.println("pot value: " + potentiometer_value);
}

// displays Level
void print_level(int level) {

  matrix.fillScreen(BLACK.to_333());
  matrix.setTextSize(1);
  matrix.setTextColor(WHITE.to_333());
  matrix.setCursor(0,0);
  matrix.print("Level");

  matrix.setCursor(0,8);
  matrix.print(level);

  delay(2000);
  matrix.fillScreen(BLACK.to_333());
}

// displays number of lives
void print_lives(int lives) {
  matrix.fillScreen(BLACK.to_333());
  matrix.setTextSize(1);
  matrix.setTextColor(WHITE.to_333());
  matrix.setCursor(0,0);
  matrix.print("Lives");

  matrix.setCursor(0,8);
  matrix.print(lives);

  delay(2000);
  matrix.fillScreen(BLACK.to_333());
}

// displays "game over"
void game_over() {
  matrix.fillScreen(BLACK.to_333());
  matrix.setTextSize(1);
  matrix.setTextColor(WHITE.to_333());
  matrix.setCursor(0,0);
  matrix.print("game over");
  delay(5000);

  game.setup_game();
}
