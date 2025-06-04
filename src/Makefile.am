# Define executable name
bin_PROGRAMS = game

# List of all source files used to build executable
game_SOURCES = \
	$(top_srcdir)/src/main.c \
	$(top_srcdir)/src/core/game.c \
	$(top_srcdir)/src/core/input.c \
	$(top_srcdir)/src/core/cmdline.c \
	$(top_srcdir)/src/core/gui/gui.c \
	$(top_srcdir)/src/core/gui/gui_utils.c \
	$(top_srcdir)/src/core/physics/physics_enemy.c \
	$(top_srcdir)/src/core/physics/physics_geometry.c \
	$(top_srcdir)/src/core/physics/physics_utils.c \
	$(top_srcdir)/src/core/physics/physics.c \
	$(top_srcdir)/src/core/sprite/animation.c \
	$(top_srcdir)/src/core/sprite/sprite.c \
	$(top_srcdir)/src/core/sprite/sprite_utils.c \
	$(top_srcdir)/src/entities/abstract/entity_utils.c \
	$(top_srcdir)/src/entities/abstract/entity.c \
	$(top_srcdir)/src/entities/abstract/geometry.c \
	$(top_srcdir)/src/entities/abstract/static_entity.c \
	$(top_srcdir)/src/entities/characters/enemy.c \
	$(top_srcdir)/src/entities/characters/donkey_kong.c \
	$(top_srcdir)/src/entities/characters/peach.c \
	$(top_srcdir)/src/entities/characters/player.c \
	$(top_srcdir)/src/entities/environment/ladder.c \
	$(top_srcdir)/src/entities/environment/platform.c \
	$(top_srcdir)/src/entities/environment/effect.c \
	$(top_srcdir)/src/entities/environment/item.c \
	$(top_srcdir)/src/level/cutscene.c \
	$(top_srcdir)/src/level/loader.c \
	$(top_srcdir)/src/level/level.c

# Preprocessor and compiler flags
# - Include paths for headers
# - GTK-related flags
AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/lib/cjson \
	$(GTK_CFLAGS)

# Link additional libraries
# - static libcjson.a library
# - GTK libraries
game_LDADD = ../lib/cjson/libcjson.a $(GTK_LIBS)
