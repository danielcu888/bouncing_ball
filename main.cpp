#include <iostream>
#include <utility>
#include <chrono>
#include <thread>
#include <cmath>

/***************************************************************/
/**************** PHYSICAL PARAMETERS **************************/
static const constexpr double ROLLING_ATTENUATION_FACTOR = 0.95;
static const constexpr double BOUNCE_ATTENUATION_FACTOR = 0.5;
static const constexpr int MAX_NUM_BOUNCES = 200;
static const constexpr double G = 9.81;
static const std::pair<double, double> P0 {0.0, 0.0};
static const std::pair<double, double> V0 {10.0, 10.0};
/***************************************************************/

/************** ROOM DIMENSIONS ********************************/
static const constexpr std::size_t X_WALL = 49;
/***************************************************************/

void update_pva( std::pair<double, double>& p
               , std::pair<double, double>& v
               , const std::pair<double, double>& a
               , int& num_bounces
               , std::size_t& num_contact_steps
               );
void draw_grid(const std::pair<double, double>& position);

int main(int argc, char *argv[])
{
  // initialize
  int num_bounces = MAX_NUM_BOUNCES;
  std::size_t num_contact_steps = 0;
  std::pair<double, double> p = P0;
  std::pair<double, double> v = V0;
  const std::pair<double, double> a {0.0, -G};

  // loop
  while(num_bounces >= 0)
  {
    update_pva(p, v, a, num_bounces, num_contact_steps);
    draw_grid(p);
  }

  return EXIT_SUCCESS;
}

void update_pva( std::pair<double, double>& p
               , std::pair<double, double>& v
               , const std::pair<double, double>& a
               , int& num_bounces
               , std::size_t& num_contact_steps
               )
{
  static const constexpr double DT = 0.1;
  static const constexpr double ROLLING_NUM_STEPS_THRESHOLD = 2;

  const std::pair<double, double> v0 = v;
  v.first += a.first*DT;
  v.second += a.second*DT;

  const std::pair<double, double> p0 = p;
  p.first = p.first + v0.first*DT + 0.5*a.first*DT*DT;
  p.second = p.second + v0.second*DT + 0.5*a.second*DT*DT;

  // Right-hand wall
  if (p.first < 0.0 && v.first < 0.0)
  {
    v.first *= -BOUNCE_ATTENUATION_FACTOR;
    p.first = 0.0;
  }

  // Left-hand wall
  if (p.first > X_WALL && v.first > 0.0)
  {
    v.first *= -BOUNCE_ATTENUATION_FACTOR;
    p.first = X_WALL;
  }

  // Floor
  if (p.second < 0.0 && v.second < 0.0)
  {
    v.second *= -BOUNCE_ATTENUATION_FACTOR;
    p.second = 0.0;
    --num_bounces;
  }

  if (p.second < 1.0)
  {
    ++num_contact_steps;
  }
  else
  {
    num_contact_steps = 0;
  }

  // Rolling
  if (num_contact_steps > ROLLING_NUM_STEPS_THRESHOLD)
  {
    v.first *= ROLLING_ATTENUATION_FACTOR;
    v.second *= ROLLING_ATTENUATION_FACTOR;
    num_contact_steps = 0;
  }

}

void draw_grid(const std::pair<double, double>& position)
{
  std::cout << "\x1b[2J\x1b[1;1H";

  static const constexpr char EMPTY = ' ';
  static const constexpr char POSITION = '*';
  static const constexpr char WALL = '|';
  static const constexpr char FLOOR = '^';
  static const constexpr std::size_t MAX_Y = 10;
  static const constexpr std::size_t MAX_X = 50;

  const std::size_t pos_x = static_cast<std::size_t>(position.first);
  const std::size_t pos_y = static_cast<std::size_t>(position.second);

  for(int row = MAX_Y-1; row >= 0; --row)
  {
    std::cout << WALL;
    for(std::size_t col = 0; col <= X_WALL; ++col)
    {
      if (row == pos_y && col == pos_x)
      {
        std::cout << POSITION;
      }
      else if (col == X_WALL)
      {
        std::cout << WALL;
      }
      else
      {
        std::cout << EMPTY;
      }
    }
    std::cout << std::endl;
  }

  for(std::size_t col = 0; col <= X_WALL+1; ++col)
  {
    std::cout << FLOOR;
  }

  std::cout << "\n\n\n";

  std::this_thread::sleep_for(std::chrono::milliseconds(30));
}
