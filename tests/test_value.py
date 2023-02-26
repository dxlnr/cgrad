import unittest

from mars.engine import Value


class TestValue(unittest.TestCase):

    def test_value_basics(self):
        # Basic Test if the Value setup is correct.
        a = Value(-4.0)
        self.assertEqual(-4.0, a.data)
    
    def test_value_add(self):
        # Basic Addition
        a = Value(-4.0)
        b = Value(2.0)
        c = a + b
        self.assertEqual(-2.0, c.data) 
    
    def test_value_all_ops(self):
        # Additional Operations
        a = Value(-4.0)
        b = Value(2.0)
        c = a * b + b**3
        self.assertEqual(0.0, c.data)
       
    def test_value_relu(self):
        # Relu Operation
        a = Value(-4.0)
        b = Value(2.0)
        d = a * b + b**3
        d += d * 2 + (a + b).relu()
        self.assertEqual(0.0, d.data)
        d += 3 * d + (b - a).relu()
        self.assertEqual(6.0, d.data)
       
    def test_basic_backward(self):
        # Backward Pass
        a = Value(-4.0)
        b = Value(2.0)
        c = b**3
        d = a * b + c 
        d.backward()
        self.assertEqual(2.0, a.grad)
       
    def test_self_ref(self):
        # Testing the variable computed by itself.
        # TODO: Needs a fix.
        a = Value(-4.0)
        b = Value(2.0)
        c = b**3
        c += c + 1
        c.backward()
        self.assertEqual(0.0, b.grad())
    
    def test_backward(self):
        pass
        # f = e**2
        # g = 2.0 / f
        # g = f / 2.0
        # g += 10.0 / f
        # e.backward()
        # self.assertEqual()

if __name__ == '__main__':
    unittest.main()
