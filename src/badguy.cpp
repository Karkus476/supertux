//  $Id$
// 
//  SuperTux
//  Copyright (C) 2000 Bill Kendrick <bill@newbreedsoftware.com>
//  Copyright (C) 2004 Tobias Glaesser <tobi.web@gmx.de>
//  Copyright (C) 2004 Matthias Braun <matze@braunis.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
//  02111-1307, USA.

#include <iostream>
#include <math.h>

#include "globals.h"
#include "defines.h"
#include "badguy.h"
#include "scene.h"
#include "screen.h"
#include "world.h"
#include "tile.h"
#include "resources.h"
#include "sprite_manager.h"

Sprite* img_bsod_squished_left;
Sprite* img_bsod_squished_right;
Sprite* img_bsod_falling_left;
Sprite* img_bsod_falling_right;
Sprite* img_laptop_flat_left;
Sprite* img_laptop_flat_right;
Sprite* img_laptop_falling_left;
Sprite* img_laptop_falling_right;
Sprite* img_bsod_left;
Sprite* img_bsod_right;
Sprite* img_laptop_left;
Sprite* img_laptop_right;
Sprite* img_jumpy_left_up;
Sprite* img_jumpy_left_down;
Sprite* img_jumpy_left_middle;
Sprite* img_mrbomb_left;
Sprite* img_mrbomb_right;
Sprite* img_mrbomb_ticking_left;
Sprite* img_mrbomb_ticking_right;
Sprite* img_mrbomb_explosion;
Sprite* img_stalactite;
Sprite* img_stalactite_broken;
Sprite* img_flame;
Sprite* img_fish;
Sprite* img_fish_down;
Sprite* img_bouncingsnowball_left;
Sprite* img_bouncingsnowball_right;
Sprite* img_bouncingsnowball_squished;
Sprite* img_flyingsnowball;
Sprite* img_flyingsnowball_squished;
Sprite* img_spiky_left;
Sprite* img_spiky_right;
Sprite* img_snowball_left;
Sprite* img_snowball_right;
Sprite* img_snowball_squished_left;
Sprite* img_snowball_squished_right;

#define BADGUY_WALK_SPEED .8f

BadGuyKind  badguykind_from_string(const std::string& str)
{
  if (str == "money")
    return BAD_MONEY;
  else if (str == "laptop" || str == "mriceblock")
    return BAD_LAPTOP;
  else if (str == "bsod")
    return BAD_BSOD;
  else if (str == "mrbomb")
    return BAD_MRBOMB;
  else if (str == "stalactite")
    return BAD_STALACTITE;
  else if (str == "flame")
    return BAD_FLAME;
  else if (str == "fish")
    return BAD_FISH;
  else if (str == "bouncingsnowball")
    return BAD_BOUNCINGSNOWBALL;
  else if (str == "flyingsnowball")
    return BAD_FLYINGSNOWBALL;
  else if (str == "spiky")
    return BAD_SPIKY;
  else if (str == "snowball")
    return BAD_SNOWBALL;
  else
    {
      printf("Couldn't convert badguy: '%s'\n", str.c_str());
      return BAD_BSOD;
    }
}

std::string badguykind_to_string(BadGuyKind kind)
{
  switch(kind)
    {
    case BAD_MONEY:
      return "money";
      break;
    case BAD_LAPTOP:
      return "laptop";
      break;
    case BAD_BSOD:
      return "bsod";
      break;
    case BAD_MRBOMB:
      return "mrbomb";
      break;
    case BAD_STALACTITE:
      return "stalactite";
      break;
    case BAD_FLAME:
      return "flame";
      break;
    case BAD_FISH:
      return "fish";
      break;
    case BAD_BOUNCINGSNOWBALL:
      return "bouncingsnowball";
      break;
    case BAD_FLYINGSNOWBALL:
      return "flyingsnowball";
      break;
    case BAD_SPIKY:
      return "spiky";
      break;
    case BAD_SNOWBALL:
      return "snowball";
      break;
    default:
      return "bsod";
    }
}

