#include "image.h"
#include "util.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>


/****************************************************************************
 * CG2Image                                                                 *
 ****************************************************************************/

CG2Image::CG2Image()
{
	reset();
}

CG2Image::~CG2Image()
{
	clear();
}

void CG2Image::reset()
{
	pixelData=nullptr;
	width=0;
	height=0;
	channels=0;
}

void CG2Image::clear()
{
	if (pixelData) {
		stbi_image_free(pixelData);
	}
	reset();
}

void CG2Image::create(int w, int h, int c)
{
	pixelData= reinterpret_cast<uint8_t*>(malloc(static_cast<size_t>(w * h * c)));
	width=w;
	height=h;
	channels=c;
}

void CG2Image::fill(const uint8_t* color, bool sRGB)
{
	int x,y;
	this->is_sRGB = sRGB;
	for (x=0; x<width; x++) {
		memcpy(pixelData + (x*channels), color, static_cast<size_t>(channels));
	}
	for (y=1; y<height; y++) {
		memcpy(pixelData, pixelData + (y*width*channels), static_cast<size_t>(width*channels));
	}
}

bool CG2Image::load(const std::string& path)
{
	clear();	
	stbi_set_flip_vertically_on_load(1);
	if(stbi_is_hdr(path.c_str())){
		this->f_pixelData = stbi_loadf(path.c_str(),&width,&height,&channels,0);
		is_HDR = true;
		is_sRGB = false;
	}
	else{
		this->pixelData = stbi_load(path.c_str(),&width,&height,&channels,0);
		is_HDR = false;
		is_sRGB = true;
	}
	if(!pixelData)
	{
		warn("Unable to load image file %s!",path.c_str());
		return  false;
	}
	return true;
}

bool CG2Image::loadLinear(const std::string& path)
{
	bool to_ret = load(path);
	is_sRGB = false;
	return to_ret;
}


bool CG2Image::writeTGA(const std::string& path) const
{
	return stbi_write_tga(path.c_str(),width,height,channels,pixelData) != 0;
}
