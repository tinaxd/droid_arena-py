import math
from arena.vector import Vector2D


class Droid:
    def __init__(self, id, init_pos, init_rot, cmds=[]):
        self.id = id # 識別ID
        self.pos = init_pos # 位置
        self.rot = init_rot # 回転
        self._cmds = cmds # 命令リスト
        self._cmd_index = 0 # 次に実行すべき命令のインデックス
        self._cmd_exec_time = 0.0 # 現在の命令の実行時間
        self._cmd_timeout = 3.0 # 最大命令実行時間
    
    def set_commands(self, cmds):
        self._cmds = cmds
        self._cmd_index = 0
        self._cmd_exec_time = 0.0
    
    @property
    def front_vec(self):
        return Vector2D(math.cos(self.rot), math.sin(self.rot))

    def next_command(self, env):
        cmd = self._cmds[self._cmd_index]
        if cmd.type == 0:
            self.pos += self.front_vec * env.dt
        elif cmd.type == 1:
            self.pos -= self.front_vec * env.dt
        elif cmd.type == 2:
            self.rot += math.pi / 2
        elif cmd.type == 3:
            self.rot -= math.pi / 2
