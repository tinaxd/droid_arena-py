import math


class Vector2D:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    
    def length(self):
        return math.sqrt(self.dot(self))
    
    def dot(self, o):
        return self.x * o.x + self.y * o.y

    def __add__(self, o):
        return self.__class__(self.x + o.x, self.y + o.y)
    
    def __sub__(self, o):
        return self.__class__(self.x - o.x, self.y - o.y)
    
    def __mul__(self, k):
        return self.__class__(self.x * k, self.y * k)
    
    def __truediv__(self, k):
        return self.__class__(self.x / k, self.y / k)
    
    def __floordiv__(self, k):
        return self.__class__(self.x // k, self.y // k)
    