#pragma once
#include "Framebuffer.h"
/*
  /////////////////////////
 // Post Process Buffer //
/////////////////////////

This class is for creating a double buffer for our post processing effects.
By using 2 framebuffers together, we can prevent the issue of trying to
read and write to the same framebuffer at the same time. Doing so can cause
artifacts, especially for effects that rely on sampling from an adjacent pixel!
Such artifacts are caused from the GPU not computing every single fragment at
the same time. This means that certain fragments may read from the buffer,
and overwrite the pixel in the buffer before other fragments have started.
Example Image: https://gyazo.com/6fcd99e5360d8edec003a7c8a1a5a404

The solution is to write to a different buffer than the one being read. In
order to chain multiple post processing effects together, the buffers being
read and drawn to must be swapped after each fullscreen quad draw call.
The purpose of this class is to simplify that! For each post processing effect,
we already know that there will be certain events EVERY time, so if we put them
inside a single function, this can clean up our code SIGNIFICANTLY, and make it
super easy to add additional effects, and even toggle certain effects on and off
for efficiency purposes! (There's no point in drawing an effect if it doesn't
have an impact on the screen)
By putting the draw call and swap functions together, this prevents the risk of
forgetting steps for specific effects, which could waste countless hours of
debugging why a certain effect isn't showing up.

I'm personally speaking from experience for that last one :P

IMPORTANT: This PostProcess class assumes that the shaders read from the
framebuffer from GL_TEXTURE0. If you require a shader to bind the scene to
a different slot, you would need to add a variable that determines what
slot the read framebuffer is binded to, and use that in place of 0.

*/
class PostProcessBuffer
{
public:
	void init(unsigned int width, unsigned int height);
	void setFormat(GLenum format);
	void clear();
	void reshape(unsigned int width, unsigned int height);

	/*
		For drawing to the post processing buffer at the beginning
		Make sure you have the texture to read from already binded
		before calling this function!
	*/
	void drawToPost();
	/*
		Combo multiple effects together using this function
		By combining the act of binding the write buffer as a framebuffer,
		binding the read buffer as a texture, drawing the fullscreen quad, and
		swapping which buffer is being read and written, the rest of our code
		will get a lot cleaner!
	*/
	void draw();
	/*
		Once we are done with post processing, we can draw the final result to the screen!
		An alternate function you can make yourself is drawing the post buffer to a
		specific framebuffer instead, in case you're drawing to multiple framebuffers
		(useful for anyone doing Split Screen!) and need to combine them together before
		drawing them to the screen.
	*/
	void drawToScreen();
protected:
	void swap();
	bool _IsInit = false;
	GLenum m_pFormat;
	Framebuffer* m_pReadBuffer;
	Framebuffer* m_pWriteBufer;
	Framebuffer m_pFramebuffers[2];
};