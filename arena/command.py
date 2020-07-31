class Command:
    def __init__(self):
        self.type = None


MOVE_F = 0
MOVE_B = 1
TURN_L = 2
TURN_R = 3


class MoveCommand(Command):
    def __init__(self, ty):
        super(MoveCommand, self).__init__()
        self.type = ty
