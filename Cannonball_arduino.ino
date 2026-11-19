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