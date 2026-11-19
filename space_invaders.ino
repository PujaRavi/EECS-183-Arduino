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
      x = x_arg;
      y = y_arg;
      fired = true;
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

      int bottomLED = y + 1; // ball is 2 LED tall this is lower one
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
    int x;
    int y;
    bool fired;
};

class Player {
  public:
    Player() {
      x = 0;
      y = 0;
      //positions player to bottom of screen
      initialize(16,14);
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
      if(x_arg<0) {
        x = 0;
      }
      else if (x_arg>31) {
        x = 31;
      }
      else {
        x = x_arg;
      }
    }
    
    // Modifies: lives
    void die() {
      if (lives > 0) {
        lives--;
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

      if (x > 0) {
        matrix.drawPixel(x-1,y+1, color.to_333());
      }

      if(x < 31) {
        matrix.drawPixel(x+1,y+1, color.to_333());
      }

    }
};

class Game {
  public:
    Game() {
      level = 0;
      time = 0;
    }
    
    // sets up a new game of Space Invaders
    // Modifies: global variable matrix
    void setupGame() {
      print_lives(lives);
      print_level(level);
      reset_level()
    }
    
    // advances the game simulation one step and renders the graphics
    // see spec for details of game
    // Modifies: global variable matrix
    void update(int potentiometer_value, bool button_pressed) {
      /*
      check for player hit
      update invaders
      update player position
      check for invader hit
      update cannonball
      */

      // check for player hit
      // TODO: based on other class implementations

      // update player position
      // TODO: set x based on how player class is implemented and potentiometer value
      player.set_x()
      
      // update invaders
      bool bottomRowAlive = false;
      for (int i = (MAX_ENEMIES / 2); i < MAX_ENEMIES; i++) {
        if (enemies[i].get_strength() != 0) {
          bottomRowAlive = true;
        }
      }
      for (int i = 0; i < (MAX_ENEMIES / 2); i++) {
        if (bottomRowAlive) {
          enemies[i + 8].move();
        } else {
          enemies[i].move();
        }
      }

      // check for invader hit
      // TODO: based on other class implemtations


      // update cannonball
      ball.move();
    }
  private:
    int level;
    unsigned long time;
    Player player;
    Cannonball ball;
    Invader enemies[NUM_ENEMIES];

    // check if Player defeated all Invaders in current level
    bool level_cleared() {
      for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].get_strength() != 0) {
          return false;
        }
      }
      return true;
    }

    // set up a level
    void reset_level() {
      level++;
      print_lives(lives);
      print_level(level);

      // Initialize player
      // TODO: set y based on player class implementation
      player.set_y();

      // Initialize enemies based on level
      // TODO: need to set x and y args depending on invader class implementation
      for (int i = 0; i < NUM_ENEMIES; i++) {
        int x = 0;
        int y = 0;
        int strength = 0;

        if (level == 1 && i < (NUM_ENEMIES / 2)) {
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
          if (i < (NUM_ENEMIES / 2) {
            strength = 5 - (i % 2);
          } else {
            strength = (i % 2) + 2;
          }

        } else if (level > 4) {
          // Level 5+: 16 invaders with random strength
          strength = random(1, 7);

        }
        enemies[i].initalize(x, y, strength);
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
  game.setupGame();
  randomSeed(analogRead(4));
}

// see https://www.arduino.cc/reference/en/language/structure/sketch/loop/
void loop() {
  int potentiometer_value = analogRead(POTENTIOMETER_PIN_NUMBER);
  bool button_pressed = (digitalRead(BUTTON_PIN_NUMBER) == HIGH);

  game.update(potentiometer_value, button_pressed);
  // Wait half a second
  delay (500);
}

// displays Level
void print_level(int level) {
}

// displays number of lives
void print_lives(int lives) {
}

// displays "game over"
void game_over() {
}

