import math
from arena.vector import Vector2D
import arena.command as acmd
from typing import List


class Droid:
    def __init__(self, id: str, init_pos: Vector2D, init_rot: float,
                 color: int,
                 cmds: List[acmd.Command]=[]) -> None:
        self.id = id # 識別ID
        self.pos = init_pos # 位置
        self.rot = init_rot # 回転
        self.color = color
        self._cmds = cmds # 命令リスト
        self._cmd_index = 0 # 次に実行すべき命令のインデックス
        self._cmd_exec_time = 0.0 # 現在の命令の実行時間
        self._cmd_timeout = 3.0 # 最大命令実行時間
    
    def set_commands(self, cmds: List[acmd.Command]) -> None:
        self._cmds = cmds
        self._cmd_index = 0
        self._cmd_exec_time = 0.0
    
    @property
    def front_vec(self) -> Vector2D:
        return Vector2D(math.cos(self.rot), math.sin(self.rot))

    def next_command(self, env) -> None:
        cmd = self._cmds[self._cmd_index]
        if cmd.type == acmd.MOVE_F:
            self.pos += self.front_vec * env.dt
        elif cmd.type == acmd.MOVE_B:
            self.pos -= self.front_vec * env.dt
        elif cmd.type == acmd.TURN_L:
            self.rot += math.pi / 2
        elif cmd.type == acmd.TURN_R:
            self.rot -= math.pi / 2
