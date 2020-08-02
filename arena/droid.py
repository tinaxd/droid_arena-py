import math
from arena.vector import Vector2D
import arena.command as acmd
from typing import List, Tuple, TYPE_CHECKING
from .shot import Shell
import lupa # type: ignore


if TYPE_CHECKING:
    from .shot import Shot
    from .game import Environment


class Droid:
    def __init__(self, id: str, team: int, init_pos: Vector2D, init_rot: float, hp: int,
                 color: Tuple[int, int, int, int],
                 cmds: List[acmd.Command]=[]) -> None:
        self.id = id # 識別ID
        self.team = team # team ID
        self.pos = init_pos # 位置
        self.rot = init_rot # 回転
        self.color = color
        self.hp = hp
        self.max_hp = hp
        self.destroyed = False
        self._cmds = cmds # 命令リスト
        self._cmd_index = 0 # 次に実行すべき命令のインデックス
        self._cmd_exec_time = 0.0 # 現在の命令の実行時間
        self._cmd_timeout = 1.0 # 最大命令実行時間
        self._cmd_executed = False
        self.adm = (5, 5, 5) # Attack, Defence, Movement
    
    def set_commands(self, cmds: List[acmd.Command]) -> None:
        self._cmds = cmds
        self._cmd_index = 0
        self._cmd_exec_time = 0.0
    
    @property
    def front_vec(self) -> Vector2D:
        return Vector2D(math.cos(self.rot), math.sin(self.rot))

    def next_command(self, env: 'Environment') -> None:
        self._update_command(env)
        cmd = self._cmds[self._cmd_index]
        if cmd.type == acmd.MOVE_F:
            self.pos += self.front_vec * env.dt * (self.adm[2] * 10.0)
        elif cmd.type == acmd.MOVE_B:
            self.pos -= self.front_vec * env.dt * (self.adm[2] * 10.0)
        elif cmd.type == acmd.TURN_L:
            self.rot -= math.pi / 2.0 / self._cmd_timeout * env.dt
        elif cmd.type == acmd.TURN_R:
            self.rot += math.pi / 2.0 / self._cmd_timeout * env.dt
        elif cmd.type == acmd.TURN_ENEMY:
            target = env.find_nearest_enemy(self)
            if target:
                r = target.pos - self.pos
                angle = math.acos(self.front_vec.dot(r) / r.length())
                if math.fabs(angle) < 0.01:
                    self._next_command_in(0.0)
                else:
                    self.rot -= angle / math.fabs(angle) * self.adm[2] * env.dt
        elif cmd.type == acmd.SHOT_SHELL:
            if not self._cmd_executed:
                env.new_shot(Shell(self.team, self.adm[0], self.pos, self.front_vec*200, 15.0))
                self._cmd_executed = True
                self._next_command_in(0.5)

    def _update_command(self, env) -> None:
        self._cmd_exec_time += env.dt
        if self._cmd_exec_time > self._cmd_timeout:
            self._cmd_executed = False
            self._cmd_exec_time = 0.0
            self._cmd_index += 1
            if self._cmd_index >= len(self._cmds):
                self._cmd_index = 0
    
    def _next_command_in(self, secs: float) -> None:
        self._cmd_exec_time = self._cmd_timeout - secs
    
    def hit(self, shot: 'Shot') -> None:
        self.hp -= shot.attack
        if self.hp <= 0:
            self.destroyed = True


class ScriptedDroid(Droid):
    def __init__(self, id: str, team: int, init_pos: Vector2D, init_rot: float, hp: int,
                 color: Tuple[int, int, int, int], luasrc: str) -> None:
        super(ScriptedDroid, self).__init__(id, team, init_pos, init_rot, hp, color, [])
        self.lua = lupa.LuaRuntime()
        self.lua.execute(luasrc)
    
    def set_commands(self, cmds) -> None:
        pass
    
    @staticmethod
    def _make_info(droid, env):
        x, y = droid.pos.unpack()
        def _find_nearest_enemy():
            return ScriptedDroid._make_info(env.find_nearest_enemy(droid), env)
        def _new_shot(shotinfo):
            if shotinfo["type"] == "shell":
                env.new_shot(Shell(droid.team, 5, droid.pos, droid.front_vec*100, 30.0))
        return {
            "id": droid.id,
            "team": droid.team,
            "x": x,
            "y": y,
            "rot": droid.rot,
            "hp": droid.hp,
            "env": {
                "dt": env.dt,
                "find_nearest_droid": _find_nearest_enemy,
                "new_shot": _new_shot
            }
        }
    
    def _apply_info(self, info):
        self.pos = Vector2D(info["x"], info["y"])
        self.rot = info["rot"]
        self.hp  = info["hp"]

    def next_command(self, env) -> None:
        update = self.lua.globals().update
        info = update(ScriptedDroid._make_info(self, env))
        self._apply_info(info)
