#include "PostProcessBuffer.h"

void PostProcessBuffer::init(unsigned width, unsigned height)
{
	SAT_ASSERT(_IsInit == false, "Post Processing Buffer already initialized!");
	SAT_ASSERT(m_pFormat != GL_NONE, "Post Processing texture \"Format\" not set!");
	if (!_IsInit)
	{
		for (int i = 0; i < 2; ++i)
		{
			m_pFramebuffers[i].addColorTarget(GL_R11F_G11F_B10F);
			m_pFramebuffers[i].init(width, height);
			setOpenGLName(GL_TEXTURE, m_pFramebuffers->_Color._Tex[0].getID(), "PostProcessing FBO #" + std::to_string(i));
		}
		m_pReadBuffer = &m_pFramebuffers[0];
		m_pWriteBuffer = &m_pFramebuffers[1];

		_IsInit = true;
	}
}

void PostProcessBuffer::clear()
{
	for (int i = 0; i < 2; ++i)
	{
		m_pFramebuffers[i].clear();
	}
}

void PostProcessBuffer::reshape(unsigned width, unsigned height)
{
	for (int i = 0; i < 2; ++i)
	{
		m_pFramebuffers[i].reshape(width, height);
		setOpenGLName(GL_TEXTURE, m_pFramebuffers->_Color._Tex[0].getID(), "PostProcessing FBO #" + std::to_string(i));
	}
}

void PostProcessBuffer::setFormat(GLenum format)
{
	m_pFormat = format;
}

void PostProcessBuffer::drawToPost()
{
	SAT_ASSERT(_IsInit == true, "Post Processing Buffer not initialized!");

	m_pWriteBuffer->renderToFSQ();
	swap();
}

void PostProcessBuffer::draw()
{
	SAT_ASSERT(_IsInit == true, "Post Processing Buffer not initialized!");

	//swap();
	m_pReadBuffer->bindColorAsTexture(0, 0);
	m_pWriteBuffer->renderToFSQ();
	m_pReadBuffer->unbindTexture(0);
	swap();
}

void PostProcessBuffer::drawToScreen()
{
	SAT_ASSERT(_IsInit == true, "Post Processing Buffer not initialized!");

	m_pReadBuffer->bindColorAsTexture(0, 0);
	Framebuffer::drawFSQ();
	m_pReadBuffer->unbindTexture(0);

	// No need to swap since we didn't change what the last buffer to be written to is
}

void PostProcessBuffer::swap()
{
	std::swap(m_pReadBuffer, m_pWriteBuffer);
}