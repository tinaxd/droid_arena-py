import math
from typing import Tuple


class Vector2D:
    def __init__(self, x: float, y: float) -> None:
        self.x = x
        self.y = y
    
    def length(self) -> float:
        return math.sqrt(self.dot(self))
    
    def dot(self, o) -> float:
        return self.x * o.x + self.y * o.y

    def __add__(self, o: 'Vector2D') -> 'Vector2D':
        return self.__class__(self.x + o.x, self.y + o.y)
    
    def __sub__(self, o: 'Vector2D') -> 'Vector2D':
        return self.__class__(self.x - o.x, self.y - o.y)
    
    def __mul__(self, k: float) -> 'Vector2D':
        return self.__class__(self.x * k, self.y * k)
    
    def __truediv__(self, k: float) -> 'Vector2D':
        return self.__class__(self.x / k, self.y / k)
    
    def __floordiv__(self, k: float) -> 'Vector2D':
        return self.__class__(self.x // k, self.y // k)
    
    def unpack(self) -> Tuple[float, float]:
        return (self.x, self.y)
    