BadGuy::BadGuy(float x, float y, BadGuyKind kind_, bool stay_on_platform_)
  : removable(false)
{
  base.x   = x;
  base.y   = y;    
  base.width  = 0;
  base.height = 0;
  base.xm  = 0;
  base.ym  = 0;

  stay_on_platform = stay_on_platform_;
  mode     = NORMAL;
  dying    = DYING_NOT;
  kind     = kind_;
  old_base = base;
  dir      = LEFT;
  seen     = false;
  animation_offset = 0;
  sprite_left = sprite_right = 0;
  physic.reset();
  timer.init(true);

  if(kind == BAD_BSOD) {
    physic.set_velocity(-BADGUY_WALK_SPEED, 0);
    set_sprite(img_bsod_left, img_bsod_right);
  } else if(kind == BAD_MRBOMB) {
    physic.set_velocity(-BADGUY_WALK_SPEED, 0);
    set_sprite(img_mrbomb_left, img_mrbomb_right);
  } else if (kind == BAD_LAPTOP) {
    physic.set_velocity(-BADGUY_WALK_SPEED, 0);
    set_sprite(img_laptop_left, img_laptop_right);
  } else if(kind == BAD_MONEY) {
    set_sprite(img_jumpy_left_up, img_jumpy_left_up);
  } else if(kind == BAD_BOMB) {
    set_sprite(img_mrbomb_ticking_left, img_mrbomb_ticking_right);
    // hack so that the bomb doesn't hurt until it expldes...
    dying = DYING_SQUISHED;
  } else if(kind == BAD_FLAME) {
    base.ym = 0; // we misuse base.ym as angle for the flame
    physic.enable_gravity(false);
    set_sprite(img_flame, img_flame);
  } else if(kind == BAD_BOUNCINGSNOWBALL) {
    physic.set_velocity(-1.3, 0);
    set_sprite(img_bouncingsnowball_left, img_bouncingsnowball_right);
  } else if(kind == BAD_STALACTITE) {
    physic.enable_gravity(false);
    set_sprite(img_stalactite, img_stalactite);
  } else if(kind == BAD_FISH) {
    set_sprite(img_fish, img_fish);
    physic.enable_gravity(true);
  } else if(kind == BAD_FLYINGSNOWBALL) {
    set_sprite(img_flyingsnowball, img_flyingsnowball);
    physic.enable_gravity(false);
  } else if(kind == BAD_SPIKY) {
    physic.set_velocity(-BADGUY_WALK_SPEED, 0);
    set_sprite(img_spiky_left, img_spiky_right);
  } else if(kind == BAD_SNOWBALL) {
    physic.set_velocity(-BADGUY_WALK_SPEED, 0);
    set_sprite(img_snowball_left, img_snowball_right);
  }

  // if we're in a solid tile at start correct that now
  if(kind != BAD_FLAME && kind != BAD_FISH && collision_object_map(base)) {
    printf("Warning: badguy started in wall!.\n");
    while(collision_object_map(base))
      --base.y;
  }
}

void
BadGuy::action_bsod(float frame_ratio)
{
  static const float BSODJUMP = 2;
    
  if (dying == DYING_NOT)
    check_horizontal_bump();

  fall();

  // jump when we're about to fall
  if (physic.get_velocity_y() == 0 && 
      !issolid(base.x+base.width/2, base.y + base.height)) 
    {
      physic.enable_gravity(true);
      physic.set_velocity(physic.get_velocity_x(), BSODJUMP);
    }

  // Handle dying timer:
  if (dying == DYING_SQUISHED && !timer.check())
    {
      /* Remove it if time's up: */
      remove_me();
      return;
    }

  // move
  physic.apply(frame_ratio, base.x, base.y);
  if(dying != DYING_FALLING)
    collision_swept_object_map(&old_base, &base);
}

