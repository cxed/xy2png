xy2png
======

This program simply inputs two numbers on lines from standard input and plots
them onto a PNG. That's it. Combined with Unix tricks it is very powerful and
because it is minimalistic C, it is very fast. So if you have some process
spewing out zillions of numbers, this is a reasonable way to plot them all.

Note that this doesn't put fancy labels or other decorations on the
image - just the data. Decorate it in Inkscape or something like that.
Or pipe the necessary decoration dots. Or don't!

This program was first created to analyze DNS data which is a good
example to see how it can be used.
[Here is a link to that whole story](http://xed.ch/b/2019/0213.html).

Although this program has _almost_ no dependencies, if you want PNGs,
you probably are going to need **libpng**.
Make sure you have these packages (probably):

```
apt install libpng-dev libpng++-dev
```

Compile with something like this:

```
gcc xy2png.c -o xy2png -lpng
```
