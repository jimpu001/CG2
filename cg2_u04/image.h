#ifndef CG2_IMAGE_H
#define CG2_IMAGE_H

#include <glad/glad.h>
#include <string>

class CG2Image {
private:
	union{
		uint8_t*pixelData;
		float* f_pixelData;
	};
	int width;
	int height;
	int channels;
	bool is_sRGB;
	bool is_HDR;
	void reset();
public:
	CG2Image();
	CG2Image(const char *filename, bool linear=false) {
		reset();
		if (linear) {
			loadLinear(filename);
		} else {
			load(filename);
		}
	}
	~CG2Image();
	void clear();
	void create(int w, int h, int c=4);
	void fill(const uint8_t *color, bool is_sRGB = true);

	// Will load any png, jpg, bmp, tga and gif
	bool load(const std::string& path);

	// Will load any png, jpg, bmp, tga and gif as linear data
	bool loadLinear(const std::string& path);

	bool writeTGA(const std::string& path) const;

	int getWidth() const {
		return width;
	}
	int getHeight() const {
		return height;
	}
	int getChannelCount() const {
		return channels;
	}
	uint8_t *getPixels() {
		return pixelData;
	}
	const uint8_t *getPixels() const {
		return pixelData;
	}
	bool isSRGB() const {
		return is_sRGB;
	}
	bool isHDR() const {
		return is_HDR;
	}

};

#endif // CG2_IMAGE_H