void
BadGuy::action_laptop(float frame_ratio)
{
  Player& tux = *World::current()->get_tux();

  if(dying == DYING_NOT)
    fall();
  
  /* Move left/right: */
  if (mode == NORMAL || mode == KICK)
    {
      // move
      physic.apply(frame_ratio, base.x, base.y);
      if (dying != DYING_FALLING)
        collision_swept_object_map(&old_base,&base);
    }
  else if (mode == HELD)
    { /* FIXME: The pbad object shouldn't know about pplayer objects. */
      /* If we're holding the laptop */
      dir = tux.dir;
      if(dir==RIGHT)
        {
          base.x = tux.base.x + 16;
          base.y = tux.base.y + tux.base.height/1.5 - base.height;
        }
      else /* facing left */
        {
          base.x = tux.base.x - 16;
          base.y = tux.base.y + tux.base.height/1.5 - base.height;
        }
      if(collision_object_map(base))
        {
          base.x = tux.base.x;
          base.y = tux.base.y + tux.base.height/1.5 - base.height;
        }

      if(tux.input.fire != DOWN) /* SHOOT! */
        {
          if(dir == LEFT)
            base.x -= 24;
          else
            base.x += 24;
          old_base = base;

          mode=KICK;
          set_sprite(img_laptop_flat_left, img_laptop_flat_right);
          physic.set_velocity_x((dir == LEFT) ? -3.5 : 3.5);
          play_sound(sounds[SND_KICK],SOUND_CENTER_SPEAKER);
        }
    }

  if (!dying)
    {
      int changed = dir;
      check_horizontal_bump();
      if(mode == KICK && changed != dir)
        {
          /* handle stereo sound (number 10 should be tweaked...)*/
          if (base.x < scroll_x + screen->w/2 - 10)
            play_sound(sounds[SND_RICOCHET], SOUND_LEFT_SPEAKER);
          else if (base.x > scroll_x + screen->w/2 + 10)
            play_sound(sounds[SND_RICOCHET], SOUND_RIGHT_SPEAKER);
          else
            play_sound(sounds[SND_RICOCHET], SOUND_CENTER_SPEAKER);
        }
    }

  /* Handle mode timer: */
  if (mode == FLAT)
    {
      if(!timer.check())
        {
          mode = NORMAL;
          set_sprite(img_laptop_left, img_laptop_right);
          physic.set_velocity( (dir == LEFT) ? -.8 : .8, 0);
        }
    }
}

void
BadGuy::check_horizontal_bump(bool checkcliff)
{
    float halfheight = base.height / 2;
    if (dir == LEFT && issolid( base.x, (int) base.y + halfheight))
    {
        dir = RIGHT;
        physic.set_velocity(-physic.get_velocity_x(), physic.get_velocity_y());
        return;
    }
    if (dir == RIGHT && issolid( base.x + base.width, (int)base.y + halfheight))
    {
        dir = LEFT;
        physic.set_velocity(-physic.get_velocity_x(), physic.get_velocity_y());
        return;
    }

    // don't check for cliffs when we're falling
    if(!checkcliff)
        return;
    if(!issolid(base.x + base.width/2, base.y + base.height))
        return;
    
    if(dir == LEFT && !issolid(base.x, (int) base.y + base.height + halfheight))
    {
        dir = RIGHT;
        physic.set_velocity(-physic.get_velocity_x(), physic.get_velocity_y());
        return;
    }
    if(dir == RIGHT && !issolid(base.x + base.width,
                (int) base.y + base.height + halfheight))
    {
        dir = LEFT;
        physic.set_velocity(-physic.get_velocity_x(), physic.get_velocity_y());
        return;
    }
}

void
BadGuy::fall()
{
  /* Fall if we get off the ground: */
  if (dying != DYING_FALLING)
    {
      if (!issolid(base.x+base.width/2, base.y + base.height))
        {
          // not solid below us? enable gravity
          physic.enable_gravity(true);
        }
      else
        {
          /* Land: */
          if (physic.get_velocity_y() < 0)
            {
              base.y = int((base.y + base.height)/32) * 32 - base.height;
              physic.set_velocity_y(0);
            }
          // no gravity anymore please
          physic.enable_gravity(false);

          if (stay_on_platform && mode == NORMAL)
            {
              if (!issolid(base.x + ((dir == LEFT) ? 0 : base.width),
                           base.y + base.height))
                {
                  physic.set_velocity_x(-physic.get_velocity_x());
                  if (dir == LEFT)
                    dir = RIGHT;
                  else
                    dir = LEFT;
                }
            }
        }
    }
  else
    {
      physic.enable_gravity(true);
    }
}

void
BadGuy::remove_me()
{
  removable = true;
}

