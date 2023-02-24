import unittest

from mars.engine import Value


class TestVariable(unittest.TestCase):

    def test_variable_init(self):
        a = Value()
        print(dir(a))
        self.assertEqual(0.0, a.storage)


if __name__ == '__main__':
    unittest.main()
