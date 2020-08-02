import sdl2.ext # type: ignore
import sdl2 # type: ignore
import sdl2.sdlgfx # type: ignore
from typing import TYPE_CHECKING, List, Optional, cast
from arena.droid import Droid, ScriptedDroid
from .shot import Shot, Shell
from arena.vector import Vector2D
import arena.command as acmd
import math


class GameFinish(Exception):
    def __init__(self, msg: str, win_team: int) -> None:
        super(GameFinish, self).__init__(msg)
        self.win_team = win_team


class Game:
    def __init__(self) -> None:
        sdl2.ext.init()
        self.size = (640, 640)
        self.window = sdl2.ext.Window("Droid Arena", size=self.size)
        self.window.show()
        self.renderer = sdl2.ext.Renderer(self.window)
        self.factory = sdl2.ext.SpriteFactory(sdl2.ext.TEXTURE, renderer=self.renderer)
        self.droids: List['Droid'] = []
        self.shots: List['Shot'] = []
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
            try:
                self._process(delta)
                self._process_collision(delta)
                self._render()
                self.renderer.present()
            except GameFinish as e:
                print(f'team {e.win_team} won')
                quit = True
        sdl2.ext.quit()
    
    def _process(self, mdelta) -> None:
        env = Environment(mdelta / 1000.0, self.droids)
        teams = set()
        for droid in self.droids:
            teams.add(droid.team)
            droid.next_command(env)
        if (len(teams) == 1):
            self._game_finish(teams.pop())
        self._apply_env_changes(env)
        self._droid_boundary_check()

        for shot in self.shots:
            shot.process(env)
        self._apply_env_changes(env)
    
    def _game_finish(self, team: int) -> None:
        raise GameFinish("game ends", team)
    
    def _droid_boundary_check(self) -> None:
        for droid in self.droids:
            x, y = droid.pos.unpack()
            if x < 0:
                droid.pos.x = 0.0
            elif x > self.size[0]:
                droid.pos.x = self.size[0] - 1.0
            if y < 0:
                droid.pos.y = 0.0
            elif y > self.size[1]:
                droid.pos.y = self.size[1] - 1.0
    
    def _process_collision(self, mdelta) -> None:
        for shot in self.shots:
            if shot.destroyed:
                continue
            for droid in self.droids:
                if droid.team == shot.shot_by:
                    continue
                if shot.droid_hit(droid):
                    droid.hit(shot)
                    shot.destroyed = True
                    break
        self.shots = [s for s in self.shots if not s.destroyed]
        self.droids = [d for d in self.droids if not d.destroyed]

    def _apply_env_changes(self, env: 'Environment') -> None:
        for shot in env._queue_shots:
            self.shots.append(shot)
        env._queue_shots.clear()
    
    def _render_droids(self) -> None:
        def _color_inv(color):
            r, g, b, a = color
            return 255 - r, 255 - g, 255 - b, a
        droid_radius = 30
        droid_dot_rad = 5
        droid_dot_from_center = 15
        hp_height = 5
        for droid in self.droids:
            x, y = droid.pos.unpack()
            # 本体
            sdl2.sdlgfx.filledCircleRGBA(self.renderer.sdlrenderer,
                                         int(x), int(y), droid_radius, *droid.color)
            # ドット
            sdl2.sdlgfx.filledCircleRGBA(self.renderer.sdlrenderer,
                                         int(x + droid_dot_from_center * math.cos(droid.rot)),
                                         int(y + droid_dot_from_center * math.sin(droid.rot)),
                                         droid_dot_rad, *_color_inv(droid.color))
            # 体力バー
            hp_start_x = int(x - droid_radius)
            hp_start_y = int(y - droid_radius - hp_height)
            hp_end_x   = int(hp_start_x + 2 * droid_radius * (float(droid.hp) / droid.max_hp))
            hp_end_y   = int(y - droid_radius)
            sdl2.sdlgfx.rectangleRGBA(self.renderer.sdlrenderer,
                                      hp_start_x, hp_start_y, int(x + droid_radius), hp_end_y,
                                      0, 0, 0, 255)
            sdl2.sdlgfx.boxRGBA(self.renderer.sdlrenderer,
                                hp_start_x, hp_start_y, hp_end_x, hp_end_y,
                                0, 0, 0, 255)
    
    def _render_shots(self) -> None:
        for shot in self.shots:
            shot = cast(Shell, shot)
            radius = int(shot.radius)
            x, y = shot.pos.unpack()
            sdl2.sdlgfx.filledCircleRGBA(self.renderer.sdlrenderer,
                                         int(x), int(y), radius, 0, 0, 0, 255)

    def _render(self) -> None:
        self.renderer.clear(sdl2.ext.Color(255, 255, 255, 0))
        self._render_droids()
        self._render_shots()


class Environment:
    def __init__(self, delta: float, droids: List[Droid]) -> None:
        """
        @param delta 前回の process からの経過時間. 単位は秒.
        @param droids 全 Droid のリスト.
        """
        self.dt = delta
        self._droids = droids
        self._queue_shots: List['Shot'] = []
    
    def find_nearest_enemy(self, droid: Droid) -> Optional[Droid]:
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
        
    def new_shot(self, shot: 'Shot') -> None:
        self._queue_shots.append(shot)

if __name__ == '__main__':
    src = """
    time = 0.0
    lastshot = 0.0

    function update(info)
        info.x = info.x + info.env.dt * 50
        time = time + info.env.dt
        if time - lastshot > 2.0 then
            info.env.new_shot({type='shell'})
            lastshot = time
        end
        return info
    end
    """

    g = Game()
    d1 = Droid('player', 0, Vector2D(320.0, 480.0), 0.0, 50, (255, 0, 0, 255),
               [acmd.MoveCommand(acmd.MOVE_F),
                acmd.MoveCommand(acmd.TURN_ENEMY),
                acmd.ShotCommand(acmd.SHOT_SHELL)])
    d1.adm = (5, 5, 5)
    d2 = Droid('enemy', 1, Vector2D(320.0, 160.0), 0.0, 50, (0, 255, 0, 255),
               [acmd.MoveCommand(acmd.TURN_ENEMY),
                acmd.MoveCommand(acmd.SHOT_SHELL),
                acmd.MoveCommand(acmd.TURN_L),
                acmd.MoveCommand(acmd.MOVE_F)])
    d2.adm = (1, 5, 9)
    #d3 = ScriptedDroid('spectator', 2, Vector2D(160.0, 320.0), 0.0, 50, (0, 0, 255, 255), src)
    g.add_droid(d1)
    g.add_droid(d2)
    #g.add_droid(d3)
    g.run()