void
BadGuy::action_money(float frame_ratio)
{
  if (fabsf(physic.get_velocity_y()) < 2.5f)
    set_sprite(img_jumpy_left_middle, img_jumpy_left_middle);
  else if (physic.get_velocity_y() < 0)
    set_sprite(img_jumpy_left_up, img_jumpy_left_up);
  else 
    set_sprite(img_jumpy_left_down, img_jumpy_left_down);

  Player& tux = *World::current()->get_tux();

  static const float JUMPV = 6;
    
  fall();
  // jump when on ground
  if(dying == DYING_NOT && issolid(base.x, base.y+32))
    {
      physic.set_velocity_y(JUMPV);
      physic.enable_gravity(true);

      mode = MONEY_JUMP;
    }
  else if(mode == MONEY_JUMP)
    {
      mode = NORMAL;
    }

  // set direction based on tux
  if(tux.base.x > base.x)
    dir = RIGHT;
  else
    dir = LEFT;

  // move
  physic.apply(frame_ratio, base.x, base.y);
  if(dying == DYING_NOT)
    collision_swept_object_map(&old_base, &base);
}

void
BadGuy::action_mrbomb(float frame_ratio)
{
  if (dying == DYING_NOT)
    check_horizontal_bump(true);

  fall();

  physic.apply(frame_ratio, base.x, base.y);
  if (dying != DYING_FALLING)
    collision_swept_object_map(&old_base,&base); 
}

void
BadGuy::action_bomb(float frame_ratio)
{
  static const int TICKINGTIME = 1000;
  static const int EXPLODETIME = 1000;
    
  fall();

  if(mode == NORMAL) {
    mode = BOMB_TICKING;
    timer.start(TICKINGTIME);
  } else if(!timer.check()) {
    if(mode == BOMB_TICKING) {
      mode = BOMB_EXPLODE;
      set_sprite(img_mrbomb_explosion, img_mrbomb_explosion);
      dying = DYING_NOT; // now the bomb hurts
      timer.start(EXPLODETIME);

      /* play explosion sound */  // FIXME: is the stereo all right? maybe we should use player cordinates...
      if (base.x < scroll_x + screen->w/2 - 10)
        play_sound(sounds[SND_EXPLODE], SOUND_LEFT_SPEAKER);
      else if (base.x > scroll_x + screen->w/2 + 10)
        play_sound(sounds[SND_EXPLODE], SOUND_RIGHT_SPEAKER);
      else
        play_sound(sounds[SND_EXPLODE], SOUND_CENTER_SPEAKER);

    } else if(mode == BOMB_EXPLODE) {
      remove_me();
      return;
    }
  }

  // move
  physic.apply(frame_ratio, base.x, base.y);                 
  collision_swept_object_map(&old_base,&base);
}

void
BadGuy::action_stalactite(float frame_ratio)
{
  Player& tux = *World::current()->get_tux();

  static const int SHAKETIME = 800;
  static const int RANGE = 40;
    
  if(mode == NORMAL) {
    // start shaking when tux is below the stalactite and at least 40 pixels
    // near
    if(tux.base.x + 32 > base.x - RANGE && tux.base.x < base.x + 32 + RANGE
            && tux.base.y + tux.base.height > base.y) {
      timer.start(SHAKETIME);
      mode = STALACTITE_SHAKING;
    }
  } if(mode == STALACTITE_SHAKING) {
    base.x = old_base.x + (rand() % 6) - 3; // TODO this could be done nicer...
    if(!timer.check()) {
      mode = STALACTITE_FALL;
    }
  } else if(mode == STALACTITE_FALL) {
    fall();
    /* Destroy if we collides with land */
    if(issolid(base.x+base.width/2, base.y+base.height))
    {
      timer.start(2000);
      dying = DYING_SQUISHED;
      mode = FLAT;
      set_sprite(img_stalactite_broken, img_stalactite_broken);
    }
  } else if(mode == FLAT) {
    fall();
  }

  // move
  physic.apply(frame_ratio, base.x, base.y);

  if(dying == DYING_SQUISHED && !timer.check())
    remove_me();
}

void
BadGuy::action_flame(float frame_ratio)
{
    static const float radius = 100;
    static const float speed = 0.02;
    base.x = old_base.x + cos(base.ym) * radius;
    base.y = old_base.y + sin(base.ym) * radius;

    base.ym = fmodf(base.ym + frame_ratio * speed, 2*M_PI);
}

