#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "Weapon.h"

Weapon::Weapon(double xcoord, double ycoord, int width, int height, int type) :x_pos{xcoord}, y_pos{ycoord}, x_size{width}, y_size{height}, weapon_type{type} {
  dstrect = {static_cast<int>(x_pos), static_cast<int>(y_pos), x_size, y_size};
  picked_up = false;
}
