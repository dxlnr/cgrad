import unittest

from mars.engine import Value


class TestVariable(unittest.TestCase):

    def test_value_basics(self):
        #
        a = Value(1.0)
        self.assertEqual(1.0, a.data)
        #
        b = Value(2.0)


if __name__ == '__main__':
    unittest.main()