void
BadGuy::action_fish(float frame_ratio)
{
  static const float JUMPV = 6;
  static const int WAITTIME = 1000;
    
  // go in wait mode when back in water
  if(dying == DYING_NOT && gettile(base.x, base.y+ base.height)->water
        && physic.get_velocity_y() <= 0 && mode == NORMAL)
    {
      mode = FISH_WAIT;
      set_sprite(0, 0);
      physic.set_velocity(0, 0);
      physic.enable_gravity(false);
      timer.start(WAITTIME);
    }
  else if(mode == FISH_WAIT && !timer.check())
    {
      // jump again
      set_sprite(img_fish, img_fish);
      mode = NORMAL;
      physic.set_velocity(0, JUMPV);
      physic.enable_gravity(true);
    }

  physic.apply(frame_ratio, base.x, base.y);
  if(dying == DYING_NOT)
    collision_swept_object_map(&old_base, &base);

  if(physic.get_velocity_y() < 0)
    set_sprite(img_fish_down, img_fish_down);
}

void
BadGuy::action_bouncingsnowball(float frame_ratio)
{
  static const float JUMPV = 4.5;
    
  fall();

  // jump when on ground
  if(dying == DYING_NOT && issolid(base.x, base.y+32))
    {
      physic.set_velocity_y(JUMPV);
      physic.enable_gravity(true);
    }                                                     
  else
    {
      mode = NORMAL;
    }

  // check for right/left collisions
  check_horizontal_bump();

  physic.apply(frame_ratio, base.x, base.y);
  if(dying == DYING_NOT)
    collision_swept_object_map(&old_base, &base);

  // Handle dying timer:
  if (dying == DYING_SQUISHED && !timer.check())
    {
      /* Remove it if time's up: */
      remove_me();
      return;
    }
}

void
BadGuy::action_flyingsnowball(float frame_ratio)
{
  static const float FLYINGSPEED = 1;
  static const int DIRCHANGETIME = 1000;
    
  // go into flyup mode if none specified yet
  if(dying == DYING_NOT && mode == NORMAL) {
    mode = FLY_UP;
    physic.set_velocity_y(FLYINGSPEED);
    timer.start(DIRCHANGETIME/2);
  }

  if(dying == DYING_NOT && !timer.check()) {
    if(mode == FLY_UP) {
      mode = FLY_DOWN;
      physic.set_velocity_y(-FLYINGSPEED);
    } else if(mode == FLY_DOWN) {
      mode = FLY_UP;
      physic.set_velocity_y(FLYINGSPEED);
    }
    timer.start(DIRCHANGETIME);
  }

  if(dying != DYING_NOT)
    physic.enable_gravity(true);

  physic.apply(frame_ratio, base.x, base.y);
  if(dying == DYING_NOT || dying == DYING_SQUISHED)
    collision_swept_object_map(&old_base, &base);

  // Handle dying timer:
  if (dying == DYING_SQUISHED && !timer.check())
    {
      /* Remove it if time's up: */
      remove_me();
      return;
    }                                                          
}

void
BadGuy::action_spiky(float frame_ratio)
{
  if (dying == DYING_NOT)
    check_horizontal_bump();

  fall();
#if 0
  // jump when we're about to fall
  if (physic.get_velocity_y() == 0 && 
          !issolid(base.x+base.width/2, base.y + base.height)) {
    physic.enable_gravity(true);
    physic.set_velocity_y(2);
  }
#endif

  physic.apply(frame_ratio, base.x, base.y);
  if (dying != DYING_FALLING)
    collision_swept_object_map(&old_base,&base);   
}

void
BadGuy::action_snowball(float frame_ratio)
{
  if (dying == DYING_NOT)
    check_horizontal_bump();

  fall();

  physic.apply(frame_ratio, base.x, base.y);
  if (dying != DYING_FALLING)
    collision_swept_object_map(&old_base,&base);
}

