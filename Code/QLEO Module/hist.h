#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <map>
#include "emd.h"

typedef struct image {
	unsigned char* pBuffer;
	int iBufSize;
	int width;
	int height;
	int channel;
}image_t;

std::map<int, int>   HistogramMap;
int m_iMaxValue;
image_t	m_Image;


bool setInputData(const image_t& src);
bool updateData(); 
const std::map<int, int>& getHistogramMap() { return  HistogramMap; }
int getMaxHistogramValue() { return m_iMaxValue; }
bool setAlgType(int) { return true; }
bool getOutputData(image_t* src);

#endif