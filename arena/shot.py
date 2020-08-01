from .vector import Vector2D
from typing import TYPE_CHECKING


if TYPE_CHECKING:
    from .droid import Droid


class Shot:
    def __init__(self, shot_by: int, attack: int, pos: Vector2D) -> None:
        self.attack = attack
        self.shot_by = shot_by
        self.pos = pos
        self.destroyed = False
        self.life = 0.0
        self.max_life = 6.0
    
    def process(self, env) -> None:
        self.life += env.dt
        if self.life > self.max_life:
            self.destroyed = True

    def droid_hit(self, droid: 'Droid') -> bool:
        """
        Droid に hit していたら True を返す.
        To be overridden by subclasses.
        """
        return False
    

class Shell(Shot):
    def __init__(self, shot_by: int, attack: int, pos: Vector2D, velocity: Vector2D, radius: float) -> None:
        super(Shell, self).__init__(shot_by, attack, pos)
        self.velocity = velocity
        self.radius = radius
    
    def process(self, env) -> None:
        super(Shell, self).process(env)
        self.pos += self.velocity * env.dt
    
    def droid_hit(self, droid: 'Droid') -> bool:
        return (self.pos - droid.pos).length() <= self.radius