void
BadGuy::action(float frame_ratio)
{
  // Remove if it's far off the screen:
  if (base.x < scroll_x - OFFSCREEN_DISTANCE)
    {
      remove_me();                                                
      return;
    }

  // BadGuy fall below the ground
  if (base.y > screen->h) {
    remove_me();
    return;
  }

  // Once it's on screen, it's activated!
  if (base.x <= scroll_x + screen->w + OFFSCREEN_DISTANCE)
    seen = true;

  if(!seen)
    return;

  switch (kind)
    {
    case BAD_BSOD:
      action_bsod(frame_ratio);
      break;

    case BAD_LAPTOP:
      action_laptop(frame_ratio);
      break;
  
    case BAD_MONEY:
      action_money(frame_ratio);
      break;

    case BAD_MRBOMB:
      action_mrbomb(frame_ratio);
      break;
    
    case BAD_BOMB:
      action_bomb(frame_ratio);
      break;

    case BAD_STALACTITE:
      action_stalactite(frame_ratio);
      break;

    case BAD_FLAME:
      action_flame(frame_ratio);
      break;

    case BAD_FISH:
      action_fish(frame_ratio);
      break;

    case BAD_BOUNCINGSNOWBALL:
      action_bouncingsnowball(frame_ratio);
      break;

    case BAD_FLYINGSNOWBALL:
      action_flyingsnowball(frame_ratio);
      break;

    case BAD_SPIKY:
      action_spiky(frame_ratio);
      break;

    case BAD_SNOWBALL:
      action_snowball(frame_ratio);
      break;
    }
}

void
BadGuy::draw()
{
  // Don't try to draw stuff that is outside of the screen
  if(base.x <= scroll_x - base.width || base.x >= scroll_x + screen->w)
    return;
  
  if(sprite_left == 0 || sprite_right == 0)
    {
      return;
    }

  Sprite* sprite = (dir == LEFT) ? sprite_left : sprite_right;
  sprite->draw(base.x - scroll_x, base.y);

  if (debug_mode)
    fillrect(base.x - scroll_x, base.y, base.width, base.height, 75,0,75, 150);
}

void
BadGuy::set_sprite(Sprite* left, Sprite* right) 
{
  if (1)
    {
      base.width = 32;
      base.height = 32;
    }
  else
    {
      // FIXME: Using the image size for the physics and collision is
      // a bad idea, since images should always overlap there physical
      // representation
      if(left != 0) {
        if(base.width == 0 && base.height == 0) {
          base.width  = left->get_width();
          base.height = left->get_height();
        } else if(base.width != left->get_width() || base.height != left->get_height()) {
          base.x -= (left->get_width() - base.width) / 2;
          base.y -= left->get_height() - base.height;
          base.width = left->get_width();
          base.height = left->get_height();
          old_base = base;
        }
      } else {
        base.width = base.height = 0;
      }
    }

  animation_offset = 0;
  sprite_left  = left;
  sprite_right = right;
}

void
BadGuy::bump()
{
  if(kind == BAD_BSOD || kind == BAD_LAPTOP || kind == BAD_MRBOMB
      || kind == BAD_BOUNCINGSNOWBALL) {
    kill_me();
  }
}

void
BadGuy::make_player_jump(Player* player)
{
  player->physic.set_velocity_y(2);
  player->base.y = base.y - player->base.height - 2;
}

void
BadGuy::squish_me(Player* player)
{
  make_player_jump(player);
    
  World::current()->add_score(base.x - scroll_x,
                              base.y, 50 * player_status.score_multiplier);
  play_sound(sounds[SND_SQUISH], SOUND_CENTER_SPEAKER);
  player_status.score_multiplier++;

  dying = DYING_SQUISHED;
  timer.start(2000);
  physic.set_velocity(0, 0);
}

