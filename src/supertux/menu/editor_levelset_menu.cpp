//  SuperTux
//  Copyright (C) 2016 Hume2 <teratux.mail@gmail.com>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "supertux/menu/editor_levelset_menu.hpp"

#include <physfs.h>

#include "gui/dialog.hpp"
#include "gui/menu.hpp"
#include "gui/menu_item.hpp"
#include "gui/menu_manager.hpp"
#include "editor/editor.hpp"
#include "supertux/menu/menu_storage.hpp"
#include "supertux/level.hpp"
#include "supertux/level_parser.hpp"
#include "supertux/world.hpp"
#include "util/file_system.hpp"
#include "util/gettext.hpp"

EditorLevelsetMenu::EditorLevelsetMenu():
  world(Editor::current()->get_world())
{
  initialize();
}

EditorLevelsetMenu::EditorLevelsetMenu(World* world_):
  world(world_)
{
  initialize();
}

EditorLevelsetMenu::~EditorLevelsetMenu()
{
  try
  {
    world->save();
  }
  catch(std::exception& e)
  {
    log_warning << "Could not save world: " << e.what() << std::endl;
  }
}

void
EditorLevelsetMenu::initialize() {
  add_label(_("Level subset properties"));
  add_hl();
  add_textfield(_("Name"), &(world->m_title));
  add_textfield(_("Description"), &(world->m_description));
  add_hl();
  add_back(_("OK"));
}

/* EOF */
