#include "PostProcessBuffer.h"

void PostProcessBuffer::init(unsigned int width, unsigned int height)
{
	SAT_ASSERT(_IsInit == false, "Post Processing Buffer already initialized!");
	if (!_IsInit)
	{
		for (int i = 0; i < 2; i++)
		{
			m_pFramebuffers[i].addColorTarget(m_pFormat);
			m_pFramebuffers[i].init(width, height);
			setOpenGLName(GL_TEXTURE,
				m_pFramebuffers->_Color._Tex[0].getID(),
				"PostProcessing FBO #" + std::to_string(i));
		}
		m_pReadBuffer = &m_pFramebuffers[0];
		m_pWriteBufer = &m_pFramebuffers[1];
		_IsInit = true;
	}
}

void PostProcessBuffer::setFormat(GLenum format)
{
	m_pFormat = format;
}

void PostProcessBuffer::clear()
{
	for (int i = 0; i < 2; i++)
	{
		m_pFramebuffers[i].clear();
	}
}

void PostProcessBuffer::reshape(unsigned int width, unsigned int height)
{
	for (int i = 0; i < 2; i++)
	{
		m_pFramebuffers[i].reshape(width, height);
		setOpenGLName(GL_TEXTURE,
			m_pFramebuffers->_Color._Tex[0].getID(),
			"PostProcessing FBO #" + std::to_string(i));
	}
}

void PostProcessBuffer::drawToPost()
{
	SAT_ASSERT(_IsInit == true, "Post Processing Buffer not initialized!");

	m_pWriteBufer->renderToFSQ();
	swap();
}

void PostProcessBuffer::draw()
{
	SAT_ASSERT(_IsInit == true, "Post Processing Buffer not initialized!");

	m_pReadBuffer->bindColorAsTexture(0, 0);
	m_pWriteBufer->renderToFSQ();
	m_pReadBuffer->unbindTexture(0);
	swap();
}

void PostProcessBuffer::drawToScreen()
{
	SAT_ASSERT(_IsInit == true, "Post Processing Buffer not initialized!");

	m_pWriteBufer->unbind();
	m_pReadBuffer->bindColorAsTexture(0, 0);
	Framebuffer::drawFSQ();
	m_pReadBuffer->unbind();
}

void PostProcessBuffer::swap()
{
	std::swap(m_pReadBuffer, m_pWriteBufer);
}