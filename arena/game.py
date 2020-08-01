import sdl2.ext # type: ignore
import sdl2 # type: ignore
import sdl2.sdlgfx # type: ignore
from typing import TYPE_CHECKING, List
from arena.droid import Droid
from arena.vector import Vector2D
import arena.command as acmd
import math


class Game:
    def __init__(self) -> None:
        sdl2.ext.init()
        self.window = sdl2.ext.Window("Droid Arena", size=(640, 640))
        self.window.show()
        self.renderer = sdl2.ext.Renderer(self.window)
        self.factory = sdl2.ext.SpriteFactory(sdl2.ext.TEXTURE, renderer=self.renderer)
        self.droids: List['Droid'] = []
        self.lasttime = sdl2.SDL_GetTicks()

    def add_droid(self, droid: 'Droid') -> None:
        self.droids.append(droid)

    def run(self) -> None:
        quit = False
        while not quit:
            events = sdl2.ext.get_events()
            for event in events:
                if event.type == sdl2.SDL_QUIT:
                    quit = True
                    break
            curr = sdl2.SDL_GetTicks()
            delta = curr - self.lasttime
            self.lasttime = curr
            self._process(delta)
            self._render()
            self.renderer.present()
        sdl2.ext.quit()
    
    def _process(self, mdelta) -> None:
        env = Environment(mdelta / 1000.0, self.droids)
        for droid in self.droids:
            droid.next_command(env)

    def _render(self) -> None:
        def _color_inv(color):
            r, g, b, a = color
            return 255 - r, 255 - g, 255 - b, a
        self.renderer.clear(sdl2.ext.Color(255, 255, 255, 0))
        droid_radius = 30
        droid_dot_rad = 5
        droid_dot_from_center = 15
        for droid in self.droids:
            x, y = droid.pos.unpack()
            sdl2.sdlgfx.filledCircleRGBA(self.renderer.sdlrenderer,
                                         int(x), int(y), droid_radius, *droid.color)
            sdl2.sdlgfx.filledCircleRGBA(self.renderer.sdlrenderer,
                                         int(x + droid_dot_from_center * math.cos(droid.rot)),
                                         int(y + droid_dot_from_center * math.sin(droid.rot)),
                                         droid_dot_rad, *_color_inv(droid.color))


class Environment:
    def __init__(self, delta, droids):
        """
        @param delta 前回の process からの経過時間. 単位は秒.
        @param droids 全 Droid のリスト.
        """
        self.dt = delta
        self._droids = droids
    
    def find_nearest_enemy(self, droid):
        """
        droid から最も近い Droid のインスタンスを返す.
        droid の他に Droid が存在しない場合は None を返す.
        """
        def _distance(other):
            return (other.pos - droid.pos).length()
        try:
            return min([d for d in self._droids if d.team != droid.team], key=_distance)
        except ValueError:
            return None


if __name__ == '__main__':
    g = Game()
    d1 = Droid('player', 0, Vector2D(320.0, 480.0), 0.0, (255, 0, 0, 255), [acmd.MoveCommand(acmd.MOVE_F), acmd.MoveCommand(acmd.TURN_ENEMY)])
    d2 = Droid('enemy', 1, Vector2D(320.0, 160.0), 0.0, (0, 255, 0, 255), [acmd.MoveCommand(acmd.MOVE_B), acmd.MoveCommand(acmd.TURN_L)])
    g.add_droid(d1)
    g.add_droid(d2)
    g.run()
