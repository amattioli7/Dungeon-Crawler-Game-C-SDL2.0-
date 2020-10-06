#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#ifndef WEAPON_H
#define WEAPON_H

class Weapon{

public:
  Weapon();
  Weapon(double xcoord, double ycoord, int width, int height, int type);
  double x_pos;
  double y_pos;
  int x_size;
  int y_size;
  int weapon_type; //potentially 1 for sword, 2 for axe, etc.
  bool picked_up;
  SDL_Rect dstrect;
  std::vector<SDL_Texture*> sprites;

};

#endif
