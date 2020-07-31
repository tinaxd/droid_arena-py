import sdl2.ext # type: ignore
import sdl2 # type: ignore
import sdl2.sdlgfx # type: ignore
from typing import TYPE_CHECKING, List
from arena.droid import Droid
from arena.vector import Vector2D
import arena.command as acmd


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
        for droid in self.droids:
            env = Environment(mdelta / 1000.0)
            droid.next_command(env)

    def _render(self) -> None:
        self.renderer.clear(sdl2.ext.Color(255, 255, 255, 0))
        droid_radius = 30
        for droid in self.droids:
            x, y = droid.pos.unpack()
            sdl2.sdlgfx.filledCircleColor(self.renderer.sdlrenderer,
                                          int(x), int(y), droid_radius, droid.color)


class Environment:
    def __init__(self, delta):
        self.dt = delta


if __name__ == '__main__':
    g = Game()
    d1 = Droid('player', Vector2D(320.0, 480.0), 0.0, 0xff0000ff, [acmd.MoveCommand(acmd.MOVE_F), acmd.MoveCommand(acmd.TURN_L)])
    d2 = Droid('enemy', Vector2D(320.0, 160.0), 0.0, 0xffff00ff, [acmd.MoveCommand(acmd.MOVE_B), acmd.MoveCommand(acmd.TURN_L)])
    g.add_droid(d1)
    g.add_droid(d2)
    g.run()
