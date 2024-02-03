# Sokol Draw Image Example
This is just a very simple example of how to draw an arbitrary bitmap image to a [`sokol_app.h`](https://github.com/floooh/sokol/blob/master/sokol_app.h) window using streaming images.

The image drawn here fills the entire window, but you could easily modify the code to draw it at a specific location and size.

It took me several hours to figure this out properly when I needed it for another project (probably because I'm not very familiar with graphics programming though), so I thought I'd share it here in case it might be useful to anyone else.