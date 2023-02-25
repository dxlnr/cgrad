import unittest

from mars.engine import Value


class TestVariable(unittest.TestCase):

    def test_value_basics(self):
        # Basic Test if the Value setup is correct.
        a = Value(-4.0)
        self.assertEqual(-4.0, a.data)
        #
        b = Value(2.0)
        c = a + b
        self.assertEqual(-2.0, c.data) 
        # d = a * b + b**3
        # c += c + 1
        # c += 1 + c + (-a)
        # d += d * 2 + (b + a).relu()
        # d += 3 * d + (b - a).relu()
        # e = c - d
        # f = e**2
        # g = f / 2.0
        # g += 10.0 / f
        # # print(f'{g.data:.4f}') 
        # g.backward()


if __name__ == '__main__':
    unittest.main()
