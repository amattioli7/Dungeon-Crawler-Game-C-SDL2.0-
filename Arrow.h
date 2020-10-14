#include "Projectile.h"
#include <vector>

#ifndef ARROW_H
#define ARROW_H

class Arrow:public Projectile{
    public:
        using Projectile::Projectile;
        const double SPEED = 350;
        const double DAMAGE = 20;
        static std::vector<SDL_Texture*> sprites;
        int x_size=25;
        int y_size=25;

};
#endif