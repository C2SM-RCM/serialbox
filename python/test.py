#!/usr/bin/env python
#This file is released under terms of BSD license`
#See LICENSE.txt for more information

import unittest

class Test(unittest.TestCase):

	def test_load_serializer(self):
		from serialbox import Serializer

	def test_load_visualizer(self):
		from serialbox import Visualizer

if __name__ == '__main__':
	unittest.main()