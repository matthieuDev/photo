#ifndef USEFUL_H
#define USEFUL_H

#include <string> 
#include <vector>

;
std::string createFileName(int i){
	if (i < 10){
		return "image000" + std::to_string(i) + ".bmp";
	}
	else if (i < 100){
		return "image00" + std::to_string(i) + ".bmp";
	}
	else if (i < 1000){
		return "image0" + std::to_string(i) + ".bmp";
	}
	else {
		return "image" + std::to_string(i) + ".bmp";
	}
};

class intensityAndimage {
public :
	int intensity, image;
	intensityAndimage(int in, int im){ intensity = in; image = im; };
	intensityAndimage(){  };
	bool operator() (intensityAndimage i, intensityAndimage j) {
		return (i.intensity < j.intensity);
	};
	
};



#endif // !1


