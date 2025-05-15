bin_PROGRAMS = game

game_SOURCES = \
	$(top_srcdir)/src/main.c \
	$(top_srcdir)/src/core/game.c \
	$(top_srcdir)/src/core/input.c \
	$(top_srcdir)/src/core/gui.c \
	$(top_srcdir)/src/core/physics/physics_enemy.c \
	$(top_srcdir)/src/core/physics/physics_geometry.c \
	$(top_srcdir)/src/core/physics/physics_utils.c \
	$(top_srcdir)/src/core/physics/physics.c \
	$(top_srcdir)/src/core/sprite/animation.c \
	$(top_srcdir)/src/core/sprite/sprite.c \
	$(top_srcdir)/src/core/sprite/sprite_utils.c \
	$(top_srcdir)/src/entities/abstract/effect.c \
	$(top_srcdir)/src/entities/abstract/entity_utils.c \
	$(top_srcdir)/src/entities/abstract/enemy.c \
	$(top_srcdir)/src/entities/abstract/entity.c \
	$(top_srcdir)/src/entities/abstract/geometry.c \
	$(top_srcdir)/src/entities/abstract/item.c \
	$(top_srcdir)/src/entities/abstract/static_entity.c \
	$(top_srcdir)/src/entities/characters/donkey_kong.c \
	$(top_srcdir)/src/entities/characters/peach.c \
	$(top_srcdir)/src/entities/characters/player.c \
	$(top_srcdir)/src/entities/environment/ladder.c \
	$(top_srcdir)/src/entities/environment/platform.c \
	$(top_srcdir)/src/level/cutscene.c \
	$(top_srcdir)/src/level/loader.c \
	$(top_srcdir)/src/level/level.c

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/lib/cjson \
	$(GTK_CFLAGS)

game_LDADD = ../lib/cjson/libcjson.a $(GTK_LIBS)

bindir = $(top_srcdir)
