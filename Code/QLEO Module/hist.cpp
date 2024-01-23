#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <termio.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <cutils/properties.h>
#include <sys/utsname.h>
#include <cutils/list.h>
#include <log/log.h>
#include <cutils/sockets.h>
#include <cutils/iosched_policy.h>

#include "hist.h"

bool setInputData(const image_t & src)
{
	if (src.pBuffer == NULL || src.width == 0 || src.height == 0)
		return false;

    if (m_Image.pBuffer != NULL)
    {
        delete[]m_Image.pBuffer;
        m_Image.pBuffer = NULL;
    }
    memcpy(&m_Image, &src, sizeof(m_Image));
	m_Image.pBuffer = new unsigned char[m_Image.iBufSize];
	memcpy(m_Image.pBuffer, src.pBuffer, m_Image.iBufSize);
	return true;
}

bool getOutputData(image_t* src)
{
	if (m_Image.pBuffer == NULL)
		return false;

    if (src->pBuffer != NULL)
    {
        delete[] src->pBuffer;
        src->pBuffer = NULL;
    }
    memcpy(src, &m_Image, sizeof(image_t));
	src->pBuffer = new unsigned char[m_Image.iBufSize];
	memcpy(src->pBuffer, m_Image.pBuffer, m_Image.iBufSize);
	return true;
}

bool updateData()
{
    unsigned char* pData = m_Image.pBuffer;
    for (unsigned long i = 0; i < m_Image.iBufSize; i += m_Image.channel)
    {
        int iGrey = (pData[0] + pData[1] + pData[2])/3;
        auto itr = HistogramMap.find(iGrey);
        if (itr == HistogramMap.end())
            HistogramMap[iGrey] = 1;
        else
        {
            itr->second += 1;
            if (itr->second > m_iMaxValue)
                m_iMaxValue = itr->second;
        }
        pData += m_Image.channel;
    }
    return true;
}