import unittest

from mars.engine import Value


class TestValue(unittest.TestCase):

    def test_value_basics(self):
        # Basic Test if the Value setup is correct.
        a = Value(-4.0)
        self.assertEqual(-4.0, a.data)
        # Basic Addition
        b = Value(2.0)
        c = a + b
        self.assertEqual(-2.0, c.data) 
        # Additional Operations
        d = a * b + b**3
        self.assertEqual(0.0, d.data)
        # Relu Operation
        c += c + 1
        c += 1 + c + (-a)
        d += d * 2 + (b + a).relu()
        self.assertEqual(0.0, d.data)
        d += 3 * d + (b - a).relu()
        self.assertEqual(6.0, d.data)
        # Backward Pass
        e = c - d
        f = e**2
        g = 2.0 /f

        # g = f / 2.0
        # g += 10.0 / f
        # g.backward()
        # self.assertEqual()

if __name__ == '__main__':
    unittest.main()
