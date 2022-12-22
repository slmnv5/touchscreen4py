# touchscreen4py

Linux shared library for Python.

This library is used with (Pepelats audio looper)[https://github.com/slmnv5/pepelats] written in Python. 

It provides graphical output and can read click events from LCD touch screen. It uses framebuffer and does not need X-server.
As an examle, one exported function reads word clicked on a screen, this allows to make simple menu in Python by printing text and reading user input.