void
BadGuy::squish(Player* player)
{
  if(kind == BAD_MRBOMB) {
    // mrbomb transforms into a bomb now
    World::current()->add_bad_guy(base.x, base.y, BAD_BOMB);
    
    make_player_jump(player);
    World::current()->add_score(base.x - scroll_x, base.y, 50 * player_status.score_multiplier);
    play_sound(sounds[SND_SQUISH], SOUND_CENTER_SPEAKER);
    player_status.score_multiplier++;
    remove_me();
    return;

  } else if(kind == BAD_BSOD) {
    squish_me(player);
    set_sprite(img_bsod_squished_left, img_bsod_squished_right);
    physic.set_velocity_x(0);
    return;
      
  } else if (kind == BAD_LAPTOP) {
    if (mode == NORMAL || mode == KICK)
      {
        /* Flatten! */
        play_sound(sounds[SND_STOMP], SOUND_CENTER_SPEAKER);
        mode = FLAT;
        set_sprite(img_laptop_flat_left, img_laptop_flat_right);
        physic.set_velocity_x(0);

        timer.start(4000);
      } else if (mode == FLAT) {
        /* Kick! */
        play_sound(sounds[SND_KICK], SOUND_CENTER_SPEAKER);

        if (player->base.x < base.x + (base.width/2)) {
          physic.set_velocity_x(5);
          dir = RIGHT;
        } else {
          physic.set_velocity_x(-5);
          dir = LEFT;
        }

        mode = KICK;
        set_sprite(img_laptop_flat_left, img_laptop_flat_right);
      }

    make_player_jump(player);
	      
    World::current()->add_score(base.x - scroll_x, base.y, 25 * player_status.score_multiplier);
    player_status.score_multiplier++;
    return;
  } else if(kind == BAD_FISH) {
    // fish can only be killed when falling down
    if(physic.get_velocity_y() >= 0)
      return;
      
    make_player_jump(player);
	      
    World::current()->add_score(base.x - scroll_x, base.y, 25 * player_status.score_multiplier);
    player_status.score_multiplier++;
     
    // simply remove the fish...
    remove_me();
    return;
  } else if(kind == BAD_BOUNCINGSNOWBALL) {
    squish_me(player);
    set_sprite(img_bouncingsnowball_squished,img_bouncingsnowball_squished);
    return;
  } else if(kind == BAD_FLYINGSNOWBALL) {
    squish_me(player);
    set_sprite(img_flyingsnowball_squished,img_flyingsnowball_squished);
    return;
  } else if(kind == BAD_SNOWBALL) {
    squish_me(player);
    set_sprite(img_snowball_squished_left, img_snowball_squished_right);
    return;
  }
}

void
BadGuy::kill_me()
{
  if(kind == BAD_BOMB || kind == BAD_STALACTITE || kind == BAD_FLAME)
    return;

  dying = DYING_FALLING;
  if(kind == BAD_LAPTOP)
    set_sprite(img_laptop_falling_left, img_laptop_falling_right);
  else if(kind == BAD_BSOD)
    set_sprite(img_bsod_falling_left, img_bsod_falling_right);
  
  physic.enable_gravity(true);
  physic.set_velocity_y(0);

  /* Gain some points: */
  if (kind == BAD_BSOD)
    World::current()->add_score(base.x - scroll_x, base.y,
                    50 * player_status.score_multiplier);
  else 
    World::current()->add_score(base.x - scroll_x, base.y,                                 
                    25 * player_status.score_multiplier);

  /* Play death sound: */
  play_sound(sounds[SND_FALL], SOUND_CENTER_SPEAKER);
}

void
BadGuy::collision(void *p_c_object, int c_object, CollisionType type)
{
  BadGuy* pbad_c    = NULL;

  if(type == COLLISION_BUMP) {
    bump();
    return;
  }

  if(type == COLLISION_SQUISH) {
    Player* player = static_cast<Player*>(p_c_object);
    squish(player);
    return;
  }

  /* COLLISION_NORMAL */
  switch (c_object)
    {
    case CO_BULLET:
      kill_me();
      break;

    case CO_BADGUY:
      pbad_c = (BadGuy*) p_c_object;

      /* If we're a kicked mriceblock, kill any badguys we hit */
      if(kind == BAD_LAPTOP && mode == KICK &&
            pbad_c->kind != BAD_FLAME && pbad_c->kind != BAD_BOMB)
        {
          pbad_c->kill_me();
        }

      /* Kill badguys that run into exploding bomb */
      else if (kind == BAD_BOMB && dying == DYING_NOT)
      {
        if (pbad_c->kind == BAD_MRBOMB)
        {
          // FIXME: this is where other MrBombs *should* explode istead of dying
          pbad_c->kill_me(); 
        }
        else if (pbad_c->kind != BAD_BOMB)
        {
          pbad_c->kill_me();
        }
      }

      /* Kill any badguys that get hit by stalactite */
      else if (kind == BAD_STALACTITE && dying == DYING_NOT)
      {
        pbad_c->kill_me();
      }

      /* When enemies run into eachother, make them change directions */
      else
      {
        // Jumpy is an exception
        if (pbad_c->kind == BAD_MONEY)
          break;

        // Bounce off of other badguy if we land on top of him
        if (base.y + base.height < pbad_c->base.y + pbad_c->base.height)
        {
          Direction old_dir = dir;
          if (pbad_c->dir == LEFT)
            dir = RIGHT;
          else if (pbad_c->dir == RIGHT)
            dir = LEFT;

          if (dir != old_dir)
            physic.inverse_velocity_x();

          physic.set_velocity(fabs(physic.get_velocity_x()), 2);

          break;
        }
        else if (base.y + base.height > pbad_c->base.y + pbad_c->base.height)
          break;

        if (dir == LEFT)
          dir = RIGHT;
        else if (dir == RIGHT)
          dir = LEFT;

        physic.inverse_velocity_x();
      }
      
      break;

    case CO_PLAYER:
      Player* player = static_cast<Player*>(p_c_object);
      /* Get kicked if were flat */
      if (mode == FLAT && !dying)
      {
        play_sound(sounds[SND_KICK], SOUND_CENTER_SPEAKER);

        // Hit from left side
        if (player->base.x < base.x) {
          physic.set_velocity_x(5);
          dir = RIGHT;
        }
        // Hit from right side
        else {
          physic.set_velocity_x(-5);
          dir = LEFT;
        }

        mode = KICK;
        set_sprite(img_laptop_flat_left, img_laptop_flat_right);
      }
      break;

    }
}

