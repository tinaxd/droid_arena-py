class Command:
    def __init__(self):
        self.type = None


MOVE_F = 0
MOVE_B = 1
TURN_L = 2
TURN_R = 3
TURN_ENEMY = 4
SHOT_SHELL = 5


class MoveCommand(Command):
    def __init__(self, ty):
        super(MoveCommand, self).__init__()
        self.type = ty


class ShotCommand(Command):
    def __init__(self, ty):
        super(ShotCommand, self).__init__()
        self.type = ty