//---------------------------------------------------------------------------

void load_badguy_gfx()
{
  img_bsod_squished_left = sprite_manager->load("bsod-squished-left");
  img_bsod_squished_right = sprite_manager->load("bsod-squished-right");
  img_bsod_falling_left = sprite_manager->load("bsod-falling-left");
  img_bsod_falling_right = sprite_manager->load("bsod-falling-right");
  img_laptop_flat_left = sprite_manager->load("laptop-flat-left");
  img_laptop_flat_right = sprite_manager->load("laptop-flat-right");
  img_laptop_falling_left = sprite_manager->load("laptop-falling-left");
  img_laptop_falling_right = sprite_manager->load("laptop-falling-right");
  img_bsod_left = sprite_manager->load("bsod-left");
  img_bsod_right = sprite_manager->load("bsod-right");
  img_laptop_left = sprite_manager->load("laptop-left");
  img_laptop_right = sprite_manager->load("laptop-right");
  img_jumpy_left_up = sprite_manager->load("jumpy-left-up");
  img_jumpy_left_down = sprite_manager->load("jumpy-left-down");
  img_jumpy_left_middle = sprite_manager->load("jumpy-left-middle");
  img_mrbomb_left = sprite_manager->load("mrbomb-left");
  img_mrbomb_right = sprite_manager->load("mrbomb-right");
  img_mrbomb_ticking_left = sprite_manager->load("mrbomb-ticking-left");
  img_mrbomb_ticking_right = sprite_manager->load("mrbomb-ticking-right");
  img_mrbomb_explosion = sprite_manager->load("mrbomb-explosion");
  img_stalactite = sprite_manager->load("stalactite");
  img_stalactite_broken = sprite_manager->load("stalactite-broken");
  img_flame = sprite_manager->load("flame");
  img_fish = sprite_manager->load("fish");
  img_fish_down = sprite_manager->load("fish-down");
  img_bouncingsnowball_left = sprite_manager->load("bouncingsnowball-left");
  img_bouncingsnowball_right = sprite_manager->load("bouncingsnowball-right");
  img_bouncingsnowball_squished = sprite_manager->load("bouncingsnowball-squished");
  img_flyingsnowball = sprite_manager->load("flyingsnowball");
  img_flyingsnowball_squished = sprite_manager->load("flyingsnowball-squished");
  img_spiky_left = sprite_manager->load("spiky-left");
  img_spiky_right = sprite_manager->load("spiky-right");
  img_snowball_left = sprite_manager->load("snowball-left");
  img_snowball_right = sprite_manager->load("snowball-right");
  img_snowball_squished_left = sprite_manager->load("snowball-squished-left");
  img_snowball_squished_right = sprite_manager->load("snowball-squished-right");
}

void free_badguy_gfx()
{
}

// EOF //
